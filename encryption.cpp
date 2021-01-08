#include "encryption.h"

#include <iostream>
#include <fstream>


nlohmann::json readDB(const std::string &filename, const std::string &passHash) {
    using namespace std;
    using json = nlohmann::json;

    ifstream fin(filename, ios_base::binary | ios::in);

    string buffer;
    fin.seekg(0, ios_base::end);
    buffer.resize(fin.tellg());
    fin.seekg(0, ios_base::beg);
    fin.read(&buffer[0], buffer.size());

    fin.close();

    crypt(&buffer, passHash);
    decode(&buffer, passHash);

    try {
        auto jsData = json::parse(buffer);
        return jsData;
    } catch (nlohmann::detail::parse_error &e) {
        PRINTERR("Пароль не верен.\nВыход из приложения...");
        exit(1);
    }
}

void crypt(std::string *buffer, const std::string &passHash) {
    for (int i = 0; i < buffer->size(); i++) {
        (*buffer)[i] = (*buffer)[i] ^ passHash[i % passHash.size()];
    }
}

void encode(std::string *text, std::string key) {
    std::string symbols = ALL_SYMBOLS;
    int indText, indKey = 0;
    for (indText = 1; indText < text->size(); indText++) {
        indKey %= key.size();
        indKey++;
        (*text)[indText] = symbols[(symbols.find((*text)[indText]) + symbols.find(key[indKey])) % symbols.size()];
    }
}

void decode(std::string *text, std::string key) {
    std::string symbols = ALL_SYMBOLS;
    int indText, indKey = 0;
    for (indText = 1; indText < text->size(); indText++) {
        indKey %= key.size();
        indKey++;
        (*text)[indText] = symbols[(symbols.find((*text)[indText]) - symbols.find(key[indKey]) + symbols.size()) %
                                   symbols.size()];
    }
}

void write(const std::string &filename, const nlohmann::json &jsData, const std::string &passHash) {
    using namespace std;
    using json = nlohmann::json;

    PRINT("Сохранение и выход...");
    string buffer = jsData.dump();

    cout << jsData.dump(4) << endl;

    encode(&buffer, passHash);
    crypt(&buffer, passHash);

    ofstream fout(filename, ios_base::binary | ios::out);
    fout << buffer;
    fout.close();
}

void checkMaster(const nlohmann::json &dataBase, const std::string &masterPassword) {
    std::cout << dataBase[MASTERPASSWORD] << std::endl;
    if (!dataBase[MASTERPASSWORD].is_null() && dataBase[MASTERPASSWORD] == masterPassword) { // not null and ==
        PRINT("Вход успешно выполнен!");
    } else {
        PRINTERR("Пароль не верен.\nВыход из приложения...");
        exit(1);
    }
}

std::string myHash(const std::string &masterPassword) {
    unsigned char p = 255;
    unsigned long long int temp = 0, p_pow = 1;
    for (char ch : masterPassword) {
        temp += ch * p_pow;
        p_pow *= p;
    }
    return std::to_string(temp);
}

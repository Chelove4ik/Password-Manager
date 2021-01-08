#define BACKSPACE 8
#define RETURN 13

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <conio.h>

#include "parsing.h"
#include "encryption.h"
#include "clipboard.h"

const std::string comWith[] = {"get", "add", "del", "chk"};

void checkConfigPATH(std::string *configPATH) {
    PRINT("Проверка доступа к конфигурационному файлу...");
    if (access(configPATH->c_str(), R_OK) == -1) {
        PRINT("Ошибка доступа к конфигурационному файлу.\nВыход...");
        exit(1);
    }
    PRINT("Успешно");
}

void login(std::string *masterPassword, std::string *configPATH, nlohmann::json *dataBase, std::string *dbPATH) {
    using namespace std;
    using json = nlohmann::json;

    json jsData;

    checkConfigPATH(configPATH);

    ifstream fin(*configPATH);
    try {
        jsData = json::parse(fin);
    } catch (nlohmann::detail::parse_error) {
        PRINTERR("Ошибка в конфиг файле (" + *configPATH + ").\nВыход...");
        exit(1);
    }
    if (!jsData[URL].is_null()) { // not null
        *dbPATH = jsData[URL];
        if (access(jsData[URL].get<string>().c_str(), R_OK) != -1) {
            *masterPassword = myHash(readPassword());
            *dataBase = readDB(jsData[URL], *masterPassword);
            checkMaster(*dataBase, *masterPassword);
        } else {
            PRINT("Базы данных с паролями не существует. Она будет создана.");
            *masterPassword = myHash(readPassword());
            (*dataBase)[MASTERPASSWORD] = *masterPassword;
            PRINT("Не забывайте свой пароль!");
        }
    } else {
        PRINTERR("Конфигурационный файл содержит неверный путь до базы данных. Выход...");
        exit(1);
    }
}


void stop(const std::string &filename, const nlohmann::json &jsData, const std::string &passHash) {
    write(filename, jsData, passHash);
    exit(0);
}

std::string readPassword() {
    std::cout << "Введите пароль: ";
    std::string password;
    unsigned char ch;

    while ((ch = getch()) != RETURN) {
        if (ch == BACKSPACE) {
            if (password.length() != 0) {
                std::cout << "\b \b";
                password.resize(password.length() - 1);
            }
        } else if (ch == 0 || ch == 224) // handle escape sequences
        {
            getch(); // ignore non printable chars
            continue;
        } else {
            password += ch;
        }
    }
    std::cout << std::endl;

    return password;
}


void parseCMD(const std::string &cmd, std::string *command, std::string *argument) {
    *command = cmd;
    for (const auto &item: comWith)
        if (cmd.find(item) != -1) {
            *command = cmd.substr(0, cmd.find(' '));
            *argument = cmd.substr(cmd.find(' ') + 1);
            break;
        }
    command->erase(std::remove_if(command->begin(), command->end(), [](unsigned char x) { return std::isspace(x); }),
                   command->end());
    argument->erase(std::remove_if(argument->begin(), argument->end(), [](unsigned char x) { return std::isspace(x); }),
                    argument->end());

}

bool parseArgument(const std::string &argument, std::string *site, std::string *login) {
    int argFind = argument.find(':');
    if (argFind != -1) {
        *site = argument.substr(0, argFind);
        *login = argument.substr(argFind + 1);
        return true;
    } else {
        PRINTERR("Ошибка в аргументе. Напишите help для помощи.");
        return false;
    }
}

void parse(const std::string &cmd, nlohmann::json *dataBase, std::string *masterPassword) {
    std::string command;
    std::string argument;

    parseCMD(cmd, &command, &argument);

    if (command == "master")
        changeMaster(dataBase, masterPassword);
    else if (command == "get")
        get(dataBase, argument);
    else if (command == "chk") {
        char code = chk(dataBase, argument);
        if (1 == code)
            PRINT("Пароль в базе данных существует");
        else if (0 == code)
            PRINT("Пароля в базе данных не существует");
    } else if (command == "add")
        add(dataBase, argument);
    else if (command == "del")
        del(dataBase, argument);
    else if (command == "q" || command == "quit" || command == "exit")
        std::cin.seekg(EOF);
    else if (command == "help")
        help();
    else if (!command.empty())
        PRINTERR("Команда не распознана");
}


void changeMaster(nlohmann::json *dataBase, std::string *masterPassword) {
    *masterPassword = myHash(readPassword());
    (*dataBase)[MASTERPASSWORD] = *masterPassword;
}

void get(nlohmann::json *dataBase, const std::string &argument) {
    std::string site, login;
    if (!parseArgument(argument, &site, &login))
        return;
    if (1 == chk(dataBase, site, login)) {
        if (setClipboard((*dataBase)[site][login]))
            PRINT("Пароль был скопирован в буфер обмена");
        else
            PRINTERR("Произошла ошибка при копировании в буфер обмена");
    } else
        PRINTERR("Пароль для логина \"" + login + "\" для сайта \"" + site + "\" не сохраненён");
}

char chk(nlohmann::json *dataBase, const std::string &argument) {
    std::string site, login;
    if (!parseArgument(argument, &site, &login))
        return (char) 2;
    return chk(dataBase, site, login);
}

char chk(nlohmann::json *dataBase, const std::string &site, const std::string &login) {
    if (dataBase->find(site) != dataBase->end() && (*dataBase)[site].find(login) != (*dataBase)[site].end()) {
        return (char) 1;
    } else
        return (char) 0;
}

void add(nlohmann::json *dataBase, const std::string &argument) {
    std::string site, login;
    if (!parseArgument(argument, &site, &login))
        return;
    (*dataBase)[site][login] = readPassword();
    PRINT("Пароль записан");
}

void del(nlohmann::json *dataBase, const std::string &argument) {
    if (argument.find(':') == -1) {
        if (dataBase->find(argument) == dataBase->end()) {
            PRINTERR("Ошибка удаления. Паролей для этого сайта не существует");
            return;
        } else {
            dataBase->erase(argument);
            PRINT("Удаление прошло успешно");
        }
    } else {
        std::string site, login;
        if (!parseArgument(argument, &site, &login))
            return;
        del(dataBase, site, login);
    }
}

void del(nlohmann::json *dataBase, const std::string &site, const std::string &login) {
    if (0 == chk(dataBase, site, login)) {
        PRINTERR("Ошибка удаления. Пароля для этого логина для этого сайта не существует");
        return;
    }
    (*dataBase)[site].erase(login);

    if ((*dataBase)[site].empty())
        dataBase->erase(site);
    PRINT("Удаление прошло успешно");
}


void help() {
    PRINT("Русские буквы не поддерживаются!\n"
          "master\n"
          "Устанавливает новый мастер-пароль\n"
          "Пароль вводится с клавиатуры\n"
          "\n"
          "get SITE:LOGIN\n"
          "Копирует в буфер обмена пароль для сайта SITE и логина LOGIN\n"
          "\n"
          "add SITE:LOGIN\n"
          "Добавляет в базу пароль для сайта SITE, логина LOGIN\n"
          "Пароль вводится с клавиатуры\n"
          "\n"
          "del SITE:LOGIN\n"
          "Удаляет из базы пароль для сайта SITE, логина LOGIN\n"
          "\n"
          "chk SITE:LOGIN\n"
          "Проверяет наличие пароля для сайта SITE, логина LOGIN\n"
          "\n"
          "help\n"
          "Выводит доступные команды и их описание\n"
          "\n"
          "q / quit / exit\n"
          "Завершает работу программы");
}
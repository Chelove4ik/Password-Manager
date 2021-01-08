#include <iostream>
#include <windows.h>

#include "parsing.h"
#include "encryption.h"


int main(int argc, char *argv[]) {
    using namespace std;

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    std::string configPATH = "passmng.conf";
    std::string masterPassword;
    std::string dbPATH = "db.enc";
    nlohmann::json dataBase;
    bool *needExit = new bool(false);

    for (int i = 1; i < argc; i++) {
        if ((string) argv[i] == "--help") {
            help();
        } else if ((string) argv[i] == "--config") {
            if (i + 1 < argc) {
                configPATH = argv[i + 1];
                i++;
            } else {
                PRINTERR("Нет пути до конфиг файла. Используется стандартный.");
            }
        } else {
            if (!*needExit)
                login(&masterPassword, &configPATH, &dataBase, &dbPATH);
            *needExit = true;
            parse((string) argv[i] == "master" ? (string) argv[i] : (string) argv[i] + ' ' + argv[i + 1], &dataBase,
                  &masterPassword);
            i++;
        }
    }
    if (*needExit) {
        stop(dbPATH, dataBase, masterPassword);
    }
    delete needExit;

    login(&masterPassword, &configPATH, &dataBase, &dbPATH);

    string cmd;

    while (getline(cin, cmd)) {
        parse(cmd, &dataBase, &masterPassword);
    }

    stop(dbPATH, dataBase, masterPassword);

    return 0;
}

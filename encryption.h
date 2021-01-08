#ifndef PASSWORDS_MANAGER_ENCRYPTION_H
#define PASSWORDS_MANAGER_ENCRYPTION_H

#define PRINT(message) std::cout << (message) << std::endl
#define PRINTERR(message) std::cerr << (message) << std::endl

#define ALL_SYMBOLS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ{}0123456789\\|/!@#№$%^&?:;*()-=_+'\" ,.[]"
#define MASTERPASSWORD "$master8()- }0PasswordYZ=_+'\"&?:;"
#define URL "url"

#include <string>
#include "json/single_include/nlohmann/json.hpp"

nlohmann::json readDB(const std::string &filename, const std::string &passHash);

void crypt(std::string *buffer, const std::string &passHash);

void encode(std::string *text, std::string key);

void decode(std::string *text, std::string key);

void write(const std::string &filename, const nlohmann::json &jsData, const std::string &passHash);

void checkMaster(const nlohmann::json &dataBase, const std::string &masterPassword);

std::string myHash(const std::string &masterPassword);

#endif //PASSWORDS_MANAGER_ENCRYPTION_H

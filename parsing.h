#ifndef PASSWORDS_MANAGER_PARSING_H
#define PASSWORDS_MANAGER_PARSING_H

#include <string>
#include "json/single_include/nlohmann/json.hpp"

void checkConfigPATH(std::string *configPATH);

void login(std::string *masterPassword, std::string *configPATH, nlohmann::json *dataBase, std::string *dbPATH);

void parse(const std::string &cmd, nlohmann::json *dataBase, std::string *masterPassword);

bool parseArgument(const std::string &argument, std::string *site, std::string *login);

std::string readPassword();

void changeMaster(nlohmann::json *dataBase, std::string *masterPassword);

void stop(const std::string &filename, const nlohmann::json &jsData, const std::string &passHash);

void get(nlohmann::json *dataBase, const std::string &argument);

char chk(nlohmann::json *dataBase, const std::string &argument);

char chk(nlohmann::json *dataBase, const std::string &site, const std::string &login);

void add(nlohmann::json *dataBase, const std::string &argument);

void del(nlohmann::json *dataBase, const std::string &argument);

void del(nlohmann::json *dataBase, const std::string &site, const std::string &login);

void help();

#endif //PASSWORDS_MANAGER_PARSING_H

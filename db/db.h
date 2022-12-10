#pragma once
#include <pqxx/pqxx>

using namespace pqxx;


result getAllUsers();

bool tryToCreateUser(std::string username, std::string email, std::string password, std::string &exceptionText);

result getUserForAuthentication(std::string email);

result getToken(std::string userId, std::string isAccess);

void saveToken(std::string value, std::string userId, std::string isAccess, std::string createTime);

void updateToken(std::string id, std::string value, std::string createTime);

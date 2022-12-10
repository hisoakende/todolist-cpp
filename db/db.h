#pragma once
#include <pqxx/pqxx>

using namespace pqxx;


result getAllUsers();

bool tryToCreateUser(std::string username, std::string email, std::string password, std::string &exceptionText);

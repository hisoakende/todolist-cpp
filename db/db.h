#pragma once
#include <pqxx/pqxx>

using namespace pqxx;


result getAllUsers();

result getUserByEmail(std::string email);

result getUserById(std::string userId);

bool tryToCreateUser(std::string username, std::string email, std::string password, std::string &exceptionText);

bool tryToUpdateUser(std::string id, std::string username, std::string email, 
                     std::string password, std::string isAdmin, std::string &exceptionText);

result getTokenByUserId(std::string userId, std::string isAccess);

result getTokenAndUserByValue(std::string value, std::string isAccess);

void saveToken(std::string value, std::string userId, std::string isAccess, std::string createTime);

void updateToken(std::string id, std::string value, std::string createTime);

void deleteTokens(std::string userId);

result getAllCategories();

result getCategory(std::string categoryId);

void createCategory(std::string name);

void deleteCategory(std::string id);

void updateCategory(std::string id, std::string name);

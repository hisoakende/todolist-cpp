#pragma once
#include "../db/db.h"
#include <map>
#include <regex>
#include <drogon/drogon.h>
#include <pqxx/pqxx>

typedef std::function<bool(std::string)> checkFunction;
typedef std::function<pqxx::result(std::string)> getDataFunction;


bool processTheDataFromTheRequest(std::map<std::string, std::string> &userData, 
                                  Json::Value &json, std::shared_ptr<Json::Value> requestBody);

void processTheDataFromTheRequest(std::map<std::string, std::string> &data, std::shared_ptr<Json::Value> requestBody);

bool isValidUserCreateOrUpdateData(std::map<std::string, std::string> userData, Json::Value &json);

bool checkQueryParam(std::string param, Json::Value &json, pqxx::result &obj, getDataFunction func);

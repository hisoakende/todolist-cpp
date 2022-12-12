#pragma once
#include "../db/db.h"
#include <map>
#include <regex>
#include <drogon/drogon.h>


bool processTheDataFromTheRequest(std::map<std::string, std::string> &userData, 
                                  Json::Value &json, std::shared_ptr<Json::Value> requestBody);

void processTheDataFromTheRequest(std::map<std::string, std::string> &data, std::shared_ptr<Json::Value> requestBody);

bool isValidUserCreateOrUpdateData(std::map<std::string, std::string> userData, Json::Value &json);

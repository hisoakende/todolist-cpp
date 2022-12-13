#pragma once
#include "../db/db.h"
#include <map>
#include <regex>
#include <drogon/drogon.h>
#include <pqxx/pqxx>

typedef std::function<bool(std::string)> checkFunction;
typedef std::function<pqxx::result(std::string)> getDataFunction;
typedef std::vector<std::map<std::string, std::string>> rowsView;
typedef std::map<std::string, std::string> rowView;


bool processTheDataFromTheRequest(rowView &userData, 
                                  Json::Value &json, std::shared_ptr<Json::Value> requestBody);

void processTheDataFromTheRequest(rowView &data, std::shared_ptr<Json::Value> requestBody);

bool isValidUserCreateOrUpdateData(rowView userData, Json::Value &json);

bool checkQueryParam(std::string param, Json::Value &json, pqxx::result &obj, getDataFunction func);

bool checkNoteDataFromRequest(rowView noteData, Json::Value &json);


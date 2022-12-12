#pragma once
#include "../db/db.h"
#include <map>
#include <pqxx/pqxx>
#include <drogon/drogon.h>
#include <openssl/sha.h>

typedef std::function<void(const drogon::HttpResponsePtr &)> Callback;
typedef std::vector<std::map<std::string, std::string>> rowsView;
typedef std::map<std::string, std::string> rowView;


void printRequestInfo(drogon::HttpRequestPtr request, drogon::HttpStatusCode status);

void createJsonForRow(Json::Value &json, rowView obj);

void createJsonBody(Json::Value &json, rowsView data);

drogon::HttpResponsePtr processTheResponseIfRequestBodyIsEmpty();

std::string getHashedString(std::string password);

void processUniqueViolationTextForUserCreation(std::string exceptionText, Json::Value &json);

std::string generateToken(std::string userId, std::string isAccessToken);

std::string processTokenHandling(std::string userId, std::string isAccessToken);

drogon::HttpResponsePtr processResponse(drogon::HttpRequestPtr request, Json::Value json, drogon::HttpStatusCode status);

drogon::HttpResponsePtr processResponse(drogon::HttpRequestPtr request, drogon::HttpStatusCode status);

std::map<std::string, std::string> authorizeUser(std::string userToken, std::string isAccess, Json::Value &json);

rowView createObjFromDb(rowView objData, pqxx::row dbRow);

rowsView createObjsFromDb(rowView objData, pqxx::result dbData);

#pragma once
#include "../db/db.h"
#include <iostream>
#include <pqxx/pqxx>
#include <drogon/drogon.h>
#include <openssl/sha.h>


typedef std::function<void(const drogon::HttpResponsePtr &)> Callback;


void printRequestInfo(drogon::HttpRequestPtr request, drogon::HttpStatusCode status);

void createJsonBody(Json::Value &json, pqxx::result &data);

drogon::HttpResponsePtr processTheResponseIfRequestBodyIsEmpty();

std::string getHashedString(std::string password);

void processUniqueViolationTextForUserCreation(std::string exceptionText, Json::Value &json);

std::string generateToken(std::string userId, std::string isAccessToken);

std::string processTokenHandling(std::string userId, std::string isAccessToken);

drogon::HttpResponsePtr processResponse(drogon::HttpRequestPtr request, Json::Value json, drogon::HttpStatusCode status);

drogon::HttpResponsePtr processResponse(drogon::HttpRequestPtr request, drogon::HttpStatusCode status);

std::pair<bool, pqxx::result> getTokenData(std::string userToken, std::string isAccess, Json::Value &json);

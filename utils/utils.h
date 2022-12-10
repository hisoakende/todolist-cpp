#pragma once
#include "../db/db.h"
#include <iostream>
#include <pqxx/pqxx>
#include <drogon/drogon.h>
#include <openssl/sha.h>


typedef std::function<void(const drogon::HttpResponsePtr &)> Callback;


void printRequestInfo(const drogon::HttpRequestPtr &request);

void createJsonBody(Json::Value &json, pqxx::result &data);

void processTheRequestWithTheEmptyBody(Json::Value json, drogon::HttpResponsePtr &response);

std::string getHashedString(std::string password);

void processUniqueViolationTextForUserCreation(std::string exceptionText, Json::Value &json);

std::string generateToken(std::string tokenType);

std::string processTokenHandling(std::string userId, std::string tokenType);

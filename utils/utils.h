#pragma once
#include <pqxx/pqxx>
#include <drogon/drogon.h>
#include <openssl/sha.h>


typedef std::function<void(const drogon::HttpResponsePtr &)> Callback;


void printRequestInfo(const drogon::HttpRequestPtr &request);

void createJsonBody(Json::Value &json, pqxx::result &data);

void processTheRequestWithTheEmptyBody(Json::Value json, drogon::HttpResponsePtr &response);

std::string getHashedPassword(std::string password);

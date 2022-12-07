#pragma once
#include <pqxx/pqxx>
#include <drogon/drogon.h>


void printRequestInfo(const drogon::HttpRequestPtr &request);

void createJsonBody(Json::Value &json, pqxx::result &data);

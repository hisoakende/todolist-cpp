#include "utils.h"
#include <iostream>
#include <drogon/drogon.h>
#include <pqxx/pqxx>


void printRequestInfo(const drogon::HttpRequestPtr &request) {
    std::cout << "REQUEST: " << request->getMethodString() << " " << request->getPath() << std::endl;
}


// Create json for one tuple
void createJsonForRow(pqxx::row &row, Json::Value &objJson) {
    for (auto field : row) {
        if (pqxx::to_string(field.name()) != "password") {
            objJson[field.name()] = pqxx::to_string(field);
        }
    }
}


// Create json for all tuples
void createJsonBody(Json::Value &json, pqxx::result &data) {
    for (auto row : data) {
        Json::Value objJson;
        createJsonForRow(row, objJson);
        json.append(objJson);
    }
}

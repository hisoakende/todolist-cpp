#include "utils.h"
#include <iostream>

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


void processTheRequestWithTheEmptyBody(Json::Value json, drogon::HttpResponsePtr &response) {
    json["message"] = "body is required";
    response = drogon::HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
}


std::string getHashedPassword(std::string password) {
    int passwordSize = password.size();
    unsigned char *unhashedPasswordCStrPtr = (unsigned char *) password.c_str();
    unsigned char buffer[100];
    unsigned char *hash = SHA256(unhashedPasswordCStrPtr, passwordSize, buffer);
    std::stringstream hashedPassword;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        hashedPassword << std::hex << std::setw(2) << std::setfill('0') <<  static_cast<int>(hash[i]);
    }
    return hashedPassword.str();
}

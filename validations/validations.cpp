#include "validations.h"
#include <iostream>

typedef std::function<bool(std::string)> checkFunction;


bool processTheDataFromTheRequest(std::map<std::string, std::string> &data, 
                                  Json::Value &json, std::shared_ptr<Json::Value> requestBody) {
    bool dataIsNormal = true;
    for (auto &pair : data) {
        if (requestBody->isMember(pair.first)) {
            pair.second = requestBody->get(pair.first, "").asString();
        } else {
            json[pair.first] = pair.first + " is required";
            dataIsNormal = false;
        }
    }
    return dataIsNormal;
}


void processTheDataFromTheRequest(std::map<std::string, std::string> &data, std::shared_ptr<Json::Value> requestBody) {
    for (auto &pair : data) {
        if (requestBody->isMember(pair.first)) {
            pair.second = requestBody->get(pair.first, "").asString();
        }
    }
}


bool isValidUsername(std::string username) {
    return std::regex_match(username, std::regex("\\S{3,}"));
}


bool isValidEmail(std::string email) {
    return std::regex_match(email, std::regex("\\S{1,64}@\\S+\\.\\S+")) && email.size() <= 254;
}


bool isValidPassword(std::string password) {
    return std::regex_match(password, std::regex("(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{6,20}"));
}


bool isValidUserCreateOrUpdateData(std::map<std::string, std::string> userData, Json::Value &json) {
    std::map<std::string, checkFunction> functions = {{"username", &isValidUsername}, 
                                                      {"email", &isValidEmail}, 
                                                      {"password", &isValidPassword}};
    bool isValidData = true;
    for (auto field : userData) {
        if (!functions[field.first](field.second)) {
            json[field.first] = "invalid field";
            isValidData = false;
        }
    }
    return isValidData;
}

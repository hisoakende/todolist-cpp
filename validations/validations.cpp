#include "validations.h"
#include <iostream>

bool processTheDataFromTheRequest(rowView &data, Json::Value &json, std::shared_ptr<Json::Value> requestBody) {
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


void processTheDataFromTheRequest(rowView &data, std::shared_ptr<Json::Value> requestBody) {
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


bool runFunctions(rowView userData, Json::Value &json, std::map<std::string, checkFunction> functions) {
    bool isValidData = true;
    for (auto field : userData) {
        if (!functions[field.first](field.second)) {
            json[field.first] = "invalid field";
            isValidData = false;
        }
    }
    return isValidData;
}


bool isValidUserCreateOrUpdateData(rowView userData, Json::Value &json) {
    std::map<std::string, checkFunction> functions = {{"username", &isValidUsername}, 
                                                      {"email", &isValidEmail}, 
                                                      {"password", &isValidPassword}};
    return runFunctions(userData, json, functions);
}


bool checkQueryParam(std::string param, Json::Value &json, pqxx::result &data, getDataFunction func) {
    if (param.find_first_not_of("0123456789") != std::string::npos || param == "") {
        json["message"] = "id is invalid";
        return false;
    }

    data = func(param);
    if (data.size() == 0) {
        json["message"] = "id is invalid";
        return false;
    }

    return true;
}


bool isValidTitle(std::string value) {
    return value.size() <= 255;
}


bool isValidText(std::string value) {
    return true;
}


bool isValidCategoryId(std::string value) {
    return getCategory(value).size() != 0;
}


bool checkNoteDataFromRequest(rowView noteData, Json::Value &json) {
    std::map<std::string, checkFunction> functions = {{"title", &isValidTitle}, 
                                                      {"text", &isValidText}, 
                                                      {"category_id", &isValidCategoryId}};
    return runFunctions(noteData, json, functions);
}

#include "utils.h"


void printRequestInfo(drogon::HttpRequestPtr request, drogon::HttpStatusCode status) {
    std::cout << "REQUEST: HTTP " << request->getMethodString() << " " 
                            << request->getPath() << " RESPONSE CODE: " << status << std::endl;
}


// Create json for one tuple
void createJsonForRow(Json::Value &json, rowView obj) {
    for (auto field : obj) {
        json[field.first] = field.second;
    }
}


// Create json for all tuples
void createJsonBody(Json::Value &json, rowsView data) {
    for (auto obj : data) {
        Json::Value objJson;
        createJsonForRow(objJson, obj);
        json.append(objJson);
        
    }
}


drogon::HttpResponsePtr processTheResponseIfRequestBodyIsEmpty() {
    Json::Value jsonBody;
    jsonBody["message"] = "body is required";
    drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(jsonBody);
    response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
    return response;
}


std::string getHashedString(std::string s) {
    int stringSize = s.size();
    unsigned char *unhashedSLikeCPtr = (unsigned char *) s.c_str();
    unsigned char buffer[100];
    unsigned char *hash = SHA256(unhashedSLikeCPtr, stringSize, buffer);
    std::stringstream hashedString;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        hashedString << std::hex << std::setw(2) << std::setfill('0') <<  static_cast<int>(hash[i]);
    }
    return hashedString.str();
}


void processUniqueViolationTextForUserCreation(std::string exceptionText, Json::Value &json) {
    int usernamePos = exceptionText.find("username");
    if (usernamePos != std::string::npos) {
        json["username"] = "field must be unique";
    }

    int emailPos = exceptionText.find("email");
    if (emailPos != std::string::npos) {
        json["email"] = "field must be unique";
    }
}


std::string generateToken(std::string userId, std::string isAccessToken) {
    std::string s = std::to_string(time(nullptr)) + isAccessToken + userId;
    return getHashedString(s);
}


std::string processTokenHandling(std::string userId, std::string isAccessToken) {
    pqxx::result oldToken = getTokenByUserId(userId, isAccessToken);
    std::string newToken = generateToken(isAccessToken, userId);
    if (oldToken.size() == 0) {
        saveToken(newToken, userId, isAccessToken, std::to_string(time(nullptr)));
    } else {
        updateToken(to_string(oldToken[0]["id"]), newToken, std::to_string(time(nullptr)));
    }
    return newToken;
}


drogon::HttpResponsePtr processResponse(drogon::HttpRequestPtr request, Json::Value json, drogon::HttpStatusCode status) {
    printRequestInfo(request, status);
    drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(json);
    response->setStatusCode(status);
    return response;
}


drogon::HttpResponsePtr processResponse(drogon::HttpRequestPtr request, drogon::HttpStatusCode status) {
    printRequestInfo(request, status);
    drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode(status);
    return response;
}


rowView createObjFromDb(rowView objData, pqxx::row dbRow) {
    for (auto &pair : objData) {
        pair.second = pqxx::to_string(dbRow[pair.first]);
    }
    return objData;
}


rowsView createObjsFromDb(rowView objData, pqxx::result dbData) {
    rowsView r;
    for (auto row : dbData) {
        r.push_back(createObjFromDb(objData, row));
    }
    return r;
}


std::map<std::string, std::string> authorizeUser(std::string userToken, std::string isAccess, Json::Value &json) {
    pqxx::result dbTokenAndUserData = getTokenAndUserByValue(userToken, isAccess);
    std::map<std::string, std::string> userData;

    if (dbTokenAndUserData.size() == 0) {
        json["message"] = "token is invalid";
        return userData;
    }

    int tokenCreateTime = stoi(pqxx::to_string(dbTokenAndUserData[0]["create_time"]));
    if (time(nullptr) - tokenCreateTime >= 3600 && isAccess == "t") {
        json["message"] = "token expired";
        return userData;
    }

    userData = {{"user_id", ""}, {"username", ""}, {"email", ""}, {"is_admin", ""}};
    return createObjFromDb(userData, dbTokenAndUserData[0]);;
}


userAndResponse processAuthorizations(const drogon::HttpRequestPtr &request, Json::Value jsonBody) {
    std::map<std::string, std::string> user;

    std::string userToken = request->getHeader("authorization");
    if (userToken.empty()) {
        jsonBody["message"] = "header \'authorization\' is required";
        return std::pair(user, processResponse(request, jsonBody, drogon::HttpStatusCode::k400BadRequest));
    }

    user = authorizeUser(userToken.substr(7, 64), "t", jsonBody);
    if (user.size() == 0) {
        return std::pair(user, processResponse(request, jsonBody, drogon::HttpStatusCode::k400BadRequest));
    }

    return std::pair(user, drogon::HttpResponse::newHttpResponse());
}


void updateDataInRowView(rowView &old, rowView new_) {
    for (auto pair : new_) {
        if (pair.second != "") {
            old[pair.first] = pair.second;
        } 
    }
}



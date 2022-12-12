#include "utils.h"


void printRequestInfo(drogon::HttpRequestPtr request, drogon::HttpStatusCode status) {
    std::cout << "REQUEST: HTTP " << request->getMethodString() << " " 
                            << request->getPath() << " RESPONSE CODE: " << status << std::endl;
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


std::pair<bool, pqxx::result> getTokenData(std::string userToken, std::string isAccess, Json::Value &json) {
    pqxx::result dbTokenData = getTokenByValue(userToken, isAccess);

    if (dbTokenData.size() == 0) {
        json["message"] = "token is invalid";
        return std::pair(false, dbTokenData);
    }

    int tokenCreateTime = stoi(pqxx::to_string(dbTokenData[0]["create_time"]));
    if (time(nullptr) - tokenCreateTime >= 3600 && isAccess == "t") {
        json["meassage"] = "token expired";
        return std::pair(false, dbTokenData);
    }

    return std::pair(true, dbTokenData);
}

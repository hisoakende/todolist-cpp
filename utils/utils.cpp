#include "utils.h"


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


std::string generateToken(std::string userId, std::string tokenType) {
    std::string s = std::to_string(time(nullptr)) + tokenType + userId;
    return getHashedString(s);
}


std::string processTokenHandling(std::string userId, std::string tokenType) {
    pqxx::result oldToken = getToken(userId, tokenType);
    std::string newToken = generateToken(tokenType, userId);
    if (oldToken.size() == 0) {
        saveToken(newToken, userId, tokenType, std::to_string(time(nullptr)));
    } else {
        updateToken(to_string(oldToken[0]["id"]), newToken, std::to_string(time(nullptr)));
    }
    return newToken;
}

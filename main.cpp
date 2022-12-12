#include "db/db.h"
#include "utils/utils.h"
#include "validations/validations.h"
#include <map>
#include <pqxx/pqxx>
#include <drogon/drogon.h>
#include <openssl/sha.h>

using namespace drogon;

typedef std::function<void(const HttpResponsePtr &)> Callback;


void usersViewGet(const HttpRequestPtr &request, Callback &&callback) {
    Json::Value jsonBody;
    result users = getAllUsers();
    createJsonBody(jsonBody, users);
  	callback(processResponse(request, jsonBody, HttpStatusCode::k200OK));
}


void createUserView(const HttpRequestPtr &request, Callback &&callback) {
    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();

    if (requestBody == nullptr) {
        return callback(processTheResponseIfRequestBodyIsEmpty());
    }

    Json::Value jsonBody;
    std::map<std::string, std::string> userData = {{"username", ""}, {"email", ""}, {"password", ""}};
    bool requestIsNormal = processTheDataFromTheRequest(userData, jsonBody, requestBody);

    if (!requestIsNormal || !isValidUserCreateData(userData, jsonBody)) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }
    
    HttpResponsePtr response;
    std::string uniqueViolationText;

    if (!tryToCreateUser(userData["username"], userData["email"], getHashedString(userData["password"]), uniqueViolationText)) {
        processUniqueViolationTextForUserCreation(uniqueViolationText, jsonBody);
        response = processResponse(request, jsonBody, HttpStatusCode::k400BadRequest);
    } else {
        response = processResponse(request, HttpStatusCode::k201Created);
    }
    
    callback(response);
}


void authenticationView(const HttpRequestPtr &request, Callback &&callback) {
    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();

    if (requestBody == nullptr) {
        return callback(processTheResponseIfRequestBodyIsEmpty());
    }

    Json::Value jsonBody;
    std::map<std::string, std::string> data = {{"email", ""}, {"password", ""}};
    bool requestIsNormal = processTheDataFromTheRequest(data, jsonBody, requestBody);

    if (!requestIsNormal) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    result user = getUserForAuthentication(data["email"]);
    
    if (user.size() == 0) {
        jsonBody["message"] = "no such user exists";
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    if (getHashedString(data["password"]) != to_string(user[0][1])) {
        jsonBody["message"] = "invalid password";
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    } 
    
    jsonBody["refreshToken"] = processTokenHandling(to_string(user[0]["id"]), "f");
    jsonBody["accessToken"] = processTokenHandling(to_string(user[0]["id"]), "t");

    callback(processResponse(request, jsonBody, HttpStatusCode::k200OK));
}


void logoutView(const HttpRequestPtr &request, Callback &&callback) {
    Json::Value jsonBody;
    std::string userToken = request->getHeader("authorization");
    
    if (userToken.empty()) {
        jsonBody["message"] = "header \'authorization\' is required";
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    std::pair<bool, pqxx::result> tokenData = getTokenData(userToken.substr(7, 64), "t", jsonBody);
    if (!tokenData.first) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    deleteTokens(pqxx::to_string(tokenData.second[0]["user_id"]));
    callback(processResponse(request, HttpStatusCode::k200OK));
}


int main() {
    app()
         .registerHandler("/api/users/", &usersViewGet, {Get})
         .registerHandler("/api/users/create_user/", &createUserView, {Post})
         .registerHandler("/api/users/authentication/", &authenticationView, {Post})
         .registerHandler("/api/users/logout/", &logoutView, {Get})
         .loadConfigFile("../config.json")
         .run();
    return 0;
}

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
    printRequestInfo(request);
    HttpResponsePtr response;
    Json::Value jsonBody;
    
    result users = getAllUsers();
    createJsonBody(jsonBody, users);
    response = HttpResponse::newHttpJsonResponse(jsonBody);    
    
  	callback(response);
}


void createUserView(const HttpRequestPtr &request, Callback &&callback) {
    printRequestInfo(request);
    HttpResponsePtr response;
    Json::Value jsonBody;

    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();

    if (requestBody == nullptr) {
        processTheRequestWithTheEmptyBody(jsonBody, response);
        return callback(response);
    }

    std::map<std::string, std::string> userData = {{"username", ""}, {"email", ""}, {"password", ""}};
    bool requestIsNormal = processTheDataFromTheRequest(userData, jsonBody, requestBody);

    if (!requestIsNormal || !isValidUserCreateData(userData, jsonBody)) {
        response = HttpResponse::newHttpJsonResponse(jsonBody);
        response->setStatusCode(HttpStatusCode::k400BadRequest);
        return callback(response);
    }
    
    std::string uniqueViolationText;
    if (!tryToCreateUser(userData["username"], userData["email"], getHashedString(userData["password"]), uniqueViolationText)) {
        processUniqueViolationTextForUserCreation(uniqueViolationText, jsonBody);
        response = HttpResponse::newHttpJsonResponse(jsonBody);
        response->setStatusCode(HttpStatusCode::k400BadRequest);
    } else {
        response = HttpResponse::newHttpResponse();
        response->setStatusCode(HttpStatusCode::k201Created);
    }
    
    callback(response);
}


void authenticationView(const HttpRequestPtr &request, Callback &&callback) {
    printRequestInfo(request);
    HttpResponsePtr response;
    Json::Value jsonBody;    

    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();

    if (requestBody == nullptr) {
        processTheRequestWithTheEmptyBody(jsonBody, response);
        return callback(response);
    }

    std::map<std::string, std::string> data = {{"email", ""}, {"password", ""}};
    bool requestIsNormal = processTheDataFromTheRequest(data, jsonBody, requestBody);

    if (!requestIsNormal) {
        response = HttpResponse::newHttpJsonResponse(jsonBody);
        response->setStatusCode(HttpStatusCode::k400BadRequest);
        return callback(response);
    }

    result user = getUserForAuthentication(data["email"]);
    
    if (user.size() == 0) {
        jsonBody["message"] = "no such user exists";
        response = HttpResponse::newHttpJsonResponse(jsonBody);
        response->setStatusCode(HttpStatusCode::k400BadRequest);
        return callback(response);
    }

    if (getHashedString(data["password"]) != to_string(user[0][1])) {
        jsonBody["message"] = "invalid password";
        response = HttpResponse::newHttpJsonResponse(jsonBody);
        response->setStatusCode(HttpStatusCode::k400BadRequest);
        return callback(response);
    } 
    
    jsonBody["refreshToken"] = processTokenHandling(to_string(user[0]["id"]), "f");
    jsonBody["accessToken"] = processTokenHandling(to_string(user[0]["id"]), "t");

    response = HttpResponse::newHttpJsonResponse(jsonBody);
    callback(response);
}


int main() {
    app()
         .registerHandler("/api/users/", &usersViewGet, {Get})
         .registerHandler("/api/users/create_user/", &createUserView, {Post})
         .registerHandler("/api/users/authentication/", &authenticationView, {Post})
         .loadConfigFile("../config.json")
         .run();
    return 0;
}

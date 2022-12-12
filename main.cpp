#include "db/db.h"
#include "utils/utils.h"
#include "validations/validations.h"
#include <iostream>
#include <map>
#include <pqxx/pqxx>
#include <drogon/drogon.h>

using namespace drogon;

typedef std::function<void(const HttpResponsePtr &)> Callback;
typedef std::vector<std::map<std::string, std::string>> rowsView;
typedef std::map<std::string, std::string> rowView;


void userView(const HttpRequestPtr &request, Callback &&callback, std::string userId) {
    Json::Value jsonBody;
    result userFromDb = getUserById(userId);

    if (userFromDb.size() == 0) {
        jsonBody["message"] = "user id is invalid";
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    rowView userSchema = {{"id", ""}, {"username", ""}, {"email", ""}, {"is_admin", ""}};
    rowView processedUser = createObjFromDb(userSchema, userFromDb[0]);
    createJsonForRow(jsonBody, processedUser);

    callback(processResponse(request, jsonBody, HttpStatusCode::k200OK));
}


void usersView(const HttpRequestPtr &request, Callback &&callback) {
    Json::Value jsonBody;
    rowView userSchema = {{"id", ""}, {"username", ""}, {"email", ""}, {"is_admin", ""}};
    
    result usersFromDb = getAllUsers();
    rowsView processedUsers = createObjsFromDb(userSchema, usersFromDb);
    createJsonBody(jsonBody, processedUsers);

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

    result user = getUserByEmail(data["email"]);
    
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

    std::map<std::string, std::string> user = authorizeUser(userToken.substr(7, 64), "t", jsonBody);
    if (user.size() == 0) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    deleteTokens(user["user_id"]);
    callback(processResponse(request, HttpStatusCode::k200OK));
}


int main() {
    app()
         .registerHandler("/api/users/{user_id}/", &userView, {Get}) // for owners and admins
         .registerHandler("/api/users/", &usersView, {Get}) // for admins
         .registerHandler("/api/users/create_user/", &createUserView, {Post}) // for all
         .registerHandler("/api/users/authentication/", &authenticationView, {Post}) // for all
         .registerHandler("/api/users/logout/", &logoutView, {Get}) // for all
         .loadConfigFile("../config.json")
         .run();
    return 0;
}

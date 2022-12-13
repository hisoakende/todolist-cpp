#include "users.h"
#include <iostream>

void userView(const HttpRequestPtr &request, Callback &&callback, std::string userId) {
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);

    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    result userFromDb;
    if (!checkQueryParam(userId, jsonBody, userFromDb, getUserById)) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    rowView userSchema = {{"id", ""}, {"username", ""}, {"email", ""}, {"is_admin", ""}};
    rowView processedUser = createObjFromDb(userSchema, userFromDb[0]);
    if (auth.first["is_admin"] == "f" && auth.first["user_id"] != processedUser["id"]) {
        return callback(processResponse(request, HttpStatusCode::k403Forbidden));
    }

    createJsonForRow(jsonBody, processedUser);

    callback(processResponse(request, jsonBody, HttpStatusCode::k200OK));
}


void updateUserView(const HttpRequestPtr &request, Callback &&callback, std::string userId) {
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);

    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    result userFromDb;
    if (!checkQueryParam(userId, jsonBody, userFromDb, getUserById)) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    rowView userSchema = {{"username", ""}, {"email", ""}, {"password", ""}, {"is_admin", ""}};
    rowView processedUser = createObjFromDb(userSchema, userFromDb[0]);
    std::string userIsAdmin = processedUser["is_admin"];
    std::string userPassword = processedUser["password"];
    processedUser.erase("is_admin");

    if (auth.first["is_admin"] == "f" && auth.first["user_id"] != processedUser["id"]) {
        return callback(processResponse(request, HttpStatusCode::k403Forbidden));
    }

    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();
    if (requestBody == nullptr || requestBody->size() == 0) {
        return callback(processTheResponseIfRequestBodyIsEmpty());
    }

    rowView userData = {{"username", ""}, {"email", ""}, {"password", ""}};
    processTheDataFromTheRequest(userData, requestBody);
    updateDataInRowView(processedUser, userData);

    if (userPassword == processedUser["password"]) {
        processedUser.erase("password");
    }

    if (!isValidUserCreateOrUpdateData(processedUser, jsonBody)) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    if (processedUser.find("password") != processedUser.end()) {
        userPassword = getHashedString(processedUser["password"]);
    } 

    std::string uniqueViolationText;
    bool userIsUpdated = tryToUpdateUser(userId, processedUser["username"], 
                                         processedUser["email"], userPassword, userIsAdmin, uniqueViolationText);
    if (!userIsUpdated) {
        processUniqueViolationTextForUserCreation(uniqueViolationText, jsonBody);
        callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }
    callback(processResponse(request, HttpStatusCode::k204NoContent));
}


void usersView(const HttpRequestPtr &request, Callback &&callback) {
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);

    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    if (auth.first["is_admin"] == "f") {
        return callback(processResponse(request, HttpStatusCode::k403Forbidden));
    }

    rowView userSchema = {{"id", ""}, {"username", ""}, {"email", ""}, {"is_admin", ""}};
    
    result usersFromDb = getAllUsers();
    rowsView processedUsers = createObjsFromDb(userSchema, usersFromDb);
    createJsonBody(jsonBody, processedUsers);

  	callback(processResponse(request, jsonBody, HttpStatusCode::k200OK));
}


void createUserView(const HttpRequestPtr &request, Callback &&callback) {
    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();

    if (requestBody == nullptr || requestBody->size() == 0) {
        return callback(processTheResponseIfRequestBodyIsEmpty());
    }

    Json::Value jsonBody;
    rowView userData = {{"username", ""}, {"email", ""}, {"password", ""}};
    bool requestIsNormal = processTheDataFromTheRequest(userData, jsonBody, requestBody);

    if (!requestIsNormal || !isValidUserCreateOrUpdateData(userData, jsonBody)) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }
    
    HttpResponsePtr response;
    std::string uniqueViolationText;

    if (!tryToCreateUser(userData["username"], userData["email"], getHashedString(userData["password"]), uniqueViolationText)) {
        processUniqueViolationTextForUserCreation(uniqueViolationText, jsonBody);
        callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }
    
    callback(processResponse(request, HttpStatusCode::k201Created));
}

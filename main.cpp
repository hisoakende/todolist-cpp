#include "db/db.h"
#include "utils/utils.h"
#include "validations/validations.h"

using namespace drogon;

typedef std::function<void(const HttpResponsePtr &)> Callback;
typedef std::pair<std::map<std::string, std::string>, HttpResponsePtr> userAndResponse;


void userView(const HttpRequestPtr &request, Callback &&callback, std::string userId) {
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);

    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    result userFromDb = getUserById(userId);
    if (userFromDb.size() == 0) {
        jsonBody["message"] = "user id is invalid";
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

    result userFromDb = getUserById(userId);
    if (userFromDb.size() == 0) {
        jsonBody["message"] = "user id is invalid";
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
    std::map<std::string, std::string> userData = {{"username", ""}, {"email", ""}, {"password", ""}};
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


void authenticationView(const HttpRequestPtr &request, Callback &&callback) {
    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();

    if (requestBody == nullptr || requestBody->size() == 0) {
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
    userAndResponse auth = processAuthorizations(request, jsonBody);

    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    deleteTokens(auth.first["user_id"]);
    callback(processResponse(request, HttpStatusCode::k200OK));
}


int main() {
    app()
         .registerHandler("/api/users/{user_id}/", &userView, {Get}) // for owners and admins
         .registerHandler("/api/users/{user_id}/", &updateUserView, {Patch}) // for owners and admins
         .registerHandler("/api/users/", &usersView, {Get}) // for admins
         .registerHandler("/api/users/", &createUserView, {Post}) // for all
         .registerHandler("/api/users/authentication/", &authenticationView, {Post}) // for all
         .registerHandler("/api/users/logout/", &logoutView, {Get}) // for all
         .loadConfigFile("../config.json")
         .run();
    return 0;
}

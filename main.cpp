#include "users/users.h"
#include "categories/categories.h"

#include "db/db.h"
#include "utils/utils.h"
#include "validations/validations.h"

using namespace drogon;

typedef std::function<void(const HttpResponsePtr &)> Callback;
typedef std::pair<std::map<std::string, std::string>, HttpResponsePtr> userAndResponse;


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
         .registerHandler("/api/categories/{category_id}/", &categoryView, {Get}) // for auth
         .registerHandler("/api/categories/{category_id}/", &deleteCategoryView, {Delete}) // for admins
         .registerHandler("/api/categories/{category_id}/", &updateCategoryView, {Put})  // for admins
         .registerHandler("/api/categories/", &categoriesView, {Get}) // for auth
         .registerHandler("/api/categories/", &createCategoryView, {Post}) // for auth
         .registerHandler("/api/users/{user_id}/", &userView, {Get}) // for owners and admins
         .registerHandler("/api/users/{user_id}/", &updateUserView, {Patch}) // for owners and admins
         .registerHandler("/api/users/", &usersView, {Get}) // for admins
         .registerHandler("/api/users/", &createUserView, {Post}) // for all
         .registerHandler("/api/authentication/", &authenticationView, {Post}) // for all
         .registerHandler("/api/logout/", &logoutView, {Get}) // for auth
         .loadConfigFile("../config.json")
         .run();
    return 0;
}

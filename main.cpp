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

    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();
    Json::Value jsonBody;

    if (requestBody == nullptr) {
        processTheRequestWithTheEmptyBody(jsonBody, response);
        callback(response);
        return;
    }

    std::map<std::string, std::string> userData = {{"username", ""}, {"email", ""}, {"password", ""}};
    bool requestIsNormal = processTheDataFromTheUserCreationRequest(userData, jsonBody, requestBody);

    if (!requestIsNormal || !isValidUserCreateData(userData, jsonBody)) {
        response = HttpResponse::newHttpJsonResponse(jsonBody);
        response->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(response);
        return;
    }
    
    std::string uniqueViolationText;
    if (!tryToCreateUser(userData["username"], userData["email"], getHashedPassword(userData["password"]), uniqueViolationText)) {
        processUniqueViolationTextForUserCreation(uniqueViolationText, jsonBody);
        response = HttpResponse::newHttpJsonResponse(jsonBody);
        response->setStatusCode(HttpStatusCode::k400BadRequest);
    } else {
        response = HttpResponse::newHttpResponse();
        response->setStatusCode(HttpStatusCode::k201Created);
    }
    
    callback(response);
}


int main() {
    app()
         .registerHandler("/api/users/", &usersViewGet, {Get})
         .registerHandler("/api/users/create_user/", &createUserView, {Post})
         .loadConfigFile("../config.json")
         .run();
    return 0;
}

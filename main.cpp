#include "db/db.h"
#include "utils/utils.h"
#include <pqxx/pqxx>
#include <drogon/drogon.h>

using namespace drogon;

typedef std::function<void(const HttpResponsePtr &)> Callback;


void usersViewGet(const HttpRequestPtr &request, Callback &&callback) {
    printRequestInfo(request);
    pqxx::result users = getAllUsers();
    Json::Value jsonBody;
    createJsonBody(jsonBody, users);
    auto response = HttpResponse::newHttpJsonResponse(jsonBody);
  	callback(response);
}


int main() {
    app()
         .registerHandler("/api/users/", &usersViewGet, {Get})
         .loadConfigFile("../config.json")
         .run();
    return 0;
}

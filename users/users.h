#pragma once
#include "../db/db.h"
#include "../utils/utils.h"
#include "../validations/validations.h"
#include <map>
#include <drogon/drogon.h>
#include <pqxx/pqxx>

using namespace drogon;

typedef std::function<void(const HttpResponsePtr &)> Callback;
typedef std::pair<std::map<std::string, std::string>, HttpResponsePtr> userAndResponse;


void userView(const HttpRequestPtr &request, Callback &&callback, std::string userId);


void updateUserView(const HttpRequestPtr &request, Callback &&callback, std::string userId);


void usersView(const HttpRequestPtr &request, Callback &&callback);


void createUserView(const HttpRequestPtr &request, Callback &&callback);

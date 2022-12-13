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
typedef std::vector<std::map<std::string, std::string>> rowsView;
typedef std::map<std::string, std::string> rowView;


void categoryView(const HttpRequestPtr &request, Callback &&callback, std::string categoryId);

void categoriesView(const HttpRequestPtr &request, Callback &&callback);

void createCategoryView(const HttpRequestPtr &request, Callback &&callback);

void deleteCategoryView(const HttpRequestPtr &request, Callback &&callback, std::string categoryId);

void updateCategoryView(const HttpRequestPtr &request, Callback &&callback, std::string categoryId);

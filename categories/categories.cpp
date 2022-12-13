#include "categories.h"


void categoryView(const HttpRequestPtr &request, Callback &&callback, std::string categoryId) {
    result categoryFromBd;
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);
    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    if (!checkQueryParam(categoryId, jsonBody, categoryFromBd, getCategory)) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    rowView category = {{"id", ""}, {"name", ""}};
    rowView processedCategory = createObjFromDb(category, categoryFromBd[0]);
    createJsonForRow(jsonBody, processedCategory);

    callback(processResponse(request, jsonBody, HttpStatusCode::k200OK));
}


void categoriesView(const HttpRequestPtr &request, Callback &&callback) {
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);
    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    rowView category = {{"id", ""}, {"name", ""}};

    result categoriesFromBd = getAllCategories();
    rowsView processedCategories = createObjsFromDb(category, categoriesFromBd);
    createJsonBody(jsonBody, processedCategories);

    callback(processResponse(request, jsonBody, HttpStatusCode::k200OK));
}


void createCategoryView(const HttpRequestPtr &request, Callback &&callback) {
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);
    if (auth.first.size() == 0) {
        return callback(auth.second);
    }
    
    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();
    if (requestBody == nullptr || requestBody->size() == 0) {
        return callback(processTheResponseIfRequestBodyIsEmpty());
    }

    rowView categoryData = {{"name", ""}};
    bool requestIsNormal = processTheDataFromTheRequest(categoryData, jsonBody, requestBody);
    if (!requestIsNormal || categoryData["name"] == "") {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    createCategory(categoryData["name"]);

    callback(processResponse(request, HttpStatusCode::k201Created));
}


void deleteCategoryView(const HttpRequestPtr &request, Callback &&callback, std::string categoryId) {
    result categoryFromBd;
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);
    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    if (!checkQueryParam(categoryId, jsonBody, categoryFromBd, getCategory)) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    if (auth.first["is_admin"] == "f") {
        return callback(processResponse(request, HttpStatusCode::k403Forbidden));
    }

    rowView category = {{"id", ""}};
    rowView processedCategory = createObjFromDb(category, categoryFromBd[0]);
    deleteCategory(processedCategory["id"]);

    callback(processResponse(request, HttpStatusCode::k204NoContent));
}


void updateCategoryView(const HttpRequestPtr &request, Callback &&callback, std::string categoryId) {
    result categoryFromBd;
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);
    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    if (!checkQueryParam(categoryId, jsonBody, categoryFromBd, getCategory)) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    if (auth.first["is_admin"] == "f") {
        return callback(processResponse(request, HttpStatusCode::k403Forbidden));
    }
    
    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();
    if (requestBody == nullptr || requestBody->size() == 0) {
        return callback(processTheResponseIfRequestBodyIsEmpty());
    }

    rowView categoryData = {{"name", ""}};
    bool requestIsNormal = processTheDataFromTheRequest(categoryData, jsonBody, requestBody);
    if (!requestIsNormal || categoryData["name"] == "") {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    updateCategory(categoryId, categoryData["name"]);

    callback(processResponse(request, HttpStatusCode::k204NoContent));
}

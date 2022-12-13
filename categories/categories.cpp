#include "categories.h"


void categoryView(const HttpRequestPtr &request, Callback &&callback, std::string categoryId) {
    result categoryFromBd;
    Json::Value jsonBody;

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
    rowView category = {{"id", ""}, {"name", ""}};

    result categoriesFromBd = getAllCategories();
    rowsView processedCategories = createObjsFromDb(category, categoriesFromBd);
    createJsonBody(jsonBody, processedCategories);

    callback(processResponse(request, jsonBody, HttpStatusCode::k200OK));
}

#include "notes.h"


void noteView(const HttpRequestPtr &request, Callback &&callback, std::string noteId) {
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);

    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    result noteFromDb;
    if (!checkQueryParam(noteId, jsonBody, noteFromDb, getNote)) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    rowView noteSchema = {{"id", ""}, {"title", ""}, {"text", ""}, {"author_id", ""}, {"category_id", ""}};
    rowView processedNote = createObjFromDb(noteSchema, noteFromDb[0]);

    if (auth.first["is_admin"] == "f" && auth.first["user_id"] != processedNote["author_id"]) {
        return callback(processResponse(request, HttpStatusCode::k403Forbidden));
    }

    createJsonForRow(jsonBody, processedNote);
    replaceRelatedNoteFieldsInObj(jsonBody);

    callback(processResponse(request, jsonBody, HttpStatusCode::k200OK));
}


void notesView(const HttpRequestPtr &request, Callback &&callback) {
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);

    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    result notesFromBd;
    if (auth.first["is_admin"] == "t") {
        notesFromBd = getAllNotes();
    } else {
        notesFromBd = getNotesByAuthor(auth.first["user_id"]);
    }

    rowView noteSchema = {{"id", ""}, {"title", ""}, {"text", ""}, {"author_id", ""}, {"category_id", ""}};
    rowsView processedNotes = createObjsFromDb(noteSchema, notesFromBd);
    createJsonBody(jsonBody, processedNotes);
    replaceRelatedNoteFieldsInObjs(jsonBody);

    callback(processResponse(request, jsonBody, HttpStatusCode::k200OK));
}


void deleteNoteView(const HttpRequestPtr &request, Callback &&callback, std::string noteId) {
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);

    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    result noteFromDb;
    if (!checkQueryParam(noteId, jsonBody, noteFromDb, getNote)) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    rowView noteSchema = {{"id", ""}, {"author_id", ""}};
    rowView processedNote = createObjFromDb(noteSchema, noteFromDb[0]);

    if (auth.first["is_admin"] == "f" && auth.first["user_id"] != processedNote["author_id"]) {
        return callback(processResponse(request, HttpStatusCode::k403Forbidden));
    }

    deleteNote(processedNote["id"]);

    callback(processResponse(request, HttpStatusCode::k204NoContent));
}


void createNoteView(const HttpRequestPtr &request, Callback &&callback) {
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);

    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();
    if (requestBody == nullptr || requestBody->size() == 0) {
        return callback(processTheResponseIfRequestBodyIsEmpty());
    }

    rowView noteData = {{"title", ""}, {"text", ""}, {"category_id", ""}};
    bool requestIsNormal = processTheDataFromTheRequest(noteData, jsonBody, requestBody);
    if (!requestIsNormal || !checkNoteDataFromRequest(noteData, jsonBody)) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    createNote(noteData["title"], noteData["text"], auth.first["user_id"], noteData["category_id"]);

    callback(processResponse(request, HttpStatusCode::k201Created));
}


void updateNoteView(const HttpRequestPtr &request, Callback &&callback, std::string noteId) {
    Json::Value jsonBody;
    userAndResponse auth = processAuthorizations(request, jsonBody);

    if (auth.first.size() == 0) {
        return callback(auth.second);
    }

    result noteFromDb;
    if (!checkQueryParam(noteId, jsonBody, noteFromDb, getNote)) {
        return callback(processResponse(request, jsonBody, HttpStatusCode::k400BadRequest));
    }

    rowView noteSchema = {{"title", ""}, {"text", ""}, {"category_id", ""}, {"author_id", ""}};
    rowView processedNote = createObjFromDb(noteSchema, noteFromDb[0]);

    if (auth.first["is_admin"] == "f" && auth.first["user_id"] != processedNote["author_id"]) {
        return callback(processResponse(request, HttpStatusCode::k403Forbidden));
    }

    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();
    if (requestBody == nullptr || requestBody->size() == 0) {
        return callback(processTheResponseIfRequestBodyIsEmpty());
    }

    rowView noteData = {{"title", ""}, {"text", ""}, {"category_id", ""}};
    processTheDataFromTheRequest(noteData, requestBody);
    updateDataInRowView(processedNote, noteData);

    updateNote(noteId, processedNote["title"], processedNote["text"], processedNote["category_id"]);

    callback(processResponse(request, HttpStatusCode::k204NoContent));
}

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

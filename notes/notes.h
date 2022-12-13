#pragma once
#include "../db/db.h"
#include "../utils/utils.h"
#include "../validations/validations.h"
#include <map>
#include <drogon/drogon.h>
#include <pqxx/pqxx>

using namespace drogon;

void noteView(const HttpRequestPtr &request, Callback &&callback, std::string noteId);

void notesView(const HttpRequestPtr &request, Callback &&callback);

void deleteNoteView(const HttpRequestPtr &request, Callback &&callback, std::string noteId);

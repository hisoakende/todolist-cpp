#include "db.h"

std::string connectionParams = "host=localhost port=5432 dbname=todolist";
connection conn(connectionParams.c_str());
nontransaction worker(conn);


result getAllUsers() {
    return worker.exec("SELECT id, username, email, is_admin FROM users");
}


result getUserById(std::string userId) {
    return worker.exec("SELECT id, username, email, password, is_admin FROM users WHERE id=\'" + userId + "\'");
}


result getUserByEmail(std::string email) {
    return worker.exec("SELECT id, password FROM users WHERE email=\'" + email + "\'");
}


bool tryToChangeUser(std::string query, std::string &exceptionText) {
    try {
        worker.exec(query);
        return true;
    }
    catch (pqxx::unique_violation &e) {
        exceptionText = e.what();
        return false;
    }
}


bool tryToCreateUser(std::string username, std::string email, std::string password, std::string &exceptionText) {
    return tryToChangeUser("INSERT INTO users (username, email, password, is_admin) VALUES (\'" 
                            + username + "\', \'" + email + "\', \'" + password + "\', \'f\')", 
                            exceptionText);
}


bool tryToUpdateUser(std::string id, std::string username, std::string email, 
                     std::string password, std::string isAdmin, std::string &exceptionText) {
    return tryToChangeUser("UPDATE users SET username=\'" + username +"\', email=\'" + email + 
                           "\', password=\'" + password + "\', is_admin=\'" + isAdmin + 
                           "\' WHERE id=\'" + id + "\'",
                           exceptionText);
}


result getTokenByUserId(std::string userId, std::string isAccess) {
    return worker.exec("SELECT * FROM tokens WHERE user_id=\'" + userId + "\' AND is_access=\'" + isAccess +"\'");
}


result getTokenAndUserByValue(std::string value, std::string isAccess) {
    return worker.exec("SELECT t.id, t.value, t.is_access, t.create_time, u.id AS user_id, u.username, "
                       "u.email, u.is_admin FROM tokens AS t JOIN users AS u ON t.user_id = u.id "
                       "WHERE t.value=\'" + value +  "\' AND t.is_access=\'" + isAccess +"\'");
}


void saveToken(std::string value, std::string userId, std::string isAccess, std::string createTime) {
    worker.exec("INSERT INTO tokens (value, user_id, is_access, create_time) VALUES (\'"
                + value + "\', \'" + userId + "\', \'" + isAccess + "\', \'" + createTime + "\')");
}


void updateToken(std::string id, std::string value, std::string createTime) {
    worker.exec("UPDATE tokens SET value=\'" + value + "\', create_time=\'" + createTime +"\' WHERE id=\'" + id +"\'");
}


void deleteTokens(std::string userId) {
    worker.exec("DELETE FROM tokens WHERE user_id=\'" + userId + "\'");
}


result getAllCategories() {
    return worker.exec("SELECT * FROM categories");
}


result getCategory(std::string categoryId) {
    return worker.exec("SELECT * FROM categories WHERE id=\'" + categoryId +"\'");
}


void createCategory(std::string name) {
    worker.exec("INSERT INTO categories (name) VALUES (\'" + name + "\')");
}


void deleteCategory(std::string id) {
    worker.exec("DELETE FROM categories WHERE id=\'" + id + "\'");
}


void updateCategory(std::string id, std::string name) {
    worker.exec("UPDATE categories SET name=\'" + name + "\' WHERE id=\'" + id +"\'");
}


result getNote(std::string noteId) {
    return worker.exec("SELECT * FROM notes WHERE id=\'" + noteId +"\'");
}


result getAllNotes() {
    return worker.exec("SELECT * FROM notes");
}


result getNotesByAuthor(std::string authorId) {
    return worker.exec("SELECT * FROM notes WHERE author_id=" + authorId);
}


void deleteNote(std::string noteId) {
    worker.exec("DELETE FROM notes WHERE id=\'" + noteId + "\'");
}


void createNote(std::string title, std::string text, std::string author_id, std::string category_id) {
    worker.exec("INSERT INTO notes (title, text, author_id, category_id) VALUES (\'" 
                            + title + "\', \'" + text + "\', " + author_id + ", " + category_id + ")");
}


void updateNote(std::string noteId, std::string title, std::string text, std::string category_id) {
    worker.exec("UPDATE notes SET title=\'" + title +"\', text=\'" + text + 
                "\', category_id=" + category_id + " WHERE id=\'" + noteId + "\'");
}
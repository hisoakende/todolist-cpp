#include "db.h"

std::string connectionParams = "host=localhost port=5432 dbname=todolist";
connection conn(connectionParams.c_str());
nontransaction worker(conn);


result getAllUsers() {
    return worker.exec("SELECT * FROM users");
}


bool tryToCreateUser(std::string username, std::string email, std::string password, std::string &exceptionText) {
    try {
        worker.exec("INSERT INTO users (username, email, password, is_admin) VALUES (\'" 
                    + username + "\', \'" + email + "\', \'" + password + "\', \'f\')");
        return true;
    }
    catch (pqxx::unique_violation &e) {
        exceptionText = e.what();
        return false;
    }
}


result getUserForAuthentication(std::string email) {
    return worker.exec("SELECT id, password FROM users WHERE email=\'" + email + "\'");
}


result getTokenByUserId(std::string userId, std::string isAccess) {
    return worker.exec("SELECT * FROM tokens WHERE user_id=\'" + userId + "\' AND is_access=\'" + isAccess +"\'");
}


result getTokenByValue(std::string value, std::string isAccess) {
    return worker.exec("SELECT * FROM tokens WHERE value=\'" + value +  "\' AND is_access=\'" + isAccess +"\'");
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

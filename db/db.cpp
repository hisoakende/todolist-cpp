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

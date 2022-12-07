#include "db.h"
#include <pqxx/pqxx>

using namespace pqxx;

std::string connectionParams = "host=localhost port=5432 dbname=todolist";
connection conn(connectionParams.c_str());
work worker(conn);


result getAllUsers() {
    return worker.exec("SELECT * FROM users");
}

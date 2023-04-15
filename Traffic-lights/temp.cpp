#include <chrono>
#include <iostream>
using namespace std;

int main()
{

    return 0;
}

// string sql_query2("INSERT INTO Traffic_lights VALUES(1, 30.90660430000, 75.85988220000, 1,0,0, 20);"
//                   "INSERT INTO Traffic_lights VALUES(2, 30.90649690000, 75.85988470000, 0,1,0, 35);"
//                   "INSERT INTO Traffic_lights VALUES(3, 30.90637920000, 75.85953980000, 1,0,0, 15);");

// rc = sqlite3_exec(db, sql_query2.c_str(), NULL, 0, &messaggeError);
// if (rc != SQLITE_OK)
// {
//     std::cerr << "Error Insert" << std::endl;
//     sqlite3_free(messaggeError);
// }

// else
//     std::cout << "Records created Successfully!" << std::endl;

// sqlite3_stmt *stmt;
// const char *sql = "SELECT * FROM my_table";
// rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
// while (sqlite3_step(stmt) == SQLITE_ROW)
// {
//     // Process each row of the result set
// }
// sqlite3_finalize(stmt);

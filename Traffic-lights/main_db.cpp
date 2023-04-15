#include <iostream>
#include <sqlite3.h>
using namespace std;

void insert(sqlite3 *db, int id, float lat, float longi, int r, int g, int y, int time)
{
    char *messaggeError;
    int rc = 0;
    string sql_query2 = "INSERT INTO Traffic_lights VALUES(" + to_string(id) + ", " + to_string(lat) + ", " + to_string(longi) + ", " + to_string(r) + "," + to_string(g) + "," + to_string(y) + "," + to_string(time) + ");";
    rc = sqlite3_exec(db, sql_query2.c_str(), NULL, 0, &messaggeError);
    if (rc != SQLITE_OK)
    {
        cerr << "Error Insert" << std::endl;
        sqlite3_free(messaggeError);
    }
    else
        cout << "Records created Successfully!" << std::endl;
}

int main()
{
    sqlite3 *db;
    int rc = sqlite3_open("main.db", &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << endl
                  << sqlite3_errmsg(db);
        return 0;
    }
    else
    {
        cout << "Opened database successfully" << endl;
    }

    string sql_query1 = "CREATE TABLE Traffic_lights("
                        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "LAT FLOAT NOT NULL, "
                        "LONG FLOAT NOT NULL, "
                        "R INT NOT NULL, "
                        "G INT NOT NULL, "
                        "Y INT NOT NULL, "
                        "TIME INT NOT NULL);";

    char *messaggeError;
    rc = sqlite3_exec(db, sql_query1.c_str(), NULL, 0, &messaggeError);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Error Create Table" << std::endl;
        sqlite3_free(messaggeError);
    }
    else
        std::cout << "Table created Successfully" << std::endl;

    insert(db, 1, 30.9066043, 75.9006233, 1, 0, 0, 20);
    insert(db, 2, 30.9180175, 75.6406355, 0, 1, 0, 15);
    insert(db, 3, 30.9320097, 75.9555338, 0, 1, 0, 2);
    insert(db, 4, 30.8588957, 75.8738860, 1, 0, 0, 30);
    insert(db, 5, 30.8752053, 75.8763215, 0, 0, 1, 2);
    insert(db, 6, 31.0267558, 75.7510221, 1, 0, 0, 40);

    sqlite3_close(db);

    return 0;
}
#include <iostream>
#include <sqlite3.h>
#include <thread>
#include <windows.h>
using namespace std;

bool is_database_open(sqlite3 *db)
{
    const char *main = sqlite3_db_filename(db, "main");
    return (main != nullptr);
}

sqlite3 *open_db()
{
    sqlite3 *db;
    int rc = sqlite3_open("main.db", &db);

    if (rc)
    {
        std::cerr << "Can't open database: " << endl
                  << sqlite3_errmsg(db);
        return;
    }
    else
    {
        cout << "Opened database successfully" << endl;
    }
    return db;
}

void update(char *argv[])
{
    sqlite3 *db = open_db();
    int time, rl, gl, yl, rc;
    char *zErrMsg = 0;
    time = rl = gl = yl = 0;
    rl = *argv[1];
    gl = *argv[2];
    yl = *argv[3];
    time = *argv[4];

    if (rl)
    {

        if (time >= 45)
        {

            rl = 0;
            yl = 1;
            time = 0;
        }
    }

    if (yl)
    {

        if (time >= 5)
        {

            yl = 0;
            gl = 1;
            time = 0;
        }
    }
    if (gl)
    {

        if (time >= 30)
        {

            gl = 0;
            rl = 1;
            time = 0;
        }
    }

    time++;
    string sql_query4 = "UPDATE Traffic_lights SET R = " + to_string(rl) + ", G = " + to_string(gl) + ", Y = " + to_string(yl) + ", TIME = " + to_string(time) + "WHERE ID = " + to_string(*argv[0]) + " ;";
    rc = sqlite3_exec(db, sql_query4.c_str(), 0, 0, &zErrMsg);
}

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

static int callback(void *data, int argc, char **argv, char **azColName)
{

    int i;
    fprintf(stderr, "%s", (const char *)data);

    for (i = 0; i < argc; i++)
    {
        update(argv);
        // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
}

int main()
{
    system("cls");
    int id, r, g, y, time;
    float lat, longi;
    string data("CALLBACK FUNCTION: \n");
    sqlite3 *db;
    db = open_db();
    int rc = is_database_open(db);

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

    insert(db, 1, 30.90660430000, 75.85988220000, 1, 0, 0, 25);
    insert(db, 2, 30.90660430000, 75.85988220000, 1, 0, 0, 25);
    insert(db, 3, 30.90660430000, 75.85988220000, 1, 0, 0, 25);
    insert(db, 4, 30.90660430000, 75.85988220000, 1, 0, 0, 25);
    insert(db, 5, 30.90660430000, 75.85988220000, 1, 0, 0, 25);
    insert(db, 6, 30.90660430000, 75.85988220000, 1, 0, 0, 25);
    while (true)
    {
        Sleep(1000);
        for (id = 0; id < 7; id++)
        {
            string sql_query3 = "SELECT ID, R,G,Y,TIME FROM Traffic_lights WHERE ID = " + to_string(id);
            rc = sqlite3_exec(db, sql_query3.c_str(), callback, (void *)data.c_str(), NULL);
            if (rc != SQLITE_OK)
            {
                std::cerr << "Error Print" << std::endl;
                sqlite3_free(messaggeError);
            }
            else
                std::cout << "Record Printed Successfully!" << std::endl;
        }
    }

    sqlite3_close(db);

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

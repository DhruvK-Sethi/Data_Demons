#include <iostream>
#include <sqlite3.h>
#include <thread>
#include <windows.h>
using namespace std;

class simulation
{
    int id;
    char *messaggeError;
    string data;
    sqlite3 *db;

public:
    simulation()
    {
        int rc = sqlite3_open("main.db", &db);
        if (rc)
        {
            std::cerr << "Can't open database: " << endl
                      << sqlite3_errmsg(db);
        }
        else
        {
            cout << "Opened database successfully" << endl;
        }
    }
    void update(int ID, int R, int G, int Y, int TIME)
    {

        int time, rl, gl, yl, rc, id;
        char *zErrMsg = 0;
        time = rl = gl = yl = 0;
        id = ID;
        rl = R;
        gl = G;
        yl = Y;
        time = TIME;

        if (rl)
        {

            if (time >= 45)
            {

                rl = 0;
                yl = 1;
                time = 0;
                cout << "Switched Red for id: " << id << endl;
            }
        }

        if (yl)
        {

            if (time >= 5)
            {

                yl = 0;
                gl = 1;
                time = 0;
                cout << "Switched Yellow for id: " << id << endl;
            }
        }
        if (gl)
        {

            if (time >= 30)
            {

                gl = 0;
                rl = 1;
                time = 0;
                cout << "Switched Green for id: " << id << endl;
            }
        }

        time++;
        cout << "Time of " + to_string(id) + " : " << time << endl;
        string sql_query4 = "UPDATE Traffic_lights SET R = " + to_string(rl) + ", G = " + to_string(gl) + ", Y = " + to_string(yl) + ", TIME = " + to_string(time) + " WHERE ID = " + to_string(id) + ";";
        rc = sqlite3_exec(db, sql_query4.c_str(), 0, 0, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            cout << "Error updating traffic lights: " << zErrMsg << endl;
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }

    void driver()
    {

        sqlite3_stmt *stmt;
        while (true)
        {
            system("cls");

            for (int i = 0; i < 7; i++)
            {

                string sql_query1 = "SELECT ID, R, G, Y, TIME FROM Traffic_lights WHERE id = " + to_string(i) + ";";
                int rc = sqlite3_prepare_v2(db, sql_query1.c_str(), -1, &stmt, nullptr);
                if (rc != SQLITE_OK)
                {
                    std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
                    sqlite3_close(db);
                    return;
                }

                while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
                {

                    int ID = sqlite3_column_int(stmt, 0);
                    int R = sqlite3_column_int(stmt, 1);
                    int G = sqlite3_column_int(stmt, 2);
                    int Y = sqlite3_column_int(stmt, 3);
                    int TIME = sqlite3_column_int(stmt, 4);

                    update(ID, R, G, Y, TIME);
                }

                if (rc != SQLITE_DONE)
                {
                    cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
                }
            }
            Sleep(1000);
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
};

int main()
{
    simulation s;
    s.driver();
    return 0;
}

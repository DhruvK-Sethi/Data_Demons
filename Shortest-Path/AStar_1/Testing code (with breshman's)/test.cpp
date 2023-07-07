#include <bits/stdc++.h>
#include <sqlite3.h>
using namespace std;

const int row = 13734;
const int col = 18622;

class test
{
    int count = 0;
    int no = 0;
    int sum = 0;
    char *messaggeError;
    string data;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    sqlite3_stmt *stml;
    int **arr;
    vector<vector<int>> myArray;
    int *numCoords;
    vector<int> coords;

public:
    test()
    {

        int rc = sqlite3_open("New_network.db", &db);
        if (rc)
        {
            std::cerr << "Can't open database: " << endl
                      << sqlite3_errmsg(db);
        }
        else
        {
            cout << "Opened database successfully" << endl;
        }
        arr = new int *[18622];
        for (int i = 0; i <= 18622; i++)
        {
            arr[i] = new int[13734]{0};
        }
        myArray.resize(18622);
        for (int i = 0; i < 18622; ++i)
        {
            myArray[i].resize(13734);
        }

        cout << "Array created successfuly..." << endl;
    }

    void check()
    {

        cout << "Checking..." << endl;
        string sql_query1 = "SELECT double1, double2 FROM myTable ;";
        int rc = sqlite3_prepare_v2(db, sql_query1.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            return;
        }

        bool result = false;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            double double1 = sqlite3_column_double(stmt, 0);
            double double2 = sqlite3_column_double(stmt, 1);
            array(double2, double1);
        }
        cout << "Process finished...: " << sum << endl;
        cout << myArray[3591][2603] << endl
             << myArray[3633][2568] << endl;

        cout << "Calling A-star..." << endl;
        // aStarSearch(myArray, src, dest);
        cout << "Calling Failed..." << endl;
        if (rc != SQLITE_DONE)
        {
            cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
        return;
    }

    void afterburn()
    {
        cout << endl
             << "Hello";
        bool result = false;
        // int mapkey = 1;
        for (int mapkey = 1; mapkey <= 11106; mapkey++)
        {

            string sql_query5 = "SELECT MAX(vectorIndex)FROM myTable WHERE mapKey = " + to_string(mapkey) + ";";
            int exit = sqlite3_prepare_v2(db, sql_query5.c_str(), -1, &stmt, nullptr);
            if (exit != SQLITE_OK)
            {
                std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_close(db);
                return;
            }

            while ((exit = sqlite3_step(stmt)) == SQLITE_ROW)
            {
                int max = sqlite3_column_int(stmt, 0);
                // cout << "max: " << max << endl;
                for (int i = 2; i <= max; i++)
                {
                    string sql_query6 = "SELECT t1.double2,t1.double1,t2.double2,t2.double1 "
                                        "FROM myTable t1 "
                                        "JOIN myTable t2 "
                                        "ON t1.mapKey = t2.mapKey AND t1.vectorIndex = " +
                                        to_string(i - 1) + " AND t2.vectorIndex = " + to_string(i) + " WHERE t1.mapKey = " + to_string(mapkey) + ";";

                    int get = sqlite3_prepare_v2(db, sql_query6.c_str(), -1, &stml, nullptr);
                    if (get != SQLITE_OK)
                    {
                        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
                        sqlite3_close(db);
                        return;
                    }

                    while ((get = sqlite3_step(stml)) == SQLITE_ROW)
                    {
                        double t1_double1 = sqlite3_column_double(stml, 0);
                        double t1_double2 = sqlite3_column_double(stml, 1);
                        double t2_double1 = sqlite3_column_double(stml, 2);
                        double t2_double2 = sqlite3_column_double(stml, 3);
                        int t1_x = convert_1(t1_double1);
                        int t1_y = convert_2(t1_double2);
                        int t2_x = convert_1(t2_double1);
                        int t2_y = convert_2(t2_double2);
                        // cout << t1_x << endl
                        //      << t1_y << endl
                        //      << t2_x << endl
                        //      << t2_y << endl;
                        drawLine(t1_x, t1_y, t2_x, t2_y);
                        // cout << fixed << setprecision(10) << t1_double1 << endl
                        //      << t1_double2 << endl
                        //      << t2_double1 << endl
                        //      << t2_double2 << endl;
                    }
                }
            }
            cout << "Mapkey: " << mapkey << endl;
            cout << no << endl;

            // no = 0;
        }
        // cout << "Mapkey: " << stepCount << endl;

        cout << "Completed...." << endl;
    }

    void drawLine(int x0, int y0, int x1, int y1)
    {
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int x = x0;
        int y = y0;
        int err = dx - dy;

        while (true)
        {
            no++;

            if (x == x1 && y == y1)
            {
                break;
            }

            int e2 = 2 * err;

            if (e2 > -dy)
            {
                err -= dy;
                x += sx;
            }

            if (e2 < dx)
            {
                err += dx;
                y += sy;
            }
        }
    }

    double convertb_1(int lat)
    {
        double lati = static_cast<double>(lat) / 100000.0;
        lati = (lati + 30.65364);
        // cout << fixed << setprecision(5) << "Double2: " << lati << endl;
        return lati;
    }
    double convertb_2(int long_1)
    {
        double longi = static_cast<double>(long_1) / 100000.0;
        longi = (longi + 76.66046);
        // cout << fixed << setprecision(5) << "Double1: " << longi << endl;
        return longi;
    }
    int convert_1(double double1)
    {
        double lati;
        double1 = round(double1 * 100000) / 100000;
        // cout << fixed << setprecision(5) << "Double1: " << double1 << endl;

        lati = (double1 - 30.65364) * 100000;
        int lat_1 = lati;
        return lat_1;
    }
    int convert_2(double double2)
    {
        double longi;
        double2 = round(double2 * 100000) / 100000;
        // cout << fixed << setprecision(5) << "Double2: " << double2 << endl;
        longi = (double2 - 76.66046) * 100000;
        int long_1 = longi;
        return long_1;
    }

    void array(double double1, double double2)
    {
        int lat_1 = convert_1(double1);
        int long_1 = convert_2(double2);
        myArray[lat_1][long_1] = 1;
        // cout << "Lat_1" << endl
        //      << lat_1 << endl
        //      << "Long_1" << endl
        //      << long_1 << endl;
        // cout << sum << endl;
        sum++;
    }

    ~test()
    {
        for (int i = 0; i < 13734; i++)
        {
            delete[] arr[i];
        }
        delete[] arr;
    }
};

int main()
{
    test t;
    t.check();
    t.afterburn();

    return 0;
}
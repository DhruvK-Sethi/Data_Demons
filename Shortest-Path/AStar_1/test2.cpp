#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <sqlite3.h>
#include <math.h>
#include <chrono>
using namespace std;

const int row = 13734;
const int col = 18622;

struct Node
{
    int x, y;
    int cost;
    Node(int x, int y, int cost) : x(x), y(y), cost(cost) {}
};

bool operator<(const Node &lhs, const Node &rhs)
{
    return lhs.cost > rhs.cost;
}

bool AStar(const vector<vector<int>> &grid, int startX, int startY, int endX, int endY, vector<vector<int>> &path)
{
    if (grid[startX][startY] == 0 || grid[endX][endY] == 0)
    {
        return false;
    }

    int numRows = grid.size();
    int numCols = grid[0].size();

    priority_queue<Node> q;
    vector<vector<int>> cost(numRows, vector<int>(numCols, INT_MAX));
    vector<vector<pair<int, int>>> parent(numRows, vector<pair<int, int>>(numCols, {-1, -1}));

    q.push(Node(startX, startY, 0));
    cost[startX][startY] = 0;

    vector<vector<int>> directions = {
        {-1, 0},  // Up
        {1, 0},   // Down
        {0, -1},  // Left
        {0, 1},   // Right
        {-1, -1}, // Up-left (diagonal)
        {-1, 1},  // Up-right (diagonal)
        {1, -1},  // Down-left (diagonal)
        {1, 1}    // Down-right (diagonal)
    };

    while (!q.empty())
    {
        Node cur = q.top();
        q.pop();

        if (cur.x == endX && cur.y == endY)
        {
            // Reconstruct the path
            while (cur.x != -1 && cur.y != -1)
            {
                path.push_back({cur.x, cur.y});
                int prevX = parent[cur.x][cur.y].first;
                int prevY = parent[cur.x][cur.y].second;
                cur.x = prevX;
                cur.y = prevY;
            }

            return true;
        }

        for (const auto &dir : directions)
        {
            int nextX = cur.x + dir[0];
            int nextY = cur.y + dir[1];

            if (nextX >= 0 && nextX < numRows && nextY >= 0 && nextY < numCols && grid[nextX][nextY] == 1)
            {
                int newCost = cur.cost + 1;
                if (newCost < cost[nextX][nextY])
                {
                    cost[nextX][nextY] = newCost;
                    q.push(Node(nextX, nextY, newCost));
                    parent[nextX][nextY] = {cur.x, cur.y};
                }
            }
        }
    }

    return false;
}

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
        int startX, startY, endX, endY;
        // cin >> startX >> startY >> endX >> endY;
        startX = 10702;
        startY = 1657;
        endX = 10721;
        endY = 1657;
        cout << myArray[3633][2568] << endl
             << myArray[3639][2564] << endl;
        // drawLine(startX, startY, endX, endY, myArray);
        // Call the A* algorithm.
        vector<vector<int>>
            path;
        bool foundPath = AStar(myArray, startX, startY, endX, endY, path);
        // bool foundPath = AStar(myArray, startY, startX, endY, endX, path);

        // Print the output.
        if (foundPath)
        {
            cout << "The cost of the shortest path is " << path.size() - 1 << endl;

            // Print the reversed path.
            cout << "The path is: ";
            for (int i = 0; i < path.size(); i++)
            {
                // cout << "(" << path[i][0] << ", " << path[i][1] << ") ";
                cout << "(" << convertb_1(path[i][0]) << ", " << convertb_2(path[i][1]) << ") ";
            }
            cout << endl;
        }
        else
        {
            cout << "No path found." << endl;
        }

        if (rc != SQLITE_DONE)
        {
            cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
        return;
    }
    void drawLine(int x0, int y0, int x1, int y1, const vector<vector<int>> &grid)
    {
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        while (true)
        {
            if (x0 == x1 && y0 == y1)
            {
                break;
            }

            // Check if the current position (x0, y0) contains a 1
            if (grid[x0][y0] == 1)
            {
                cout << "Found 1 at coordinates: (" << x0 << ", " << y0 << ")" << endl;
            }

            int e2 = 2 * err;

            if (e2 > -dy)
            {
                err -= dy;
                x0 += sx;
            }

            if (e2 < dx)
            {
                err += dx;
                y0 += sy;
            }
        }
    }

    double convertb_1(int lat)
    {
        double lati = static_cast<double>(lat) / 100000.0;
        lati = (lati + 30.65364);
        return lati;
    }
    double convertb_2(int long_1)
    {
        double longi = static_cast<double>(long_1) / 100000.0;
        longi = (longi + 76.66046);
        return longi;
    }
    int convert_1(double double1)
    {
        double lati;
        double1 = round(double1 * 100000) / 100000;
        lati = (double1 - 30.65364) * 100000;
        int lat_1 = lati;
        return lat_1;
    }
    int convert_2(double double2)
    {
        double longi;
        double2 = round(double2 * 100000) / 100000;
        longi = (double2 - 76.66046) * 100000;
        int long_1 = longi;
        return long_1;
    }

    void array(double double1, double double2)
    {
        int lat_1 = convert_1(double1);
        int long_1 = convert_2(double2);
        myArray[lat_1][long_1] = 1;
        sum++;
    }
};

int main()
{
    test t;
    auto start = std::chrono::high_resolution_clock::now();

    t.check();

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;

    return 0;
}
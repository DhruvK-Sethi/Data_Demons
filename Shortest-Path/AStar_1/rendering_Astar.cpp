#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <sqlite3.h>
#include <math.h>
#include <chrono>
#include <random>
using namespace std;

const int row = 13734;
const int col = 18622;
const int GRID_SIZE = 1000;

bool compareCoordinates(vector<int> &a, vector<int> &b)
{
    if (a[0] != b[0])
    {
        return a[0] < b[0];
    }
    return a[1] < b[1];
}

void sortPath(vector<vector<int>> &path)
{
    sort(path.begin(), path.end(), compareCoordinates);
}

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

bool isInRadius(int x, int y, int centerX, int centerY, int radius)
{
    int dx = abs(x - centerX);
    int dy = abs(y - centerY);
    return (dx * dx + dy * dy) <= (radius * radius);
}

class map
{
    int count = 0;
    int no = 0;
    int sum = 0;
    char *messaggeError;
    string data;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    sqlite3_stmt *stml;
    vector<vector<int>> grid;
    int *numCoords;
    vector<int> coords;

public:
    map()
    {
        vector<vector<int>> grid(GRID_SIZE, vector<int>(GRID_SIZE));

        int rc = sqlite3_open("New_network.db", &db);
        if (rc)
        {
            cerr << "Can't open database: " << endl
                 << sqlite3_errmsg(db);
        }
        else
        {
            cout << "Opened database successfully" << endl;
        }
        grid.resize(GRID_SIZE);
        for (int i = 0; i < GRID_SIZE; ++i)
        {
            grid[i].resize(13734);
        }

        cout << "Array created successfuly..." << endl;
    }

    bool AStar(const vector<vector<int>> &grid, int numRows, int numCols, int startX, int startY, int endX, int endY, vector<vector<int>> &path)
    {
        if (grid[startX][startY] == 0 || grid[endX][endY] == 0)
        {
            return false;
        }

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

            bool isStuck = true;
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
                        isStuck = false;
                    }
                }
            }

            // Check for alternative paths within a radius of 10
            if (isStuck)
            {
                for (int r = -10; r <= 10; r++)
                {
                    for (int c = -10; c <= 10; c++)
                    {
                        int curX = cur.x;
                        int curY = cur.y;
                        int nextX = cur.x + r;
                        int nextY = cur.y + c;

                        if (nextX >= 0 && nextX < numRows && nextY >= 0 && nextY < numCols && grid[nextX][nextY] == 1 && cost[nextX][nextY] == INT_MAX)
                        {
                            int newCost = cur.cost + 1;
                            cost[nextX][nextY] = newCost;
                            q.push(Node(nextX, nextY, newCost));
                            parent[nextX][nextY] = {cur.x, cur.y};
                            isStuck = false;
                            break;
                        }
                    }
                    if (!isStuck)
                    {
                        break;
                    }
                }
            }

            // Check if the current node is at the boundary of the loaded grid section
            if (cur.x == 0 || cur.x == numRows - 1 || cur.y == 0 || cur.y == numCols - 1)
            {

                int currentRow = cur.x / GRID_SIZE;
                int currentCol = cur.y / GRID_SIZE;

                // Load the next grid section
                int startRow = currentRow * GRID_SIZE;
                int endRow = startRow + GRID_SIZE;
                int startCol = currentCol * GRID_SIZE;
                int endCol = startCol + GRID_SIZE;

                vector<vector<int>> nextGrid(GRID_SIZE, vector<int>(GRID_SIZE));
                loader(startRow, endRow, startCol, endCol, nextGrid);

                int newX = cur.x % GRID_SIZE;
                int newY = cur.y % GRID_SIZE;

                int updatedStartX = newX;
                int updatedStartY = newY;
                int updatedEndX = endX % GRID_SIZE;
                int updatedEndY = endY % GRID_SIZE;
                int updatedNumRows = nextGrid.size();
                int updatedNumCols = nextGrid[0].size();
                vector<vector<int>> updatedPath;

                bool success = AStar(nextGrid, updatedNumRows, updatedNumCols, updatedStartX, updatedStartY, updatedEndX, updatedEndY, updatedPath);

                if (success)
                {
                    sortPath(updatedPath);

                    path.insert(path.end(), updatedPath.begin(), updatedPath.end());

                    for (const auto &point : updatedPath)
                    {
                        int x = point[0] + startRow;
                        int y = point[1] + startCol;
                        parent[x][y] = {cur.x, cur.y};
                    }
                }
                else
                {

                    return false;
                }
            }
        }

        return false;
    }

    void loader(int startRow, int endRow, int startCol, int endCol, vector<vector<int>> &grid)
    {
        cout << "Loading grid section..." << endl;
        string sql_query1 = "SELECT MIN(double1), MIN(double2) FROM myTable WHERE double1 >= " + to_string(convertb_2(startRow)) + " AND double1 < " + to_string(convertb_2(endRow)) + " AND double2 >= " + to_string(convertb_1(startCol)) + " AND double2 < " + to_string(convertb_1(endCol)) + ";";
        int rc = sqlite3_prepare_v2(db, sql_query1.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return;
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            cout << "First SQL statement..." << endl;
            double min1 = sqlite3_column_double(stmt, 0);
            double min2 = sqlite3_column_double(stmt, 1);

            string sql_query = "SELECT double1, double2 FROM myTable WHERE double1 >= " + to_string(convertb_2(startRow)) + " AND double1 < " + to_string(convertb_2(endRow)) + " AND double2 >= " + to_string(convertb_1(startCol)) + " AND double2 < " + to_string(convertb_1(endCol)) + ";";
            sqlite3_stmt *stmt;
            int rc = sqlite3_prepare_v2(db, sql_query.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK)
            {
                cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
                sqlite3_close(db);
                return;
            }

            // Bind the grid section coordinates to the SQL query
            // sqlite3_bind_int(stmt, 1, convertb_2(startRow));
            // sqlite3_bind_int(stmt, 2, convertb_2(endRow));
            // sqlite3_bind_int(stmt, 3, convertb_1(startCol));
            // sqlite3_bind_int(stmt, 4, convertb_1(endCol));

            int row = 0;
            int col = 0;

            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
            {
                double double1 = sqlite3_column_double(stmt, 0);
                double double2 = sqlite3_column_double(stmt, 1);
                cout << "Second SQL statement..." << endl;
                // cout << "Min1: " << min1 << endl;
                // cout << "Min2: " << min2 << endl;
                array(double1, double2, min1, min2);
            }

            if (rc != SQLITE_DONE)
            {
                cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
            }

            // Finalize the prepared statement
            sqlite3_finalize(stmt);
        }

        if (rc != SQLITE_DONE)
        {
            cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        }

        cout << "Grid section loaded." << endl;
    }

    void driver()
    {

        cout << "Calling A-star..." << endl;

        int startX, startY, endX, endY;
        startY = conv_2(76.76614);
        startX = conv_1(30.726899);
        cout << "starX: " << startX << endl;
        cout << "starY: " << startY << endl;
        endX = 7530;
        endY = 10401;

        int numGridsRow = (row + GRID_SIZE - 1) / GRID_SIZE;
        int numGridsCol = (col + GRID_SIZE - 1) / GRID_SIZE;

        int startGridRow = startX / GRID_SIZE;
        int startGridCol = startY / GRID_SIZE;

        // Load the initial grid section
        int startRow = startGridRow * GRID_SIZE;
        int endRow = startRow + GRID_SIZE;
        int startCol = startGridCol * GRID_SIZE;
        int endCol = startCol + GRID_SIZE;
        cout << "StartRow: " << startRow << endl;
        cout << "endRow: " << endRow << endl;
        cout << "startCol: " << startCol << endl;
        cout << "endCol: " << endCol << endl;
        loader(startRow, endRow, startCol, endCol, grid);

        // startX = 3592;
        // startY = 2604;
        // endX = 3639;
        // endY = 2564;

        // cin >> startX >> startY >> endX >> endY;
        cout
            << grid[startX][startY] << endl
            << grid[endX][endY] << endl;

        vector<vector<int>>
            path;
        vector<vector<int>>
            npath;
        bool foundPath = AStar(grid, GRID_SIZE, GRID_SIZE, startX % GRID_SIZE, startY % GRID_SIZE, endX % GRID_SIZE, endY % GRID_SIZE, path);

        // Print the output.
        if (foundPath)
        {
            cout << "The cost of the shortest path is " << path.size() - 1 << endl;

            cout << "The path is: " << endl;

            for (int i = 0; i < path.size(); i++)
            {

                if (i < path.size() - 1)
                {
                    if (abs(path[i + 1][0] - path[i][0]) > 1 || abs(path[i + 1][1] - path[i][1]) > 1)
                    {
                        drawLine(path[i + 1][0], path[i + 1][1], path[i][0], path[i][1], npath, i);
                    }
                }
                // cout << "(" << path[i][0] << ", " << path[i][1] << ") ";

                npath.push_back({path[i][0], path[i][1]});
                // cout << "(" << convertb_1(path[i][0]) << ", " << convertb_2(path[i][1]) << ") ";
            }

            cout << "The cost of the shortest path is " << npath.size() - 1 << endl;

            sortPath(npath);

            for (int i = 0; i < npath.size(); i++)
            {
                cout << "(" << npath[i][0] << ", " << npath[i][1] << ") ";
            }

            cout << endl;
        }
        else
        {
            cout << "No path found." << endl;
        }

        sqlite3_finalize(stmt);
        return;
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
    int convert_1(double double1, double min1)
    {
        double lati;
        double1 = round(double1 * 100000) / 100000;
        lati = (double1 - min1) * 100000;
        int lat_1 = lati;
        return lat_1;
    }
    int convert_2(double double2, double min2)
    {
        double longi;
        double2 = round(double2 * 100000) / 100000;
        longi = (double2 - min2) * 100000;
        int long_1 = longi;
        return long_1;
    }
    void array(double double1, double double2, double min1, double min2)
    {

        cout << "Min1: " << min1 << endl;
        cout << "Min2: " << min2 << endl;
        int lat_1 = convert_1(double1, min1);
        int long_1 = convert_2(double2, min2);
        grid[lat_1][long_1] = 1;
        cout << "Lat: " << lat_1 << endl;
        cout << "Long: " << long_1 << endl;
        sum++;
    }
    int conv_1(double double1)
    {
        double lati;
        double1 = round(double1 * 100000) / 100000;
        lati = (double1 - 30.65364) * 100000;
        int lat_1 = lati;
        return lat_1;
    }
    int conv_2(double double2)
    {
        double longi;
        double2 = round(double2 * 100000) / 100000;
        longi = (double2 - 76.66046) * 100000;
        int long_1 = longi;
        return long_1;
    }

    void drawLine(int x0, int y0, int x1, int y1, vector<vector<int>> &npath, int j)
    {

        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        while (x0 != x1 || y0 != y1)
        {
            // grid[x0][y0] = 1;
            // cout << "(" << x0 << ", " << y0 << ") ";
            npath.push_back({x0, y0});

            // path.insert(path.begin() + j, {x0, y0});

            int e2 = err * 2;
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
};

int main()
{
    map t;
    auto start = chrono::high_resolution_clock::now();

    t.driver();

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> duration = end - start;

    cout << "Execution time: " << duration.count() << " ms" << endl;

    return 0;
}

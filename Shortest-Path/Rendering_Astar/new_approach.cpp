#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <sqlite3.h>
#include <math.h>
#include <chrono>
#include <random>
#include <iomanip>
#include <ios>
using namespace std;

const int row = 13734;
const int col = 18622;
const int GRID_SIZE = 1000; // Size of each grid section
const double EarthRadiusKm = 6371.0;

double toRadians(double degrees)
{
    return degrees * M_PI / 180.0;
}

double calculateDistance(double lat1, double lon1, double lat2, double lon2)
{
    double a = 6378137.0;         // semi-major axis of the ellipsoid
    double b = 6356752.314245;    // semi-minor axis of the ellipsoid
    double f = 1 / 298.257223563; // flattening of the ellipsoid

    double L = toRadians(lon2 - lon1);

    double U1 = atan((1 - f) * tan(toRadians(lat1)));
    double U2 = atan((1 - f) * tan(toRadians(lat2)));
    double sinU1 = sin(U1), cosU1 = cos(U1);
    double sinU2 = sin(U2), cosU2 = cos(U2);

    double lambda = L;
    double lambdaPrev;
    double sinLambda, cosLambda;
    double sinSigma, cosSigma, sigma;
    double sinAlpha, cosSqAlpha;
    double cos2SigmaM;
    double C;

    do
    {
        sinLambda = sin(lambda);
        cosLambda = cos(lambda);
        sinSigma = sqrt((cosU2 * sinLambda) * (cosU2 * sinLambda) +
                        (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda) *
                            (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda));
        if (sinSigma == 0)
            return 0; // coincident points
        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
        sigma = atan2(sinSigma, cosSigma);
        sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
        cosSqAlpha = 1 - sinAlpha * sinAlpha;
        cos2SigmaM = cosSigma - 2 * sinU1 * sinU2 / cosSqAlpha;
        C = f / 16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        lambdaPrev = lambda;
        lambda = L +
                 (1 - C) * f * sinAlpha *
                     (sigma + C * sinSigma *
                                  (cos2SigmaM + C * cosSigma *
                                                    (-1 + 2 * cos2SigmaM * cos2SigmaM)));
    } while (std::abs(lambda - lambdaPrev) > 1e-12);

    double uSq = cosSqAlpha * (a * a - b * b) / (b * b);
    double A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
    double B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));
    double deltaSigma =
        B * sinSigma *
        (cos2SigmaM + B / 4 *
                          (cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM) -
                           B / 6 * cos2SigmaM * (-3 + 4 * sinSigma * sinSigma) *
                               (-3 + 4 * cos2SigmaM * cos2SigmaM)));

    double distance = b * A * (sigma - deltaSigma);

    return distance / 1000.0; // Convert to kilometers
}

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
            grid[i].resize(GRID_SIZE);
        }

        cout << "Array created successfuly..." << endl;
    }

    bool AStar(vector<vector<int>> &grid, int numRows, int numCols, int startX, int startY, int endX, int endY, int startRow, int endRow, int startCol, int endCol, vector<vector<int>> &path)
    {
        bool isStuck = true;
        cout << "Checking..." << endl;
        if (grid[startX][startY] == 0)
        {
            cout << "Out of boundary..." << endl;
            // return false;
            isStuck = true;
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
                while (cur.x != -1 && cur.y != -1)
                {
                    cout << "Exp: (" << cur.x << ", " << cur.y << ")" << endl;
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
                            // cout << "Checking Alternate..." << endl;
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

            if (cur.x == 0 || cur.x == numRows || cur.y == 0 || cur.y == numCols)
            {

                cout << "StartRow: " << startRow << endl;
                cout << "endRow: " << endRow << endl;
                cout << "startCol: " << startCol << endl;
                cout << "endCol: " << endCol << endl;

                vector<vector<int>> nextGrid;
                nextGrid.resize(GRID_SIZE);
                for (int i = 0; i < GRID_SIZE; ++i)
                {
                    nextGrid[i].resize(GRID_SIZE);
                }

                cout << "Cur_X: " << cur.x << endl;
                cout << "Cur_Y: " << cur.y << endl;

                int updatedStartX = 0;
                int updatedStartY = 0;

                // Diagnol edges exception
                if (cur.x == 0 && cur.y < numRows)
                {
                    cout << "New grid loaded to the left." << endl;
                    startRow -= GRID_SIZE;
                    endRow -= GRID_SIZE;
                    updatedStartX = cur.x + GRID_SIZE;
                    updatedStartY = cur.y;
                }
                else if (cur.x == numRows && cur.y > 0)
                {
                    cout << "New grid loaded to the right." << endl;
                    startRow += GRID_SIZE;
                    endRow += GRID_SIZE;
                    updatedStartX = cur.x - GRID_SIZE;
                    updatedStartY = cur.y;
                }
                else if (cur.x < numRows && cur.y == 0)
                {
                    cout << "New grid loaded below." << endl;
                    startCol -= GRID_SIZE;
                    endCol -= GRID_SIZE;
                    updatedStartX = cur.x;
                    updatedStartY = cur.y + GRID_SIZE;
                }
                else if (cur.x < numRows && cur.y == numRows)
                {
                    cout << "New grid loaded above." << endl;
                    startCol += GRID_SIZE;
                    endCol += GRID_SIZE;
                    updatedStartX = cur.x;
                    updatedStartY = cur.y - GRID_SIZE;
                }

                int updatedNumRows = numRows;
                int updatedNumCols = numRows;
                vector<vector<int>> updatedPath;

                cout << "StartX: " << updatedStartX << endl;
                cout << "StartY: " << updatedStartY << endl;

                cout << endl
                     << endl
                     << endl;
                cout << "StartRow: " << startRow << endl;
                cout << "EndRow: " << endRow << endl;
                cout << "StartCol: " << startCol << endl;
                cout << "EndCol: " << endCol << endl;

                loader(startRow, endRow, startCol, endCol, grid);

                bool success = AStar(grid, updatedNumRows, updatedNumCols, updatedStartX, updatedStartY, endX, endY, startRow, endRow, startCol, endCol, updatedPath);

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
        string sql_query1 = "SELECT MIN(double1), MIN(double2),MAX(double1),MAX(double2) FROM myTable WHERE double1 >= " + to_string(convertb_2(startRow)) + " AND double1 < " + to_string(convertb_2(endRow)) + " AND double2 >= " + to_string(convertb_1(startCol)) + " AND double2 < " + to_string(convertb_1(endCol)) + ";";
        int rc = sqlite3_prepare_v2(db, sql_query1.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return;
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            double min1 = sqlite3_column_double(stmt, 0);
            double min2 = sqlite3_column_double(stmt, 1);
            double max1 = sqlite3_column_double(stmt, 2);
            double max2 = sqlite3_column_double(stmt, 3);

            string sql_query = "SELECT double1, double2 FROM myTable WHERE double1 >= " + to_string(min1) + " AND double1 < " + to_string(max1) + " AND double2 >= " + to_string(min2) + " AND double2 < " + to_string(max2) + ";";
            sqlite3_stmt *stmt;
            int rc = sqlite3_prepare_v2(db, sql_query.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK)
            {
                cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
                sqlite3_close(db);
                return;
            }

            int row = 0;
            int col = 0;

            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
            {

                double double1 = sqlite3_column_double(stmt, 0);
                double double2 = sqlite3_column_double(stmt, 1);
                array(double1, double2, min1, min2, grid);
            }
            cout << "Grid..." << endl;

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
        cout << "Sum: " << sum << endl;
        cout << "Grid section loaded." << endl;
    }

    void driver()
    {

        cout << "Calling A-star..." << endl;

        int startX, startY, endX, endY;
        startX = 10567;
        startY = 7326;
        cout << "starX: " << convertb_2(startX) << endl;
        cout << "starY: " << convertb_1(startY) << endl;
        endX = 10401;
        endY = 7530;
        cout << "endX: " << convertb_2(endX) << endl;
        cout << "endY: " << convertb_1(endY) << endl;

        int startRow = (startX / GRID_SIZE) * GRID_SIZE;
        int startCol = (startY / GRID_SIZE) * GRID_SIZE;
        int endRow = startRow + GRID_SIZE;
        int endCol = startCol + GRID_SIZE;

        cout << endl
             << endl;
        cout << "Initial..." << endl;
        cout << "StartRow: " << startRow << endl;
        cout << "endRow: " << endRow << endl;
        cout << "startCol: " << startCol << endl;
        cout << "endCol: " << endCol << endl;

        loader(startRow, endRow, startCol, endCol, grid);

        cout << "Hello..." << endl;
        cout << "StarX:" << startX - startRow << endl;
        cout << "StarY:" << startY - startCol << endl;
        cout << "endX: " << endX - startRow << endl;
        cout << "endY: " << endY - startCol << endl;

        cout << grid[startX - startRow][startY - startCol] << endl;

        vector<vector<int>>
            path;
        vector<vector<int>>
            npath;
        bool foundPath = AStar(grid, GRID_SIZE, GRID_SIZE, startX - startRow, startY - startCol, endX - startRow, endY - startCol, startRow, endRow, startCol, endCol, path);

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
                npath.push_back({path[i][0], path[i][1]});
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
    void array(double double1, double double2, double min1, double min2, vector<vector<int>> &nextgrid)
    {
        int lat_1 = convert_1(double1, min1);
        int long_1 = convert_2(double2, min2);
        nextgrid[lat_1][long_1] = 1;
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
            npath.push_back({x0, y0});

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

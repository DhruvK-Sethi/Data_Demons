#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <cmath>
#include <sqlite3.h>
#include <queue>
#include <algorithm>
#include <climits>
#include <math.h>
#include <random>
#include <iomanip>
#include <ios>

using namespace std;

const double TIME_STEP = 0.1;
const double MAX_SPEED = 20.0;

using namespace std;

const int row = 13734;
const int col = 18622;
const int SMALL_MAP_SIZE = 1000;
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

bool compareCoordinates(std::vector<int> &a, std::vector<int> &b)
{
    if (a[0] != b[0])
    {
        return a[0] < b[0];
    }
    return a[1] < b[1];
}

void sortPath(std::vector<std::vector<int>> &path)
{
    std::sort(path.begin(), path.end(), compareCoordinates);
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

bool AStar(std::vector<std::vector<int>> &grid, int startX, int startY, int endX, int endY, std::vector<std::vector<int>> &path)
{
    if (grid[startX][startY] == 0 || grid[endX][endY] == 0)
    {
        return false;
    }

    int numRows = grid.size();
    int numCols = grid[0].size();

    std::priority_queue<Node> q;
    std::vector<std::vector<int>> cost(numRows, std::vector<int>(numCols, INT_MAX));
    std::vector<std::vector<std::pair<int, int>>> parent(numRows, std::vector<std::pair<int, int>>(numCols, {-1, -1}));

    q.push(Node(startX, startY, 0));
    cost[startX][startY] = 0;

    std::vector<std::vector<int>> directions = {
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
    }
    return false;
}

class Car
{
public:
    Car(int source, int destination, double speed = 1.0, double acceleration = 2.0, double brake = 3.0,
        double mass = 1000.0, double maxSteeringAngle = 45.0, double perceptionTime = 0.5)
        : source(source), destination(destination), position(source), speed(speed),
          acceleration(acceleration), brake(brake), mass(mass), maxSteeringAngle(maxSteeringAngle),
          perceptionTime(perceptionTime), steeringAngle(0.0) {}

    void move(double deltaTime)
    {
        position += speed * deltaTime;
    }

    void accelerate(double deltaTime)
    {

        speed += acceleration * deltaTime;

        if (speed > MAX_SPEED)
        {
            speed = MAX_SPEED;
        }
    }

    void brakeCar(double deltaTime)
    {

        speed -= brake * deltaTime;

        if (speed < 0.0)
        {
            speed = 0.0;
        }
    }

    void stop()
    {

        speed = 0.0;
    }

    void steer(double deltaTime, double targetAngle)
    {

        double angleDiff = targetAngle - steeringAngle;

        double maxSteeringChange = maxSteeringAngle * deltaTime;

        if (abs(angleDiff) > maxSteeringChange)
        {
            angleDiff = copysign(maxSteeringChange, angleDiff);
        }

        steeringAngle += angleDiff;
    }

    bool isAtDestination()
    {

        return position >= destination;
    }

    void update(bool traffic, double deltaTime)
    {
        double perceptionDistance = speed * perceptionTime;

        if (traffic)
        {
            brakeCar(deltaTime);
        }
        else
        {
            accelerate(deltaTime);
        }

        if (speed <= 0)
        {
            stop();
        }

        double targetAngle = 0.0;
        if (position + perceptionDistance < destination)
        {
            targetAngle = maxSteeringAngle;
        }

        steer(deltaTime, targetAngle);

        move(deltaTime);
    }

private:
    int source;
    int destination;
    double position;
    double speed;
    double acceleration;
    double brake;
    double mass;
    double maxSteeringAngle;
    double perceptionTime;
    double steeringAngle;
    vector<vector<int>> path;
};

class Map
{
    int count = 0;
    int no = 0;
    int sum = 0;
    char *messageError;
    string data;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    sqlite3_stmt *stml;
    vector<vector<int>> myArray;
    vector<int> coords;
    double ncost = 0;
    vector<vector<int>> path;
    vector<vector<int>> npath;
    vector<vector<double>> fpath;

public:
    Map()
    {
        // cout << "Constructor..." << endl;
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
        myArray.resize(18622);
        for (int i = 0; i < 18622; ++i)
        {
            myArray[i].resize(13734);
        }
        cout << "Array created successfully..." << endl;
    }

    void reset()
    {
        // Reset all necessary variables and vectors to their initial values
        count = 0;
        no = 0;
        sum = 0;
        coords.clear();
        path.clear();
        npath.clear();
        fpath.clear();
        ncost = 0;
    }

    void loader()
    {
        reset();
        cout << "Checking..." << endl;
        string sql_query1 = "SELECT double1, double2 FROM myTable ;";
        int rc = sqlite3_prepare_v2(db, sql_query1.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
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
        cout << "Process finished: " << sum << endl;
        cout << myArray[3591][2603] << endl
             << myArray[3633][2568] << endl;

        if (rc != SQLITE_DONE)
        {
            cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
        stmt = nullptr; // Clear the stmt variable
    }

    vector<vector<double>> driver(double sX, double sY, double eX, double eY)
    {
        // loader();

        cout << "Calling A-star..." << endl;

        int startX, startY, endX, endY;
        startX = convert_1(sX);
        startY = convert_2(sY);
        endX = convert_1(eX);
        endY = convert_2(eY);

        cout << fixed << setprecision(5) << "EndX: " << convertb_2(endX) << endl
             << "EndY: " << convertb_1(endY) << endl;

        cout << myArray[startX][startY] << endl
             << myArray[endX][endY] << endl;

        bool foundPath = AStar(myArray, startX, startY, endX, endY, path);

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

            cout << "Step1..." << endl;

            sortPath(npath);
            for (int i = 0; i < npath.size() - 1; i++)
            {
                double startx = convertb_1(npath[i][0]);
                double starty = convertb_2(npath[i][1]);
                double endx = convertb_1(npath[i + 1][0]);
                double endy = convertb_2(npath[i + 1][1]);

                ncost += calculateDistance(startx, starty, endx, endy);
            }
            cout << "Step2..." << endl;
            path.clear();
            for (int i = 0; i < npath.size(); i++)
            {
                fpath.push_back({convertb_1(npath[i][0]), convertb_2(npath[i][1])});
            }

            cout << endl;
            cout << "Total cost: " << ncost / 100 << endl;
            cout << "The cost of the shortest path is " << npath.size() << endl;
        }
        else
        {
            cout << "No path found." << endl;
        }

        return fpath;
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

    ~Map()
    {
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        myArray.clear();
        coords.clear();
        path.clear();
        npath.clear();
        fpath.clear();
    }
};

class Db
{
    vector<vector<double>> inputs;
    vector<vector<double>> path;
    sqlite3_stmt *stmt;
    sqlite3 *db;
    int rc;
    int max1, vmax1, max2, vmax2;
    int source, destination;
    int count = 0;
    Map m;

public:
    Db()
    {

        rc = sqlite3_open("roads.db", &db);
        if (rc)
        {
            cerr << "Can't open database: " << endl
                 << sqlite3_errmsg(db);
        }
        else
        {
            cout << "Opened database successfully" << endl;
        }
    }
    int get_max(int temp)
    {
        int max, vmax;
        string sql_query2 = "SELECT MAX(vectorIndex) FROM myTable WHERE mapkey=" + to_string(temp) + " ;";

        rc = sqlite3_prepare_v2(db, sql_query2.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return 0;
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            cout << "Checkimg...." << endl;
            max = sqlite3_column_int(stmt, 0);
            cout << max << endl;
            vmax = (rand() % max) + 1;
        }
        sqlite3_finalize(stmt);
        return vmax;
    }

    void genrand()
    {
        path.clear();
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, 11106);

        source = dis(gen);
        destination = dis(gen);

        while (source == destination)
        {
            destination = dis(gen);
        }
        cout << "Source: " << source << endl
             << "Destination: " << destination << endl;
        vmax1 = get_max(source);
        vmax2 = get_max(destination);
        // cout << "Vmax1: " << vmax1 << endl
        //      << "Vmax2: " << vmax2 << endl;
        getval(source, vmax1);
        getval(destination, vmax2);
        // cout << "Long1: " << inputs[0][0] << endl
        //      << "Lat1: " << inputs[0][1] << endl
        //      << "Long2: " << inputs[1][0] << endl
        //      << "Lat2: " << inputs[1][1] << endl;
    }

    void getpath()
    {
        cout << "Count: " << count << endl;
        count++;
        genrand();
        path = m.driver(inputs[0][1], inputs[0][0], inputs[1][1], inputs[1][0]);

        if (path.size() == 0)
        {
            getpath();
        }

        // for (int i = 0; i < path.size(); i++)
        // {
        //     cout << "(" << path[i][0] << ", " << path[i][1] << ") ";
        // }
        if (path.size() != 0)
        {
            cout << "Found path of lenght: " << path.size() << endl;
        }
    }
    void driver()
    {
        m.loader(); // pls call only 1 time

        for (int i = 0; i < 3; i++)
        {
            getpath();
        }
    }

    void getval(int source, int vectorIn_1)
    {
        string sql_query1 = "SELECT double1, double2 FROM myTable WHERE mapkey=" + to_string(source) + " AND vectorIndex= " + to_string(vectorIn_1) + ";";

        rc = sqlite3_prepare_v2(db, sql_query1.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return;
        }

        double double1, double2;

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            double1 = sqlite3_column_double(stmt, 0);
            double2 = sqlite3_column_double(stmt, 1);
            cout << "Latitude: " << double2 << endl
                 << "Longitude: " << double1 << endl;
            inputs.push_back({double1, double2});
        }
        sqlite3_finalize(stmt);
    }
    ~Db()
    {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
};

int main()
{

    Db b;
    b.driver();

    // vector<Car> cars;
    // srand(static_cast<unsigned>(time(nullptr)));

    // Car car(source, destination);
    // cars.push_back(car);

    // double deltaTime = TIME_STEP;
    // bool traffic = false;
    // while (true)
    // {
    //     auto startTime = chrono::high_resolution_clock::now();

    //     for (auto &car : cars)
    //     {
    //         car.update(traffic, deltaTime);
    //     }

    //     traffic = rand() % 2 == 0;

    //     auto endTime = chrono::high_resolution_clock::now();
    //     auto elapsedTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    //     long long remainingTime = static_cast<long long>((TIME_STEP * 1000) - elapsedTime);
    //     if (remainingTime > 0)
    //     {
    //         this_thread::sleep_for(chrono::milliseconds(remainingTime));
    //     }
    //     else
    //     {
    //         cout << "Simulation is running behind the desired time step." << endl;
    //     }
    // }

    return 0;
}

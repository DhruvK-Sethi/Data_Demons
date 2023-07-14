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

using namespace std;

const double TIME_STEP = 0.1;
const double MAX_SPEED = 20.0;

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
};

class Db
{
    sqlite3_stmt *stmt;
    sqlite3 *db;
    int rc;

public:
    Db()
    {

        rc = sqlite3_open("New_network.db", &db);
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
        return vmax;
    }
    void driver()
    {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, 11106);

        int max1, vmax1, max2, vmax2;
        int source = dis(gen);
        int destination = dis(gen);

        while (source == destination)
        {
            destination = dis(gen);
        }
        cout << "Source: " << source << endl
             << "Destination: " << destination << endl;
        vmax1 = get_max(source);
        vmax2 = get_max(destination);
        cout << "Vmax1: " << vmax1 << endl
             << "Vmax2: " << vmax2 << endl;
        getval(source, destination, vmax1, vmax2);
    }

    void getval(int source, int destination, int vectorIn_1, int vectorIn_2)
    {
        string sql_query1 = "SELECT double1, double2 FROM myTable WHERE mapkey=" + to_string(source) + " AND vectorIndex= " + to_string(vectorIn_1) + " OR mapkey= " + to_string(destination) + " AND vectorIndex= " + to_string(vectorIn_2) + ";";

        rc = sqlite3_prepare_v2(db, sql_query1.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return;
        }
        // cout << "testing..." << endl;

        double double1, double2;

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            double1 = sqlite3_column_double(stmt, 0);
            double2 = sqlite3_column_double(stmt, 1);
            cout << "Latitude: " << double2 << endl
                 << "Longitude: " << double1 << endl;
        }
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

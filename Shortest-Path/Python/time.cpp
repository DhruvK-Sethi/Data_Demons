#include <iostream>
#include <string>

using namespace std;
int main()
{
    string vehicle_id;
    cin >> vehicle_id;
    float vf, vi, a, d;
    cin >> vf >> vi >> a;
    int time = ((vf - vi) / a) + (d / vf);
    cout << time << endl;
    return 0;
}
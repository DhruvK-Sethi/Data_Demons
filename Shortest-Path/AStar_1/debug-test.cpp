#include <iostream>
#include <vector>
#include <random>
using namespace std;

std::vector<std::vector<int>> generateGrid(int numRows, int numCols)
{
    std::vector<std::vector<int>> grid(numRows, std::vector<int>(numCols, 0));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 9);

    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            // Randomly assign paths and obstacles
            if (dist(gen) < 8) // Adjust the threshold to control the density of paths
                grid[i][j] = 1;
        }
    }

    return grid;
}

int main()
{
    int numRows = 30;
    int numCols = 30;
    std::vector<std::vector<int>> grid = generateGrid(numRows, numCols);

    // Display the generated grid
    for (const auto &row : grid)
    {
        cout << "{";
        for (const auto &cell : row)
        {
            std::cout << cell << ", ";
        }
        cout << "},";
        std::cout << std::endl;
    }

    return 0;
}

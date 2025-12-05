#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int countAdjacentRolls(const std::vector<std::string>& grid, int row, int col)
{
    int rows = grid.size();
    int cols = grid[0].size();
    int count = 0;

    // Check all 8 adjacent positions
    int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    for (int i = 0; i < 8; i++) {
        int newRow = row + dx[i];
        int newCol = col + dy[i];

        // Check bounds
        if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
            if (grid[newRow][newCol] == '@') {
                count++;
            }
        }
    }

    return count;
}

int main()
{
    std::ifstream inputFile("inputs/input.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open inputs/input.txt" << std::endl;
        return 1;
    }

    std::vector<std::string> grid;
    std::string line;

    // Read the grid
    while (std::getline(inputFile, line)) {
        grid.push_back(line);
    }

    inputFile.close();

    int totalRemoved = 0;
    bool changed = true;

    // Keep removing rolls until no more can be accessed
    while (changed) {
        changed = false;
        std::vector<std::pair<int, int>> toRemove;

        // Find all accessible rolls in current state
        for (int row = 0; row < grid.size(); row++) {
            for (int col = 0; col < grid[row].size(); col++) {
                // If this position has a roll of paper
                if (grid[row][col] == '@') {
                    // Count adjacent rolls
                    int adjacentRolls = countAdjacentRolls(grid, row, col);

                    // Accessible if fewer than 4 adjacent rolls
                    if (adjacentRolls < 4) {
                        toRemove.push_back({ row, col });
                    }
                }
            }
        }

        // Remove all accessible rolls
        if (!toRemove.empty()) {
            changed = true;
            totalRemoved += toRemove.size();

            for (auto& pos : toRemove) {
                grid[pos.first][pos.second] = '.';
            }
        }
    }

    std::cout << totalRemoved << std::endl;

    return 0;
}

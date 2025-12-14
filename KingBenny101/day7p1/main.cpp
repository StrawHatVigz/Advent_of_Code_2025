#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

static inline long long key_from(int r, int c)
{
    return (static_cast<long long>(r) << 32) | static_cast<unsigned long long>(c);
}

int main()
{
    std::ifstream inputFile("inputs/input.txt");
    if (!inputFile.is_open()) {
        // fallback to test
        inputFile.open("inputs/test.txt");
        if (!inputFile.is_open()) {
            std::cerr << "Error: Could not open inputs/input.txt or inputs/test.txt" << std::endl;
            return 1;
        }
    }

    std::vector<std::string> grid;
    std::string line;
    while (std::getline(inputFile, line)) {
        // keep exact spacing
        grid.push_back(line);
    }
    inputFile.close();

    if (grid.empty()) {
        std::cout << 0 << std::endl;
        return 0;
    }

    int rows = static_cast<int>(grid.size());
    int cols = 0;
    for (auto& l : grid)
        if ((int)l.size() > cols)
            cols = (int)l.size();
    for (auto& l : grid)
        if ((int)l.size() < cols)
            l.resize(cols, ' ');

    int sr = -1, sc = -1;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] == 'S') {
                sr = r;
                sc = c;
                break;
            }
        }
        if (sr != -1)
            break;
    }

    if (sr == -1) {
        std::cerr << "No S found in grid" << std::endl;
        return 1;
    }

    std::queue<std::pair<int, int>> q;
    std::unordered_set<long long> visited;

    // Starting beam: just below S
    int start_r = sr + 1;
    if (start_r < rows) {
        q.emplace(start_r, sc);
    }

    long long splits = 0;
    std::unordered_set<long long> split_visited;

    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();
        if (r < 0 || r >= rows || c < 0 || c >= cols)
            continue;
        long long k = key_from(r, c);
        if (visited.count(k))
            continue;
        visited.insert(k);

        int cur_r = r;
        while (cur_r < rows) {
            char ch = grid[cur_r][c];
            if (ch == '.' || ch == 'S' || ch == ' ') {
                // empty, continue downward
                ++cur_r;
                continue;
            }
            if (ch == '^') {
                long long split_k = key_from(cur_r, c);
                if (!split_visited.count(split_k)) {
                    // split: spawn left and right beams at same row
                    ++splits;
                    split_visited.insert(split_k);
                    int left_c = c - 1;
                    int right_c = c + 1;
                    if (left_c >= 0) {
                        long long lk = key_from(cur_r, left_c);
                        if (!visited.count(lk)) {
                            q.emplace(cur_r, left_c);
                        }
                    }
                    if (right_c < cols) {
                        long long rk = key_from(cur_r, right_c);
                        if (!visited.count(rk)) {
                            q.emplace(cur_r, right_c);
                        }
                    }
                }
                break; // this beam stops at the splitter
            }
            // anything else (including space) counts as no path; break
            break;
        }
    }

    std::cout << splits << std::endl;

    return 0;
}

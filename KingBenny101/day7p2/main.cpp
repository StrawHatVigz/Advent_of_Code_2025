#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    std::string path = "inputs/input.txt";
    if (argc > 1)
        path = argv[1];
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        // if default full input not present, try test
        if (path == "inputs/input.txt") {
            inputFile.open("inputs/test.txt");
            if (!inputFile.is_open()) {
                std::cerr << "Error: Could not open inputs/input.txt or inputs/test.txt" << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Error: Could not open input file: " << path << std::endl;
            return 1;
        }
    }

    std::vector<std::string> grid;
    std::string line;
    while (std::getline(inputFile, line))
        grid.push_back(line);
    inputFile.close();

    if (grid.empty()) {
        std::cout << 0 << std::endl;
        return 0;
    }

    int rows = (int)grid.size();
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
        std::cerr << "No S found" << std::endl;
        return 1;
    }

    using u128 = unsigned long long; // assume fits; can be replaced with larger integer
    std::vector<std::vector<u128>> counts(rows, std::vector<u128>(cols, 0));
    std::vector<std::vector<char>> inQueue(rows, std::vector<char>(cols, 0));
    std::queue<std::pair<int, int>> q;

    auto enqueue = [&](int r, int c, u128 add) {
        if (r < 0 || r >= rows || c < 0 || c >= cols)
            return;
        bool wasZero = (counts[r][c] == 0);
        counts[r][c] += add;
        if (!inQueue[r][c]) {
            q.emplace(r, c);
            inQueue[r][c] = 1;
        }
    };

    if (sr + 1 < rows) {
        counts[sr + 1][sc] = 1;
        q.emplace(sr + 1, sc);
        inQueue[sr + 1][sc] = 1;
    }

    unsigned long long total_timelines = 0;

    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();
        inQueue[r][c] = 0;
        u128 cur = counts[r][c];
        counts[r][c] = 0;
        if (cur == 0)
            continue;
        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            total_timelines += (unsigned long long)cur;
            continue;
        }
        char ch = grid[r][c];
        if (ch == '^') {
            // split: add to left and right at same row
            if (c - 1 >= 0)
                enqueue(r, c - 1, cur);
            if (c + 1 < cols)
                enqueue(r, c + 1, cur);
        } else if (ch == '.' || ch == ' ' || ch == 'S') {
            // continue downward
            if (r + 1 >= rows) {
                total_timelines += (unsigned long long)cur;
            } else
                enqueue(r + 1, c, cur);
        } else {
            // other characters: treat as block/stop -> consider timeline ended
            total_timelines += (unsigned long long)cur;
        }
    }

    std::cout << total_timelines << std::endl;
    return 0;
}

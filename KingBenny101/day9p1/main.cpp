#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    std::string path = "inputs/input.txt";
    if (argc > 1)
        path = argv[1];
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        if (path == "inputs/input.txt")
            inputFile.open("inputs/test.txt");
        if (!inputFile.is_open()) {
            std::cerr << "Error: Could not open input file" << std::endl;
            return 1;
        }
    }

    std::vector<std::pair<long long, long long>> pts;
    std::string line;
    while (std::getline(inputFile, line)) {
        if (line.empty())
            continue;
        long long x, y;
        char c;
        std::stringstream ss(line);
        ss >> x >> c >> y;
        pts.emplace_back(x, y);
    }
    inputFile.close();

    long long best = 0;
    for (size_t i = 0; i < pts.size(); ++i) {
        for (size_t j = i + 1; j < pts.size(); ++j) {
            long long dx = std::llabs(pts[i].first - pts[j].first) + 1; // inclusive
            long long dy = std::llabs(pts[i].second - pts[j].second) + 1;
            long long area = dx * dy;
            if (area > best)
                best = area;
        }
    }

    std::cout << best << std::endl;
    return 0;
}

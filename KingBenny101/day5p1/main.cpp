#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Range {
    long long start;
    long long end;
};

bool isFresh(long long id, const std::vector<Range>& ranges)
{
    for (const auto& range : ranges) {
        if (id >= range.start && id <= range.end) {
            return true;
        }
    }
    return false;
}

int main()
{
    std::ifstream inputFile("inputs/input.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open inputs/input.txt" << std::endl;
        return 1;
    }

    std::vector<Range> freshRanges;
    std::vector<long long> availableIds;
    std::string line;
    bool readingRanges = true;

    while (std::getline(inputFile, line)) {
        // Empty line separates ranges from available IDs
        if (line.empty()) {
            readingRanges = false;
            continue;
        }

        if (readingRanges) {
            // Parse range: "3-5"
            size_t dashPos = line.find('-');
            if (dashPos != std::string::npos) {
                long long start = std::stoll(line.substr(0, dashPos));
                long long end = std::stoll(line.substr(dashPos + 1));
                freshRanges.push_back({ start, end });
            }
        } else {
            // Parse available ingredient ID
            availableIds.push_back(std::stoll(line));
        }
    }

    inputFile.close();

    // Count fresh ingredients
    int freshCount = 0;
    for (long long id : availableIds) {
        if (isFresh(id, freshRanges)) {
            freshCount++;
        }
    }

    std::cout << freshCount << std::endl;

    return 0;
}

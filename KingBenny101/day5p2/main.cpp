#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Range {
    long long start;
    long long end;
};

int main()
{
    std::ifstream inputFile("inputs/input.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open inputs/input.txt" << std::endl;
        return 1;
    }

    std::vector<Range> freshRanges;
    std::string line;

    // Read only the fresh ingredient ID ranges (ignore second section)
    while (std::getline(inputFile, line)) {
        // Stop at blank line
        if (line.empty()) {
            break;
        }

        // Parse range: "3-5"
        size_t dashPos = line.find('-');
        if (dashPos != std::string::npos) {
            long long start = std::stoll(line.substr(0, dashPos));
            long long end = std::stoll(line.substr(dashPos + 1));
            freshRanges.push_back({ start, end });
        }
    }

    inputFile.close();

    // Sort ranges by start position
    std::sort(freshRanges.begin(), freshRanges.end(),
        [](const Range& a, const Range& b) { return a.start < b.start; });

    // Merge overlapping ranges and count total IDs
    long long totalFreshIds = 0;
    long long currentStart = freshRanges[0].start;
    long long currentEnd = freshRanges[0].end;

    for (size_t i = 1; i < freshRanges.size(); i++) {
        if (freshRanges[i].start <= currentEnd + 1) {
            // Ranges overlap or are adjacent, merge them
            currentEnd = std::max(currentEnd, freshRanges[i].end);
        } else {
            // No overlap, count current range and start new one
            totalFreshIds += (currentEnd - currentStart + 1);
            currentStart = freshRanges[i].start;
            currentEnd = freshRanges[i].end;
        }
    }

    // Add the last range
    totalFreshIds += (currentEnd - currentStart + 1);

    std::cout << totalFreshIds << std::endl;

    return 0;
}

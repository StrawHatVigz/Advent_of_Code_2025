#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

bool isInvalidID(long long id)
{
    std::string idStr = std::to_string(id);
    int len = idStr.length();

    // Try all possible pattern lengths (from 1 digit up to half the length)
    // The pattern must repeat at least twice
    for (int patternLen = 1; patternLen <= len / 2; patternLen++) {
        // Check if the length is divisible by pattern length
        if (len % patternLen != 0) {
            continue;
        }

        std::string pattern = idStr.substr(0, patternLen);
        bool isRepeating = true;

        // Check if the entire ID is made of this pattern repeated
        for (int i = patternLen; i < len; i += patternLen) {
            if (idStr.substr(i, patternLen) != pattern) {
                isRepeating = false;
                break;
            }
        }

        if (isRepeating) {
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

    std::string line;
    std::getline(inputFile, line);

    long long totalSum = 0;
    std::stringstream ss(line);
    std::string range;

    // Parse comma-separated ranges
    while (std::getline(ss, range, ',')) {
        // Find the dash separator
        size_t dashPos = range.find('-');
        if (dashPos == std::string::npos) {
            continue;
        }

        long long start = std::stoll(range.substr(0, dashPos));
        long long end = std::stoll(range.substr(dashPos + 1));

        // Check each ID in the range
        for (long long id = start; id <= end; id++) {
            if (isInvalidID(id)) {
                totalSum += id;
            }
        }
    }

    std::cout << totalSum << std::endl;

    inputFile.close();
    return 0;
}

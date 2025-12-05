#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

bool isInvalidID(long long id)
{
    std::string idStr = std::to_string(id);
    int len = idStr.length();

    // ID must have even length to be repeated twice
    if (len % 2 != 0) {
        return false;
    }

    int halfLen = len / 2;
    std::string firstHalf = idStr.substr(0, halfLen);
    std::string secondHalf = idStr.substr(halfLen, halfLen);

    // Check if first half equals second half (repeated twice)
    return firstHalf == secondHalf;
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

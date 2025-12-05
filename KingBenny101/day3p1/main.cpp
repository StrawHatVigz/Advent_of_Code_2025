#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

int findMaxJoltage(const std::string& bank)
{
    int maxJoltage = 0;

    // Try all pairs of batteries (positions i and j where i < j)
    for (size_t i = 0; i < bank.length(); i++) {
        for (size_t j = i + 1; j < bank.length(); j++) {
            // Form the two-digit number from positions i and j
            int joltage = (bank[i] - '0') * 10 + (bank[j] - '0');
            maxJoltage = std::max(maxJoltage, joltage);
        }
    }

    return maxJoltage;
}

int main()
{
    std::ifstream inputFile("inputs/input.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open inputs/input.txt" << std::endl;
        return 1;
    }

    int totalJoltage = 0;
    std::string line;

    while (std::getline(inputFile, line)) {
        int maxJoltage = findMaxJoltage(line);
        totalJoltage += maxJoltage;
    }

    std::cout << totalJoltage << std::endl;

    inputFile.close();
    return 0;
}

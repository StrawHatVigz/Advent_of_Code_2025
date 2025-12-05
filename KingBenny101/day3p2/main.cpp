#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

long long findMaxJoltage(const std::string& bank)
{
    int n = bank.length();
    int needed = 12; // Number of batteries to select
    int canSkip = n - needed; // Number we can skip

    std::string result = "";
    int skipped = 0;

    // Greedy approach: for each position, pick the largest digit
    // that still allows us to select enough remaining digits
    for (int i = 0; i < n && result.length() < 12; i++) {
        int remaining = n - i - 1; // Digits after position i
        int stillNeed = 12 - result.length(); // How many more we need

        // Can we skip this digit?
        if (remaining >= stillNeed) {
            // We can afford to skip, so look ahead for a better digit
            char maxDigit = bank[i];
            int maxPos = i;

            // Look ahead to find the maximum digit we can use
            for (int j = i; j <= i + canSkip - skipped && j < n; j++) {
                if (bank[j] > maxDigit) {
                    maxDigit = bank[j];
                    maxPos = j;
                }
            }

            // Skip all digits before maxPos
            skipped += (maxPos - i);
            result += maxDigit;
            i = maxPos; // Move to the position we selected
        } else {
            // Must take this digit
            result += bank[i];
        }
    }

    long long joltage = std::stoll(result);
    return joltage;
}

int main()
{
    std::ifstream inputFile("inputs/input.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open inputs/input.txt" << std::endl;
        return 1;
    }

    long long totalJoltage = 0;
    std::string line;

    while (std::getline(inputFile, line)) {
        long long maxJoltage = findMaxJoltage(line);
        totalJoltage += maxJoltage;
    }

    std::cout << totalJoltage << std::endl;

    inputFile.close();
    return 0;
}

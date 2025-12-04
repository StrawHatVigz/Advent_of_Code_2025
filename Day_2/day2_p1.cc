#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>

// Function to check if a number constitutes a "repeated twice" pattern
// e.g. 1212 -> "12" + "12" -> true
bool isInvalidID(long long n) {
    std::string s = std::to_string(n);
    
    // Must be even length to be split into two equal halves
    if (s.length() % 2 != 0) {
        return false;
    }

    std::string firstHalf = s.substr(0, s.length() / 2);
    std::string secondHalf = s.substr(s.length() / 2);

    // Use hashset logic as requested to verify repetition
    std::unordered_set<std::string> parts;
    parts.insert(firstHalf);
    
    // If the second half is in the set, it's a repeat
    return parts.count(secondHalf);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open " << argv[1] << std::endl;
        return 1;
    }

    std::string line;
    long long totalSum = 0;

    // FIX: Use a while loop to read ALL lines in the file.
    // The previous version only read the first line, causing "Too Low" results
    // if the input was split across multiple lines.
    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string segment;

        while (std::getline(ss, segment, ',')) {
            // Robustness: Trim whitespace around segment
            size_t first = segment.find_first_not_of(" \t\r\n");
            if (first == std::string::npos) continue;
            size_t last = segment.find_last_not_of(" \t\r\n");
            segment = segment.substr(first, (last - first + 1));

            size_t dashPos = segment.find('-');
            if (dashPos != std::string::npos) {
                try {
                    long long start = std::stoll(segment.substr(0, dashPos));
                    long long end = std::stoll(segment.substr(dashPos + 1));

                    for (long long i = start; i <= end; ++i) {
                        if (isInvalidID(i)) {
                            totalSum += i;
                        }
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing range: " << segment << std::endl;
                }
            }
        }
    }

    std::cout << "Sum of all invalid IDs: " << totalSum << std::endl;

    return 0;
}
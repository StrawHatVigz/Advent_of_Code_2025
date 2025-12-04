#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>

// Updated Logic: Check if number is a sequence repeated *at least* twice.
// Examples: 123123 (repeat 2x), 121212 (repeat 3x), 1111 (repeat 4x)
bool isInvalidID(long long n) {
    std::string s = std::to_string(n);
    int totalLen = s.length();

    // Try all possible substring lengths 'k' that could form the pattern.
    // Since it must repeat at least twice, the substring cannot be longer than half the total length.
    for (int subLen = 1; subLen <= totalLen / 2; ++subLen) {
        
        // The total length must be cleanly divisible by the substring length
        // e.g. "123123123" (len 9) can be chunks of 3. It cannot be chunks of 2.
        if (totalLen % subLen == 0) {
            std::unordered_set<std::string> uniqueParts;
            
            // Split the string into chunks of size 'subLen'
            for (int i = 0; i < totalLen; i += subLen) {
                std::string chunk = s.substr(i, subLen);
                uniqueParts.insert(chunk);
                
                // Optimization: If we already found 2 different chunks, 
                // this subLen is definitely not the answer.
                if (uniqueParts.size() > 1) break;
            }

            // YOUR LOGIC: If the hashset has no duplicates (size == 1),
            // it means the entire string is just that one chunk repeated.
            if (uniqueParts.size() == 1) {
                return true;
            }
        }
    }

    return false;
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
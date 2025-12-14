#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::ifstream inputFile("inputs/input.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open inputs/input.txt" << std::endl;
        return 1;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }
    inputFile.close();

    size_t max_len = 0;
    for (const auto& l : lines) {
        if (l.size() > max_len)
            max_len = l.size();
    }
    // Pad lines to max_len with spaces
    for (auto& l : lines) {
        l.resize(max_len, ' ');
    }

    if (lines.empty()) {
        std::cout << 0 << std::endl;
        return 0;
    }

    int rows = lines.size();
    int cols = max_len;

    // Find separators: columns where all rows are space
    std::vector<int> separators;
    for (int c = 0; c < cols; ++c) {
        bool is_sep = true;
        for (int r = 0; r < rows; ++r) {
            if (lines[r][c] != ' ') {
                is_sep = false;
                break;
            }
        }
        if (is_sep)
            separators.push_back(c);
    }

    // Add sentinel boundaries so we include segments at the edges
    std::vector<int> bounds;
    bounds.push_back(-1);
    for (int s : separators)
        bounds.push_back(s);
    bounds.push_back(cols);

    long long total = 0;
    for (size_t i = 0; i + 1 < bounds.size(); ++i) {
        int start = bounds[i] + 1;
        int end = bounds[i + 1] - 1;

        // Find op_col: the column in start-end where lines[rows-1][c] != ' '
        int op_col = -1;
        char op = ' ';
        for (int c = start; c <= end; ++c) {
            if (lines[rows - 1][c] != ' ') {
                op_col = c;
                op = lines[rows - 1][c];
                break;
            }
        }

        // Numbers from all columns, read right-to-left within the problem
        std::vector<long long> nums;
        for (int c = end; c >= start; --c) {
            std::string num_str;
            for (int r = 0; r < rows - 1; ++r) {
                num_str += lines[r][c];
            }
            // Trim leading/trailing spaces
            num_str.erase(num_str.begin(), std::find_if(num_str.begin(), num_str.end(), [](char ch) { return ch != ' '; }));
            num_str.erase(std::find_if(num_str.rbegin(), num_str.rend(), [](char ch) { return ch != ' '; }).base(), num_str.end());
            if (!num_str.empty()) {
                nums.push_back(std::stoll(num_str));
            }
        }

        // Compute result
        long long res = nums.empty() ? 0 : nums[0];
        for (size_t j = 1; j < nums.size(); ++j) {
            if (op == '+')
                res += nums[j];
            else if (op == '*')
                res *= nums[j];
        }
        total += res;
    }

    std::cout << total << std::endl;
    return 0;
}

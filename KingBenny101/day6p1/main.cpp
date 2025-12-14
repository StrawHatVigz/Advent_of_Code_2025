#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string& s)
{
    std::vector<std::string> res;
    std::stringstream ss(s);
    std::string token;
    while (ss >> token) {
        res.push_back(token);
    }
    return res;
}

int main()
{
    std::ifstream inputFile("inputs/input.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open inputs/input.txt" << std::endl;
        return 1;
    }

    std::vector<std::vector<std::string>> data;
    std::string line;
    while (std::getline(inputFile, line)) {
        data.push_back(split(line));
    }
    inputFile.close();

    if (data.empty()) {
        std::cout << 0 << std::endl;
        return 0;
    }

    size_t num_problems = data[0].size();
    long long total = 0;
    for (size_t p = 0; p < num_problems; ++p) {
        char op = data.back()[p][0];
        std::vector<long long> nums;
        for (size_t r = 0; r < data.size() - 1; ++r) {
            nums.push_back(std::stoll(data[r][p]));
        }
        long long res = nums[0];
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

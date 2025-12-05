#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream inputFile("inputs/input.txt");
    
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open inputs/input.txt" << std::endl;
        return 1;
    }
    
    std::string line;
    while (std::getline(inputFile, line)) {
        std::cout << line << std::endl;
    }
    
    inputFile.close();
    return 0;
}

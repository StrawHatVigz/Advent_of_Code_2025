#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using ll = long long;

ll dfs2(const std::string& u, int mask, const std::string& target, std::unordered_map<std::string, std::vector<std::string>>& adj, std::unordered_map<std::string, std::array<ll, 4>>& memo, std::unordered_set<std::string>& vis)
{
    if (u == target)
        return (mask == 3) ? 1 : 0;
    if (vis.count(u))
        return 0; // avoid cycles
    auto mit = memo.find(u);
    if (mit != memo.end() && mit->second[mask] != -1)
        return mit->second[mask];
    vis.insert(u);
    int newmask = mask;
    if (u == "dac")
        newmask |= 1;
    if (u == "fft")
        newmask |= 2;
    ll total = 0;
    auto it = adj.find(u);
    if (it != adj.end()) {
        for (auto& v : it->second)
            total += dfs2(v, newmask, target, adj, memo, vis);
    }
    vis.erase(u);
    if (mit == memo.end())
        memo[u] = { -1, -1, -1, -1 };
    memo[u][mask] = total;
    return total;
}

int main(int argc, char** argv)
{
    std::string path = "inputs/input.txt";
    if (argc > 1)
        path = argv[1];
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open " << path << std::endl;
        return 1;
    }

    std::unordered_map<std::string, std::vector<std::string>> adj;
    std::string line;
    while (std::getline(inputFile, line)) {
        if (line.empty())
            continue;
        auto colon = line.find(':');
        if (colon == std::string::npos)
            continue;
        std::string name = line.substr(0, colon);
        std::string rest = line.substr(colon + 1);
        std::stringstream ss(rest);
        std::string tok;
        while (ss >> tok)
            adj[name].push_back(tok);
    }
    inputFile.close();

    std::unordered_map<std::string, std::array<ll, 4>> memo;
    std::unordered_set<std::string> vis;
    ll result = dfs2("svr", 0, "out", adj, memo, vis);
    std::cout << result << std::endl;
    return 0;
}

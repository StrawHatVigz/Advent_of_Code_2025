#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct DSU {
    std::vector<int> p, r, sz;
    DSU(int n = 0)
        : p(n)
        , r(n)
        , sz(n, 1)
    {
        for (int i = 0; i < n; ++i)
            p[i] = i;
    }
    int find(int x) { return p[x] == x ? x : p[x] = find(p[x]); }
    bool unite(int a, int b)
    {
        a = find(a);
        b = find(b);
        if (a == b)
            return false;
        if (r[a] < r[b])
            std::swap(a, b);
        p[b] = a;
        sz[a] += sz[b];
        if (r[a] == r[b])
            ++r[a];
        return true;
    }
    int size(int x) { return sz[find(x)]; }
};

int main(int argc, char** argv)
{
    std::string path = "inputs/input.txt";
    if (argc > 1)
        path = argv[1];
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        if (path == "inputs/input.txt")
            inputFile.open("inputs/test.txt");
        if (!inputFile.is_open()) {
            std::cerr << "Error: Could not open input file" << std::endl;
            return 1;
        }
    }

    std::vector<std::array<long long, 3>> pts;
    std::string line;
    while (std::getline(inputFile, line)) {
        if (line.empty())
            continue;
        long long x, y, z;
        char c;
        std::stringstream ss(line);
        ss >> x >> c >> y >> c >> z;
        pts.push_back({ x, y, z });
    }
    inputFile.close();

    int n = (int)pts.size();
    if (n == 0) {
        std::cout << 0 << std::endl;
        return 0;
    }

    // collect all pairwise edges with squared distances
    std::vector<std::pair<unsigned long long, std::pair<int, int>>> edges;
    edges.reserve((size_t)n * (n - 1) / 2);
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            long long dx = pts[i][0] - pts[j][0];
            long long dy = pts[i][1] - pts[j][1];
            long long dz = pts[i][2] - pts[j][2];
            unsigned long long d2 = (unsigned long long)(dx * dx) + (unsigned long long)(dy * dy) + (unsigned long long)(dz * dz);
            edges.push_back({ d2, { i, j } });
        }
    }
    std::sort(edges.begin(), edges.end(), [](auto& a, auto& b) { if (a.first != b.first) return a.first < b.first; return a.second < b.second; });

    DSU dsu(n);
    int components = n;
    unsigned long long last_product = 0;

    for (auto& e : edges) {
        int a = e.second.first;
        int b = e.second.second;
        if (dsu.unite(a, b)) {
            --components;
            if (components == 1) {
                // product of x-coordinates of these two points
                last_product = (unsigned long long)pts[a][0] * (unsigned long long)pts[b][0];
                break;
            }
        }
    }

    std::cout << last_product << std::endl;
    return 0;
}

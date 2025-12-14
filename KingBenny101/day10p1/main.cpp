#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using u64 = unsigned long long;

int main(int argc, char** argv)
{
    std::string path = "inputs/input.txt";
    if (argc > 1)
        path = argv[1];
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open inputs/input.txt" << std::endl;
        return 1;
    }

    std::string line;
    long long total = 0;
    while (std::getline(inputFile, line)) {
        if (line.empty())
            continue;
        // parse pattern
        auto lb = line.find('[');
        auto rb = line.find(']');
        if (lb == std::string::npos || rb == std::string::npos)
            continue;
        std::string pattern = line.substr(lb + 1, rb - lb - 1);
        int m = (int)pattern.size();
        u64 target = 0;
        for (int i = 0; i < m; ++i)
            if (pattern[i] == '#')
                target |= (1ULL << i);

        // parse buttons (parentheses groups before the first '{')
        std::vector<u64> buttons;
        size_t pos = rb + 1;
        while (true) {
            auto lp = line.find('(', pos);
            auto rp = line.find(')', pos);
            if (lp == std::string::npos || rp == std::string::npos || rp < lp)
                break;
            std::string inside = line.substr(lp + 1, rp - lp - 1);
            std::stringstream ss(inside);
            u64 mask = 0;
            int idx;
            while (ss >> idx) {
                mask |= (1ULL << idx);
                if (ss.peek() == ',')
                    ss.ignore();
            }
            buttons.push_back(mask);
            pos = rp + 1;
        }

        int k = (int)buttons.size();
        if (k == 0) {
            // no buttons: must be already configured
            total += (__builtin_popcountll(target) == 0) ? 0 : 0; // if impossible, count 0
            continue;
        }

        // Build matrix A (m rows) and rhs b
        std::vector<u64> A;
        A.reserve(m);
        std::vector<int> b;
        b.reserve(m);
        for (int row = 0; row < m; ++row) {
            u64 mask = 0;
            for (int j = 0; j < k; ++j)
                if ((buttons[j] >> row) & 1ULL)
                    mask |= (1ULL << j);
            A.push_back(mask);
            b.push_back(((target >> row) & 1ULL) ? 1 : 0);
        }

        // Gaussian elimination over GF(2)
        int nrows = m, ncols = k;
        std::vector<int> where(ncols, -1);
        int row = 0;
        for (int col = 0; col < ncols && row < nrows; ++col) {
            int sel = row;
            while (sel < nrows && (((A[sel] >> col) & 1ULL) == 0))
                ++sel;
            if (sel == nrows)
                continue;
            std::swap(A[sel], A[row]);
            std::swap(b[sel], b[row]);
            where[col] = row;
            for (int i = 0; i < nrows; ++i) {
                if (i != row && ((A[i] >> col) & 1ULL)) {
                    A[i] ^= A[row];
                    b[i] ^= b[row];
                }
            }
            ++row;
        }

        // check consistency
        bool ok = true;
        for (int i = 0; i < nrows; ++i)
            if (A[i] == 0 && b[i])
                ok = false;
        if (!ok)
            continue; // skip impossible

        // particular solution with free vars = 0
        u64 x0 = 0;
        for (int j = 0; j < ncols; ++j)
            if (where[j] != -1) {
                if (b[where[j]])
                    x0 |= (1ULL << j);
            }

        // build nullspace basis vectors for free variables
        std::vector<u64> basis;
        for (int j = 0; j < ncols; ++j)
            if (where[j] == -1) {
                u64 vec = (1ULL << j);
                for (int c = 0; c < ncols; ++c)
                    if (where[c] != -1) {
                        int r = where[c];
                        if ((A[r] >> j) & 1ULL)
                            vec |= (1ULL << c);
                    }
                basis.push_back(vec);
            }

        int f = (int)basis.size();
        int best = __builtin_popcountll(x0);
        if (f <= 28) {
            // brute force through nullspace
            int combos = 1 << f;
            for (int mask = 1; mask < combos; ++mask) {
                u64 x = x0;
                int mm = mask;
                int idx = 0;
                while (mm) {
                    if (mm & 1)
                        x ^= basis[idx];
                    mm >>= 1;
                    ++idx;
                }
                best = std::min(best, __builtin_popcountll(x));
            }
        } else {
            // if too many free vars, use greedy local search (should be rare)
            u64 x = x0;
            bool improved = true;
            while (improved) {
                improved = false;
                for (auto& v : basis) {
                    int cur = __builtin_popcountll(x);
                    int nxt = __builtin_popcountll(x ^ v);
                    if (nxt < cur) {
                        x ^= v;
                        improved = true;
                    }
                }
            }
            best = std::min(best, __builtin_popcountll(x));
        }

        (void)best;
        total += best;
    }

    inputFile.close();
    std::cout << total << std::endl;
    return 0;
}

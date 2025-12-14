#include <climits>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

using i64 = long long;

struct Fr {
    i64 n, d; // n/d
    Fr(i64 _n = 0, i64 _d = 1)
    {
        n = _n;
        d = _d;
        norm();
    }
    void norm()
    {
        if (d < 0) {
            n = -n;
            d = -d;
        }
        if (n == 0) {
            d = 1;
            return;
        }
        i64 g = std::gcd(std::llabs(n), d);
        n /= g;
        d /= g;
    }
    Fr operator+(const Fr& o) const { return Fr(n * o.d + o.n * d, d * o.d); }
    Fr operator-(const Fr& o) const { return Fr(n * o.d - o.n * d, d * o.d); }
    Fr operator*(const Fr& o) const { return Fr(n * o.n, d * o.d); }
    Fr operator/(const Fr& o) const { return Fr(n * o.d, d * o.n); }
    bool isZero() const { return n == 0; }
};

static bool frac_is_int(const Fr& f, i64& out)
{
    if (f.d == 1) {
        out = f.n;
        return true;
    }
    return false;
}

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

    auto parse_list = [&](const std::string& s) -> std::vector<int> {
        std::vector<int> res;
        std::stringstream ss(s);
        int v;
        while (ss >> v) {
            res.push_back(v);
            if (ss.peek() == ',')
                ss.ignore();
        }
        return res;
    };

    std::string line;
    long long total = 0;
    while (std::getline(inputFile, line)) {
        if (line.empty())
            continue;
        // parse pattern (ignore)
        auto lb = line.find('{');
        auto rb = line.find('}');
        if (lb == std::string::npos || rb == std::string::npos)
            continue;
        std::string rhs = line.substr(lb + 1, rb - lb - 1);
        std::vector<int> bvec = parse_list(rhs);
        int m = (int)bvec.size();

        // parse buttons
        std::vector<std::vector<int>> btns;
        size_t pos = 0;
        while (true) {
            auto lp = line.find('(', pos);
            auto rp = line.find(')', pos);
            if (lp == std::string::npos || rp == std::string::npos || rp < lp)
                break;
            std::string inside = line.substr(lp + 1, rp - lp - 1);
            std::vector<int> v = parse_list(inside);
            btns.push_back(v);
            pos = rp + 1;
        }

        int k = (int)btns.size();
        if (k == 0)
            continue;

        // Build matrix A (m x k) and rhs b
        std::vector<std::vector<Fr>> A(m, std::vector<Fr>(k));
        std::vector<Fr> B(m);
        for (int i = 0; i < m; ++i)
            B[i] = Fr(bvec[i]);
        for (int j = 0; j < k; ++j)
            for (int i = 0; i < m; ++i)
                A[i][j] = Fr(0);
        for (int j = 0; j < k; ++j)
            for (int idx : btns[j])
                if (idx >= 0 && idx < m)
                    A[idx][j] = Fr(1);

        // Gaussian elimination to RREF
        std::vector<int> where(k, -1);
        int row = 0;
        for (int col = 0; col < k && row < m; ++col) {
            int sel = -1;
            for (int i = row; i < m; ++i)
                if (!A[i][col].isZero()) {
                    sel = i;
                    break;
                }
            if (sel == -1)
                continue;
            std::swap(A[sel], A[row]);
            std::swap(B[sel], B[row]);
            Fr pivot = A[row][col];
            // normalize row
            for (int j = col; j < k; ++j)
                A[row][j] = A[row][j] / pivot;
            B[row] = B[row] / pivot;
            where[col] = row;
            // eliminate other rows
            for (int i = 0; i < m; ++i)
                if (i != row && !A[i][col].isZero()) {
                    Fr factor = A[i][col];
                    for (int j = col; j < k; ++j)
                        A[i][j] = A[i][j] - A[row][j] * factor;
                    B[i] = B[i] - B[row] * factor;
                }
            ++row;
        }

        // check consistency
        bool ok = true;
        for (int i = 0; i < m; ++i) {
            bool allz = true;
            for (int j = 0; j < k; ++j)
                if (!A[i][j].isZero()) {
                    allz = false;
                    break;
                }
            if (allz && !B[i].isZero()) {
                ok = false;
                break;
            }
        }
        if (!ok)
            continue;

        // collect free vars
        std::vector<int> free_idx;
        for (int j = 0; j < k; ++j)
            if (where[j] == -1)
                free_idx.push_back(j);
        int f = (int)free_idx.size();

        // upper bounds for each variable: ub_j = min_{i in btn j} b_i
        std::vector<int> ub(k, INT_MAX);
        for (int j = 0; j < k; ++j) {
            int ubj = INT_MAX;
            if (btns[j].empty())
                ubj = 0;
            else {
                for (int idx : btns[j])
                    ubj = std::min(ubj, bvec[idx]);
            }
            ub[j] = ubj == INT_MAX ? 0 : ubj;
        }

        i64 best = LLONG_MAX;

        // recursive enumeration over free vars with pruning
        std::vector<int> assign(f, 0);
        std::function<void(int, i64)> dfs = [&](int p, i64 cursum) {
            if (cursum >= best)
                return;
            if (p == f) {
                // compute pivot vars
                bool good = true;
                i64 total_sum = cursum;
                // build map of free assignments
                std::vector<i64> x(k, 0);
                for (int i = 0; i < f; ++i)
                    x[free_idx[i]] = assign[i];
                for (int j = 0; j < k; ++j)
                    if (where[j] != -1) {
                        int r = where[j];
                        Fr val = B[r];
                        for (int t = 0; t < f; ++t) {
                            int fj = free_idx[t];
                            if (!A[r][fj].isZero())
                                val = val - A[r][fj] * Fr(assign[t]);
                        }
                        i64 xv;
                        if (!frac_is_int(val, xv) || xv < 0) {
                            good = false;
                            break;
                        }
                        x[j] = xv;
                        total_sum += xv;
                        if (total_sum >= best) {
                            good = false;
                            break;
                        }
                    }
                if (good)
                    best = std::min(best, total_sum);
                return;
            }
            int idx = free_idx[p];
            int upper = ub[idx];
            if (upper < 0)
                upper = 0;
            // try 0..upper
            for (int v = 0; v <= upper; ++v) {
                assign[p] = v;
                dfs(p + 1, cursum + v);
            }
            assign[p] = 0;
        };

        // If no free variables, compute unique solution
        if (f == 0) {
            std::vector<i64> x(k, 0);
            bool good = true;
            i64 s = 0;
            for (int j = 0; j < k; ++j) {
                if (where[j] != -1) {
                    int r = where[j];
                    i64 xv;
                    if (!frac_is_int(B[r], xv) || xv < 0) {
                        good = false;
                        break;
                    }
                    x[j] = xv;
                    s += xv;
                }
            }
            if (good)
                best = s;
        } else {
            dfs(0, 0);
        }

        if (best < LLONG_MAX)
            total += best;
    }

    inputFile.close();
    std::cout << total << std::endl;
    return 0;
}

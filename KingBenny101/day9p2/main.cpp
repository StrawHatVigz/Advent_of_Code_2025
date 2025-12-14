#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using ll = long long;
using pii = std::pair<int, int>;

bool onSegment(pii p, pii q, pii r)
{
    if (q.first <= std::max(p.first, r.first) && q.first >= std::min(p.first, r.first) && q.second <= std::max(p.second, r.second) && q.second >= std::min(p.second, r.second))
        return true;
    return false;
}

int orientation(pii p, pii q, pii r)
{
    ll val = (ll)(q.second - p.second) * (r.first - q.first) - (ll)(q.first - p.first) * (r.second - q.second);
    if (val == 0)
        return 0;
    return (val > 0) ? 1 : 2;
}

bool doIntersect(pii p1, pii q1, pii p2, pii q2)
{
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    if (o1 != o2 && o3 != o4)
        return true;

    if (o1 == 0 && onSegment(p1, p2, q1))
        return true;
    if (o2 == 0 && onSegment(p1, q2, q1))
        return true;
    if (o3 == 0 && onSegment(p2, p1, q2))
        return true;
    if (o4 == 0 && onSegment(p2, q1, q2))
        return true;

    return false;
}

bool pointInPolygon(const std::vector<pii>& poly, int x, int y)
{
    int n = poly.size();
    bool inside = false;
    pii p = { x, y };
    for (int i = 0; i < n; ++i) {
        pii a = poly[i], b = poly[(i + 1) % n];
        if (onSegment(a, p, b))
            return true; // on boundary
        if (a.second != b.second) { // not horizontal
            if (((a.second > y) != (b.second > y)) && (x < a.first + (b.first - a.first) * (y - a.second) / (b.second - a.second + 1e-9))) {
                inside = !inside;
            }
        }
        // check vertical edges
        if (a.first == b.first && a.first > x && std::min(a.second, b.second) < y && std::max(a.second, b.second) > y) {
            inside = !inside;
        }
    }
    return inside;
}

int main(int argc, char** argv)
{
    std::string path = "inputs/input.txt";
    if (argc > 1)
        path = argv[1];
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        inputFile.open("inputs/test.txt");
        if (!inputFile.is_open()) {
            std::cerr << "Error: Could not open input file" << std::endl;
            return 1;
        }
    }

    std::vector<pii> reds;
    std::string line;
    while (std::getline(inputFile, line)) {
        if (line.empty())
            continue;
        int x, y;
        char c;
        std::stringstream ss(line);
        ss >> x >> c >> y;
        reds.emplace_back(x, y);
    }
    inputFile.close();

    if (reds.empty()) {
        std::cout << 0 << std::endl;
        return 0;
    }

    // Coordinate-compressed grid to avoid huge memory usage
    std::vector<int> xs, ys;
    int minx = reds[0].first, maxx = reds[0].first, miny = reds[0].second, maxy = reds[0].second;
    for (auto& p : reds) {
        minx = std::min(minx, p.first);
        maxx = std::max(maxx, p.first);
        miny = std::min(miny, p.second);
        maxy = std::max(maxy, p.second);
    }
    xs.push_back(minx - 1);
    ys.push_back(miny - 1);
    for (auto& p : reds) {
        xs.push_back(p.first);
        xs.push_back(p.first + 1);
        ys.push_back(p.second);
        ys.push_back(p.second + 1);
    }
    xs.push_back(maxx + 2);
    ys.push_back(maxy + 2);
    std::sort(xs.begin(), xs.end());
    xs.erase(std::unique(xs.begin(), xs.end()), xs.end());
    std::sort(ys.begin(), ys.end());
    ys.erase(std::unique(ys.begin(), ys.end()), ys.end());

    int W = (int)xs.size() - 1;
    int H = (int)ys.size() - 1;
    auto idxX = [&](int x) {
        int it = std::lower_bound(xs.begin(), xs.end(), x) - xs.begin();
        if (it == (int)xs.size() || xs[it] != x)
            --it;
        return it;
    };
    auto idxY = [&](int y) {
        int it = std::lower_bound(ys.begin(), ys.end(), y) - ys.begin();
        if (it == (int)ys.size() || ys[it] != y)
            --it;
        return it;
    };

    std::vector<std::vector<char>> grid(H, std::vector<char>(W, '.'));
    // mark red points
    for (auto& p : reds) {
        int gx = idxX(p.first);
        int gy = idxY(p.second);
        grid[gy][gx] = 'R';
    }

    // draw green edges between consecutive reds
    int n = reds.size();
    for (int i = 0; i < n; ++i) {
        auto a = reds[i];
        auto b = reds[(i + 1) % n];
        if (a.first == b.first) {
            int gx = idxX(a.first);
            int y1 = std::min(a.second, b.second);
            int y2 = std::max(a.second, b.second);
            int gy1 = idxY(y1);
            int gy2 = idxY(y2);
            for (int gy = gy1; gy <= gy2; ++gy)
                if (grid[gy][gx] == '.')
                    grid[gy][gx] = 'G';
        } else if (a.second == b.second) {
            int gy = idxY(a.second);
            int x1 = std::min(a.first, b.first);
            int x2 = std::max(a.first, b.first);
            int gx1 = idxX(x1);
            int gx2 = idxX(x2);
            for (int gx = gx1; gx <= gx2; ++gx)
                if (grid[gy][gx] == '.')
                    grid[gy][gx] = 'G';
        }
    }

    // flood fill from outside (0,0) cell which is guaranteed outside due to padding
    std::vector<std::vector<int>> vis(H, std::vector<int>(W, 0));
    std::vector<std::pair<int, int>> q;
    q.emplace_back(0, 0);
    vis[0][0] = 1;
    for (size_t qi = 0; qi < q.size(); ++qi) {
        auto [y, x] = q[qi];
        static int dy[4] = { 1, -1, 0, 0 };
        static int dx[4] = { 0, 0, 1, -1 };
        for (int d = 0; d < 4; ++d) {
            int ny = y + dy[d], nx = x + dx[d];
            if (ny < 0 || ny >= H || nx < 0 || nx >= W)
                continue;
            if (vis[ny][nx])
                continue;
            if (grid[ny][nx] == '.') {
                vis[ny][nx] = 1;
                q.emplace_back(ny, nx);
            }
        }
    }
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            if (grid[y][x] == '.' && !vis[y][x])
                grid[y][x] = 'G';

    // Build prefix sum of good cells (red or green) weighted by original area
    std::vector<std::vector<ll>> pref(H + 1, std::vector<ll>(W + 1, 0));
    for (int y = 0; y < H; ++y) {
        ll height = ys[y + 1] - ys[y];
        for (int x = 0; x < W; ++x) {
            ll width = xs[x + 1] - xs[x];
            ll good = (grid[y][x] == 'R' || grid[y][x] == 'G') ? width * height : 0;
            pref[y + 1][x + 1] = pref[y + 1][x] + pref[y][x + 1] - pref[y][x] + good;
        }
    }

    // For all pairs of red points, check rectangle area using compressed indices
    ll best = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            auto a = reds[i], b = reds[j];
            int x1 = std::min(a.first, b.first), x2 = std::max(a.first, b.first);
            int y1 = std::min(a.second, b.second), y2 = std::max(a.second, b.second);
            ll area = (ll)(x2 - x1 + 1) * (y2 - y1 + 1);
            int lx = idxX(x1), rx = idxX(x2);
            int ly = idxY(y1), ry = idxY(y2);
            ll sum = pref[ry + 1][rx + 1] - pref[ly][rx + 1] - pref[ry + 1][lx] + pref[ly][lx];
            if (sum == area)
                best = std::max(best, area);
        }
    }

    std::cout << best << std::endl;
    return 0;
}

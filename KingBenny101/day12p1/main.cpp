#include <algorithm>
#include <cctype>
#include <chrono>
#include <climits>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

using vi = std::vector<int>;

// exact cover backtracking with primary columns (shape instances) and secondary columns (grid cells)
bool solve_exact_cover(int P, int S, const std::vector<vi>& rows)
{
    int C = P + S;
    int R = rows.size();
    std::vector<std::vector<int>> rowsOfCol(C);
    for (int r = 0; r < R; ++r)
        for (int c : rows[r])
            rowsOfCol[c].push_back(r);
    std::vector<char> rowActive(R, 1);
    std::vector<char> colCovered(P, 0);
    int covered = 0;

    std::function<bool()> dfs = [&]() -> bool {
        if (covered == P)
            return true;
        // choose uncovered primary column with fewest active rows
        int choose = -1;
        int best = INT_MAX;
        for (int c = 0; c < P; ++c)
            if (!colCovered[c]) {
                int cnt = 0;
                for (int r : rowsOfCol[c])
                    if (rowActive[r])
                        ++cnt;
                if (cnt == 0)
                    return false;
                if (cnt < best) {
                    best = cnt;
                    choose = c;
                }
            }
        // try rows
        auto saveRowActive = rowActive;
        auto saveColCovered = colCovered;
        int saveCovered = covered;
        for (int r : rowsOfCol[choose])
            if (rowActive[r]) {
                // cover this row: disable all rows that intersect its columns
                std::vector<int> disabled;
                for (int c : rows[r]) {
                    // mark primary covered
                    if (c < P && !colCovered[c]) {
                        colCovered[c] = 1;
                        ++covered;
                    }
                    for (int rr : rowsOfCol[c])
                        if (rowActive[rr]) {
                            rowActive[rr] = 0;
                            disabled.push_back(rr);
                        }
                }
                // recurse
                if (dfs())
                    return true;
                // undo
                for (int rr : disabled)
                    rowActive[rr] = 1;
                rowActive[r] = 1;
                colCovered = saveColCovered;
                covered = saveCovered;
            }
        return false;
    };

    return dfs();
}

// bitmask-based backtracking per shape (placements per shape, occupancy bitset, memoization)
using Block = uint64_t;
bool solve_by_placements(int W, int H, const std::map<int, std::vector<std::string>>& shapes, std::vector<int> counts, double timeLimitSeconds = 10.0)
{
    int numShapes = 0;
    for (auto& p : shapes)
        numShapes = std::max(numShapes, p.first + 1);
    int cells = W * H;
    int B = (cells + 63) / 64;

    std::vector<std::vector<std::vector<Block>>> placements(numShapes);
    std::vector<int> shapeArea(numShapes, 0);
    for (int s = 0; s < numShapes; ++s) {
        auto it = shapes.find(s);
        if (it == shapes.end())
            continue;
        auto shapeLines = it->second;
        std::vector<std::pair<int, int>> cellsVec;
        for (int y = 0; y < (int)shapeLines.size(); ++y)
            for (int x = 0; x < (int)shapeLines[y].size(); ++x)
                if (shapeLines[y][x] == '#')
                    cellsVec.emplace_back(x, y);
        shapeArea[s] = cellsVec.size();
        if (cellsVec.empty())
            continue;
        std::set<std::vector<std::pair<int, int>>> oris;
        for (int flip = 0; flip < 2; ++flip)
            for (int rot = 0; rot < 4; ++rot) {
                std::vector<std::pair<int, int>> nc;
                for (auto pr : cellsVec) {
                    int x = pr.first, y = pr.second;
                    int xx = flip ? -x : x;
                    int yy = y;
                    for (int r = 0; r < rot; ++r) {
                        int tx = xx;
                        xx = -yy;
                        yy = tx;
                    }
                    nc.emplace_back(xx, yy);
                }
                int minx = INT_MAX, miny = INT_MAX;
                for (auto& p : nc) {
                    minx = std::min(minx, p.first);
                    miny = std::min(miny, p.second);
                }
                for (auto& p : nc) {
                    p.first -= minx;
                    p.second -= miny;
                }
                std::sort(nc.begin(), nc.end());
                oris.insert(nc);
            }
        for (auto& ori : oris) {
            int maxx = 0, maxy = 0;
            for (auto& p : ori) {
                maxx = std::max(maxx, p.first);
                maxy = std::max(maxy, p.second);
            }
            for (int oy = 0; oy + maxy < H; ++oy)
                for (int ox = 0; ox + maxx < W; ++ox) {
                    std::vector<Block> mask(B, 0);
                    for (auto& p : ori) {
                        int cx = ox + p.first, cy = oy + p.second;
                        int idx = cy * W + cx;
                        int bi = idx / 64;
                        int off = idx % 64;
                        mask[bi] |= (Block(1) << off);
                    }
                    placements[s].push_back(mask);
                }
        }
    }

    long long totalNeeded = 0;
    for (int s = 0; s < numShapes; ++s)
        totalNeeded += 1LL * counts[s] * shapeArea[s];
    if (totalNeeded > cells)
        return false;

    std::vector<Block> occ(B, 0);
    auto now = std::chrono::steady_clock::now;
    auto start = now();
    std::unordered_set<uint64_t> visited;
    auto occ_hash = [&](const std::vector<Block>& v) -> uint64_t { uint64_t h = 14695981039346656037ULL; for (Block b : v) h = (h ^ b) * 1099511628211ULL; return h; };

    std::function<bool()> dfs = [&]() -> bool {
        if (std::chrono::duration<double>(now() - start).count() > timeLimitSeconds)
            return false;
        bool done = true;
        for (int s = 0; s < numShapes; ++s)
            if (counts[s] > 0) {
                done = false;
                break;
            }
        if (done)
            return true;
        int freeCells = 0;
        for (int bi = 0; bi < B; ++bi)
            freeCells += __builtin_popcountll(~occ[bi]);
        long long remainArea = 0;
        for (int s = 0; s < numShapes; ++s)
            remainArea += 1LL * counts[s] * shapeArea[s];
        if (remainArea > freeCells)
            return false;
        int bestS = -1;
        int bestCount = INT_MAX;
        std::vector<int> feasibleList;
        for (int s = 0; s < numShapes; ++s)
            if (counts[s] > 0) {
                std::vector<int> feas;
                feas.reserve(128);
                for (int pi = 0; pi < (int)placements[s].size(); ++pi) {
                    auto& mask = placements[s][pi];
                    bool ok = true;
                    for (int bi = 0; bi < B; ++bi)
                        if ((mask[bi] & occ[bi]) != 0) {
                            ok = false;
                            break;
                        }
                    if (ok)
                        feas.push_back(pi);
                }
                if ((int)feas.size() < counts[s])
                    return false;
                int metric = (int)feas.size();
                if (metric < bestCount) {
                    bestCount = metric;
                    bestS = s;
                    feasibleList = std::move(feas);
                }
            }
        if (bestS == -1)
            return false;
        uint64_t key = occ_hash(occ);
        for (int s = 0; s < numShapes; ++s)
            key = key * 1315423911u + (uint64_t)(counts[s] + 1);
        if (visited.count(key))
            return false;
        for (int pi : feasibleList) {
            auto& mask = placements[bestS][pi];
            for (int bi = 0; bi < B; ++bi)
                occ[bi] |= mask[bi];
            counts[bestS]--;
            if (dfs())
                return true;
            counts[bestS]++;
            for (int bi = 0; bi < B; ++bi)
                occ[bi] &= ~mask[bi];
        }
        visited.insert(key);
        return false;
    };

    return dfs();
}
// improved solver: placements per shape with bitmask backtracking and memoization
bool solve_small_instance(int W, int H, const std::map<int, std::vector<std::string>>& shapes, std::vector<int> counts)
{
    // use the bitmask placements solver with a moderate per-region time limit
    return solve_by_placements(W, H, shapes, counts, 5.0);
}

int main(int argc, char** argv)
{
    std::string path = "inputs/input.txt";
    if (argc > 1)
        path = argv[1];
    std::ifstream inputFile(path);
    std::cerr << "DEBUG: opening file '" << path << "'" << std::endl;
    if (!inputFile.is_open()) {
        std::cerr << "Error opening " << path << std::endl;
        return 1;
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(inputFile, line))
        lines.push_back(line);
    inputFile.close();
    std::cerr << "DEBUG: read lines=" << lines.size() << std::endl;

    // parse shapes
    int i = 0;
    std::map<int, std::vector<std::string>> shapes;
    // parsed input lines
    while (i < (int)lines.size()) {
        if (lines[i].empty()) {
            ++i;
            continue;
        }
        // region lines start like "WxH:"
        bool isRegion = false;
        for (char ch : lines[i])
            if (ch == 'x') {
                isRegion = true;
                break;
            }
        if (isRegion)
            break;
        // header like "0:"
        int idx = -1;
        {
            std::stringstream ss(lines[i]);
            ss >> idx; // ignore colon
        }
        ++i;
        std::vector<std::string> shapeLines;
        while (i < (int)lines.size() && !lines[i].empty() && (lines[i][0] == '.' || lines[i][0] == '#')) {
            shapeLines.push_back(lines[i]);
            ++i;
        }
        shapes[idx] = shapeLines;
    }
    std::cerr << "DEBUG: parsed " << shapes.size() << " shapes" << std::endl;

    // parse regions
    // parse regions
    int answer = 0;
    for (; i < (int)lines.size(); ++i) {
        // processing region line
        if (lines[i].empty())
            continue;
        std::string L = lines[i];
        std::stringstream ss(L);
        int W, H;
        char xch, colon;
        ss >> W >> xch >> H >> colon;
        std::vector<int> counts;
        int v;
        while (ss >> v)
            counts.push_back(v);
        // build placements
        std::cerr << "DEBUG: region W=" << W << " H=" << H << " counts:";
        for (int cc : counts)
            std::cerr << cc << ",";
        std::cerr << std::endl;
        // map shapes indices to list and order shapes by idx
        int numShapes = 0;
        for (auto& p : shapes)
            numShapes = std::max(numShapes, p.first + 1);
        // compute list of shape indices and counts; if counts shorter than numShapes, pad
        counts.resize(numShapes);

        // quick area check
        int totalCellsNeeded = 0;
        for (int s = 0; s < numShapes; ++s) {
            auto it = shapes.find(s);
            if (it == shapes.end())
                continue;
            int cellsCount = 0;
            for (auto& row : it->second)
                for (char ch : row)
                    if (ch == '#')
                        ++cellsCount;
            totalCellsNeeded += cellsCount * counts[s];
        }
        if (totalCellsNeeded > W * H) { /* impossible */
            continue;
        }

        // if this is a small test file or tiny region, try quick DFS tiling first
        bool small_ok = false;
        if (W * H <= 60) {
            small_ok = solve_small_instance(W, H, shapes, counts);
            if (!small_ok) {
                continue;
            }
        }

        // For larger regions, prefer the bitmask-based placements solver with
        // an adaptive per-region time limit to avoid pathological long runs.
        if (W * H > 60) {
            double tlim = 10.0;
            if (W * H > 2000)
                tlim = 60.0;
            else if (W * H > 1000)
                tlim = 30.0;
            else if (W * H > 500)
                tlim = 20.0;
            bool bit_ok = solve_by_placements(W, H, shapes, counts, tlim);
            if (bit_ok) {
                ++answer;
                continue;
            }
            // if the bitmask solver timed out/failed, fall back to exact cover
            // only for moderate regions to avoid expensive/huge enumeration.
            if (W * H > 2000) {
                // Skip heavy exact-cover attempt for very large regions
                continue;
            }
        }

        // collect all placements rows
        int P = 0;
        std::vector<int> baseIdx(numShapes, 0);
        for (int s = 0; s < numShapes; ++s) {
            baseIdx[s] = P;
            P += counts[s];
        }
        int S = W * H;
        std::vector<vi> rows;
        rows.reserve(10000);

        for (int s = 0; s < numShapes; ++s) {
            if (counts[s] == 0)
                continue;
            auto it = shapes.find(s);
            if (it == shapes.end())
                continue;
            auto shapeLines = it->second;
            // compute coords of '#' cells
            std::vector<std::pair<int, int>> cells;
            for (int y = 0; y < (int)shapeLines.size(); ++y)
                for (int x = 0; x < (int)shapeLines[y].size(); ++x)
                    if (shapeLines[y][x] == '#')
                        cells.emplace_back(x, y);
            if (cells.empty())
                continue;
            // generate orientations
            std::set<std::vector<std::pair<int, int>>> orients;
            for (int flip = 0; flip < 2; ++flip)
                for (int rot = 0; rot < 4; ++rot) {
                    std::vector<std::pair<int, int>> nc;
                    for (auto pr : cells) {
                        int x = pr.first, y = pr.second;
                        int xx = flip ? -x : x;
                        int yy = y;
                        for (int r = 0; r < rot; ++r) {
                            int tx = xx;
                            xx = -yy;
                            yy = tx;
                        }
                        nc.emplace_back(xx, yy);
                    }
                    int minx = INT_MAX, miny = INT_MAX;
                    for (auto& p : nc) {
                        minx = std::min(minx, p.first);
                        miny = std::min(miny, p.second);
                    }
                    for (auto& p : nc) {
                        p.first -= minx;
                        p.second -= miny;
                    }
                    std::sort(nc.begin(), nc.end());
                    orients.insert(nc);
                }
            // for each orientation, attempt placements
            for (auto& ori : orients) {
                int maxx = 0, maxy = 0;
                for (auto& p : ori) {
                    maxx = std::max(maxx, p.first);
                    maxy = std::max(maxy, p.second);
                }
                for (int oy = 0; oy + maxy < H; ++oy)
                    for (int ox = 0; ox + maxx < W; ++ox) {
                        vi rowcols;
                        for (auto& p : ori) {
                            int cx = ox + p.first, cy = oy + p.second;
                            rowcols.push_back(P + cy * W + cx);
                        }
                        // add rows for each instance index
                        for (int inst = 0; inst < counts[s]; ++inst) {
                            vi r = rowcols;
                            r.push_back(baseIdx[s] + inst);
                            rows.push_back(r);
                        }
                    }
            }
        }

        // if any shape has zero placements, impossible
        bool anyZero = false;
        for (int s = 0; s < numShapes; ++s)
            if (counts[s] > 0) {
                int cntRows = 0;
                int start = baseIdx[s];
                int end = baseIdx[s] + counts[s];
                for (int r = 0; r < (int)rows.size(); ++r)
                    for (int c : rows[r])
                        if (c >= start && c < end) {
                            ++cntRows;
                            break;
                        }
                if (cntRows == 0) {
                    anyZero = true;
                    break;
                }
            }
        if (anyZero)
            continue;

        bool ok = true;
        // if there are zero primary columns, trivially ok
        if (P > 0) {
            ok = solve_exact_cover(P, S, rows);
        }
        if (ok)
            ++answer;
    }

    std::cout << answer << std::endl;
    return 0;
}

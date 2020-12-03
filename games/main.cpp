#include <cstdlib>
#include <functional>
#include <iostream>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Game {
    class Table {
    public:
        enum CellType {
            Empty,
            Wall,
            Runner,
            Terminator,
        };

        Table() = default;

        Table(size_t nrows, size_t ncols) : _rows(nrows), _cols(ncols) {}

        void Load(std::istream &is) {
            _table.resize(_rows);
            for (auto &row : _table) {
                is >> row;
            }
        }

        struct Point {
            int x = 0;
            int y = 0;

            Point() = default;

            Point(int x, int y) : x(x), y(y) {}

            Point(const Point &other) = default;

            bool operator==(const Point &other) const {
                return x == other.x && y == other.y;
            }

            bool operator!=(const Point &other) const { return !(*this == other); }
        };

        bool IsValidPoint(const Point &p) const {
            return 0 <= p.x && p.x < _rows && 0 <= p.y && p.y < _cols;
        }

        bool ArePointsOnLine(const Point &p1, const Point &p2) const {
            int dx = std::abs(p1.x - p2.x);
            int dy = std::abs(p1.y - p2.y);
            bool onLine = (dx == dy || !dx || !dy);
            if (!onLine) {
                return false;
            }
            if (dx) {
                dx /= p1.x - p2.x;
            }
            if (dy) {
                dy /= p1.y - p2.y;
            }
            int x = p2.x, y = p2.y;
            while (x != p1.x || y != p1.y) {
                if (GetCellType(Point(x, y)) == CellType::Wall) {
                    return false;
                }
                x += dx;
                y += dy;
            }
            return true;
        }

        std::pair<size_t, size_t> GetShape() const { return {_rows, _cols}; }

        CellType GetCellType(const Point &p) const {
            return static_cast<CellType>(_table[p.x][p.y] - '0');
        }

        Point FindCell(CellType type) const {
            for (size_t r = 0; r < _rows; ++r) {
                for (size_t c = 0; c < _cols; ++c) {
                    auto cell = Point(r, c);
                    if (GetCellType(cell) == type) {
                        return cell;
                    }
                }
            }
            throw std::invalid_argument("Not found");
        }

    private:
        size_t _rows = 8;
        size_t _cols = 8;
        std::vector<std::string> _table;
    };

    class Graph {
    public:
        struct Vertex {
            Table::Point runner;
            Table::Point terminator;
            bool isRunnersMove = false;

            Vertex() = default;

            Vertex(Table::Point runner, Table::Point terminator, bool isRunnersMove)
                    : runner(runner), terminator(terminator), isRunnersMove(isRunnersMove) {
            }

            bool operator==(const Vertex &other) const {
                return runner == other.runner && terminator == other.terminator &&
                       isRunnersMove == other.isRunnersMove;
            }
        };

        using VertexId = size_t;
        using AdjacencyList = std::vector<VertexId>;

        class VertexHash {
        public:
            size_t operator()(const Vertex &v) const {
                std::vector<int> coeffs = {v.runner.x, v.runner.y, v.terminator.x,
                                           v.terminator.y, v.isRunnersMove};
                size_t hash = 0;
                for (auto value : coeffs) {
                    hash *= _BASE;
                    hash += value;
                }
                return hash;
            }

        private:
            static const size_t _BASE = 13; // magic
        };

        enum Winner {
            First,
            Second,
            Undefined,
        };

        struct VertexInfo {
            bool used = false;
            Winner winner = Winner::Undefined;
            size_t openNeighbours = 0;
        };

        void LoadTableAndInitGraph(std::istream &is) {
            _table.Load(is);
            InitializeVertices();
            InitializeEdges();
        }

        Winner InitState(const Vertex &v) const {
            VertexId currId = _idByVertex.at(v);
            if (_g[currId].empty()) {
                if (_table.ArePointsOnLine(v.runner, v.terminator)) {
                    return (v.isRunnersMove ? Winner::Second : Winner::First);
                } else if (v.runner.x + 1 == _table.GetShape().first && v.isRunnersMove) {
                    return Winner::First;
                }
            }
            return Winner::Undefined;
        }

        Winner FindWinner() {
            Vertex start(_table.FindCell(Table::CellType::Runner),
                         _table.FindCell(Table::CellType::Terminator), true);
            VertexId startId = _idByVertex[start];
            for (VertexId id = 0; id < _vertices.size(); ++id) {
                if (InitState(_vertices[id]) != Winner::Undefined) {
                    Dfs(id, true);
                }
            }
            return _info[startId].winner;
        }

    private:
        std::unordered_map<Vertex, VertexId, VertexHash> _idByVertex;
        std::vector<Vertex> _vertices;
        std::vector<VertexInfo> _info;
        std::vector<AdjacencyList> _g;
        std::vector<AdjacencyList> _gReversed;
        Table _table;

        bool IsValidVertex(const Vertex &v) const {
            return (v.runner != v.terminator &&
                    _table.GetCellType(v.runner) != Table::CellType::Wall &&
                    _table.GetCellType(v.terminator) != Table::CellType::Wall);
        }

        void Dfs(VertexId from, bool isReversed = false) {
            _info[from].used = true;
            for (VertexId to : (isReversed ? _gReversed[from] : _g[from])) {
                if (_info[to].used) {
                    continue;
                }
                if (_info[from].winner == Winner::Second) {
                    _info[to].winner = Winner::First;
                } else {
                    if (_info[to].openNeighbours) {
                        _info[to].openNeighbours--;
                    }
                    if (!_info[to].openNeighbours) {
                        _info[to].winner = Winner::Second;
                    } else {
                        continue;
                    }
                }
                Dfs(to, isReversed);
            }
        }

        void InitializeVertices() {
            auto[nrows, ncols] = _table.GetShape();
            VertexId currId = 0;
            for (int runnerPos = 0; runnerPos < nrows * ncols; ++runnerPos) {
                for (int termPos = 0; termPos < nrows * ncols; ++termPos) {
                    for (int flag = 0; flag < 2; ++flag) {
                        Vertex v(Table::Point(runnerPos / ncols, runnerPos % ncols),
                                 Table::Point(termPos / ncols, termPos % ncols),
                                 v.isRunnersMove = flag);
                        if (IsValidVertex(v)) {
                            _idByVertex[v] = currId++;
                            _vertices.push_back(v);
                        }
                    }
                }
            }
            _g.resize(_vertices.size());
            _gReversed.resize(_vertices.size());
            _info.resize(_vertices.size());
            InitializeStates();
        }

        std::vector<VertexId> GetNeighbours(const Vertex &v) const {
            // end of game
            if (InitState(v) != Winner::Undefined) {
                return {};
            }

            std::vector<VertexId> neighbours;
            neighbours.reserve(8);
            for (int dx = -1; dx < 2; ++dx) {
                for (int dy = -1; dy < 2; ++dy) {
                    if (!dx && !dy) {
                        continue;
                    }
                    Table::Point newPoint;
                    if (v.isRunnersMove) {
                        newPoint = Table::Point(v.runner.x + dx, v.runner.y + dy);
                    } else {
                        newPoint = Table::Point(v.terminator.x + dx, v.terminator.y + dy);
                    }
                    if (!_table.IsValidPoint(newPoint)) {
                        continue;
                    }
                    Vertex next;
                    next.runner = (v.isRunnersMove ? newPoint : v.runner);
                    next.terminator = (v.isRunnersMove ? v.terminator : newPoint);
                    next.isRunnersMove = !v.isRunnersMove;
                    if (IsValidVertex(next) && _idByVertex.count(next)) {
                        neighbours.push_back(_idByVertex.at(next));
                    }
                }
            }
            return neighbours;
        }

        void InitializeEdges() {
            for (const auto &v : _vertices) {
                VertexId currId = _idByVertex[v];
                for (VertexId to : GetNeighbours(v)) {
                    _g[currId].push_back(to);
                    _gReversed[to].push_back(currId);
                }
            }
            for (VertexId id = 0; id < _vertices.size(); ++id) {
                _info[id].openNeighbours = _g[id].size();
            }
        }

        void InitializeStates() {
            for (const auto &v : _vertices) {
                VertexId id = _idByVertex[v];
                _info[id].winner = InitState(v);
            }
        }
    };

} // namespace Game

void solve(std::istream &in, std::ostream &out) {
    Game::Graph g;
    g.LoadTableAndInitGraph(in);
    auto winner = g.FindWinner();
    if (winner == Game::Graph::Winner::First) {
        out << "1\n";
    } else {
        out << "-1\n";
    }
}

int main() {
#ifdef LOCAL
    std::ifstream fin("../input.txt");
    solve(fin, std::cout);
#else
    solve(std::cin, std::cout);
#endif
    return 0;
}
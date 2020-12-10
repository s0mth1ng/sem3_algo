#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector>

class Vector {
  private:
    const static size_t _DIMENSION = 3;
    std::array<double, _DIMENSION> coords;

  public:
    static Vector Zero;

    Vector() : coords({}) {}

    Vector(double x, double y, double z) : coords({x, y, z}) {}

    Vector(const std::initializer_list<double> &newCoords) {
        if (newCoords.size() > _DIMENSION) {
            throw std::invalid_argument("Dimension is too big.");
        }
        coords.fill(0);
        std::copy(newCoords.begin(), newCoords.end(), coords.begin());
    }

    double length() const {
        double res = 0;
        for (auto val : coords) {
            res += val * val;
        }
        return sqrt(res);
    }

    bool operator==(const Vector &v) const {
        for (size_t i = 0; i < _DIMENSION; ++i) {
            if (coords[i] != v.coords[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator<(const Vector &v) const { return coords < v.coords; }

    double operator[](size_t ind) const { return coords[ind]; }

    Vector operator+(const Vector &v) const {
        Vector res;
        for (size_t i = 0; i < _DIMENSION; ++i) {
            res.coords[i] = coords[i] + v.coords[i];
        }
        return res;
    }

    Vector operator-(const Vector &v) const {
        Vector res;
        for (size_t i = 0; i < _DIMENSION; ++i) {
            res.coords[i] = coords[i] - v.coords[i];
        }
        return res;
    }

    double operator*(const Vector &v) const {
        double res = 0;
        for (size_t i = 0; i < _DIMENSION; ++i) {
            res += coords[i] * v.coords[i];
        }
        return res;
    }

    Vector operator*(double a) const {
        Vector res;
        for (size_t i = 0; i < _DIMENSION; ++i) {
            res.coords[i] = coords[i] * a;
        }
        return res;
    }

    Vector operator%(const Vector &v) const {
        Vector res;
        for (size_t i = 0; i < _DIMENSION; ++i) {
            res.coords[i] =
                coords[(i + 1) % _DIMENSION] * v.coords[(i + 2) % _DIMENSION] -
                coords[(i + 2) % _DIMENSION] * v.coords[(i + 1) % _DIMENSION];
        }
        return res;
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector &v) {
        os << '{';
        for (size_t i = 0; i < _DIMENSION; ++i) {
            if (i) {
                os << ", ";
            }
            os << v.coords[i];
        }
        os << '}';
        return os;
    }

    friend std::istream &operator>>(std::istream &is, Vector &v) {
        for (size_t i = 0; i < _DIMENSION; ++i) {
            is >> v.coords[i];
        }
        return is;
    }

    void ScanAs2d(std::istream &is) {
        is >> coords[0] >> coords[1];
        coords[2] = 0;
    }
};

void SortByAngle(std::vector<Vector> &pts) {
    if (pts.empty()) {
        return;
    }
    auto it = std::min_element(begin(pts), end(pts),
                               [](const auto &lhs, const auto &rhs) {
                                   return std::make_pair(lhs[1], -lhs[0]) <
                                          std::make_pair(rhs[1], -rhs[0]);
                               });
    std::iter_swap(pts.begin(), it);
    const auto &p0 = pts[0];
    std::sort(begin(pts) + 1, end(pts), [p0](const auto &lhs, const auto &rhs) {
        auto a = lhs - p0;
        auto b = rhs - p0;
        auto c = a % b;
        if (c[2] == 0) {
            return a.length() > b.length();
        }
        return c[2] > 0;
    });
}

std::vector<Vector> getConvexHull(std::vector<Vector> &pts) {
    if (pts.size() < 3) {
        throw std::invalid_argument("Number of points is less than 3");
    }
    std::sort(pts.begin(), pts.end());
    pts.erase(std::unique(pts.begin(), pts.end()), pts.end());
    SortByAngle(pts);
    std::vector<Vector> hull = {pts[0], pts[1]};
    for (size_t i = 2; i < pts.size(); ++i) {
        const auto &p3 = pts[i];
        while (hull.size() >= 2) {
            const auto &p1 = hull[hull.size() - 2];
            const auto &p2 = hull.back();
            auto F = p2 - p1;
            auto S = p3 - p2;
            if ((F % S)[2] < 0) {
                hull.pop_back();
            } else {
                break;
            }
        }
        hull.push_back(p3);
    }
    return hull;
}

double ComputePerimeter(const std::vector<Vector> &polygon) {
    double ans = 0;
    for (size_t i = 0; i < polygon.size(); ++i) {
        size_t ne = (i + 1) % polygon.size();
        ans += (polygon[ne] - polygon[i]).length();
    }
    return ans;
}

void solve(std::istream &in, std::ostream &out) {
    size_t n_pts;
    in >> n_pts;
    std::vector<Vector> pts(n_pts);
    for (auto &pt : pts) {
        pt.ScanAs2d(in);
    }
    auto hull = getConvexHull(pts);
    out << std::setprecision(10) << std::fixed << ComputePerimeter(hull);
}

void TestConstructor() {
    {
        Vector v({1, 2, 3});
        assert(v == Vector(1, 2, 3));
    }
    {
        Vector v({1, 2});
        assert(v == Vector(1, 2, 0));
    }
    {
        Vector v({1});
        assert(v == Vector(1, 0, 0));
    }
    {
        Vector v({});
        assert(v == Vector());
    }
}

int main() {
#ifdef LOCAL
    std::ifstream fin("../input.txt");
    TestConstructor();
    solve(fin, std::cout);
#else
    solve(std::cin, std::cout);
#endif
    return 0;
}

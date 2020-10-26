#include <array>
#include <iterator>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <algorithm>

class Vector {
private:
    std::array<double, 3> coords;

public:
    static Vector Zero;

    Vector() : coords({}) {}

    Vector(double x, double y, double z) : coords({x, y, z}) {}

    double length() const {
        double res = 0;
        for (auto val : coords) {
            res += val * val;
        }
        return sqrt(res);
    }

    bool operator==(const Vector &v) const {
        for (size_t i = 0; i < 3; ++i) {
            if (coords[i] != v.coords[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator<(const Vector &v) const {
        return coords < v.coords;
    }

    double operator[](size_t ind) const { return coords[ind]; }

    Vector operator+(const Vector &v) const {
        Vector res;
        for (size_t i = 0; i < 3; ++i) {
            res.coords[i] = coords[i] + v.coords[i];
        }
        return res;
    }

    Vector operator-(const Vector &v) const {
        Vector res;
        for (size_t i = 0; i < 3; ++i) {
            res.coords[i] = coords[i] - v.coords[i];
        }
        return res;
    }

    double operator*(const Vector &v) const {
        double res = 0;
        for (size_t i = 0; i < 3; ++i) {
            res += coords[i] * v.coords[i];
        }
        return res;
    }

    Vector operator*(double a) const {
        Vector res;
        for (size_t i = 0; i < 3; ++i) {
            res.coords[i] = coords[i] * a;
        }
        return res;
    }

    Vector operator%(const Vector &v) const {
        Vector res;
        for (size_t i = 0; i < 3; ++i) {
            res.coords[i] = coords[(i + 1) % 3] * v.coords[(i + 2) % 3] -
                            coords[(i + 2) % 3] * v.coords[(i + 1) % 3];
        }
        return res;
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector &v) {
        os << '{';
        for (size_t i = 0; i < 3; ++i) {
            if (i) {
                os << ", ";
            }
            os << v.coords[i];
        }
        os << '}';
        return os;
    }

    friend std::istream &operator>>(std::istream &is, Vector &v) {
        for (size_t i = 0; i < 2; ++i) {
            is >> v.coords[i];
        }
        return is;
    }
};

std::vector<Vector> getConvexHull(std::vector<Vector> &pts) {
    std::sort(pts.begin(), pts.end());
    pts.erase(std::unique(pts.begin(), pts.end()), pts.end());

    size_t ind = 0;
    for (size_t i = 1; i < pts.size(); ++i) {
        if ((pts[ind][1] == pts[i][1] && pts[ind][0] > pts[i][0]) || (pts[ind][1] < pts[i][1])) {
            ind = i;
        }
    }

    std::swap(pts[ind], pts.back());
    std::vector<Vector> hull;
    hull.push_back(pts.back());
    pts.pop_back();

    const auto &p0 = hull.back();
    std::sort(begin(pts), end(pts), [p0](const auto &lhs, const auto &rhs) {
        auto a = lhs - p0;
        auto b = rhs - p0;
        auto c = a % b;
        if (c[2] == 0) {
            return a.length() > b.length();
        }
        return c[2] < 0;
    });

    hull.push_back(pts[0]);
    for (size_t i = 1; i < pts.size(); ++i) {
        const auto &p3 = pts[i];
        while (hull.size() >= 2) {
            const auto &p1 = hull[hull.size() - 2];
            const auto &p2 = hull.back();
            auto F = p2 - p1;
            auto S = p3 - p2;
            if ((F % S)[2] > 0) {
                hull.pop_back();
            } else {
                break;
            }
        }
        hull.push_back(p3);
    }

    return hull;
}

void solve(std::istream &in, std::ostream &out) {
    size_t n_pts;
    in >> n_pts;
    std::vector<Vector> pts(std::istream_iterator<Vector>(in), {});
    auto hull = getConvexHull(pts);
    double ans = 0;
    for (size_t i = 0; i < hull.size(); ++i) {
        size_t ne = (i + 1) % hull.size();
        ans += (hull[ne] - hull[i]).length();
    }

    out << std::setprecision(10) << std::fixed << ans;
}

int main() {
#ifdef LOCAL
    std::ifstream fin("input.txt");
    solve(fin, std::cout);
#else
    solve(std::cin, std::cout);
#endif
    return 0;
}

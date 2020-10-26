#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

template<typename T>
class Vector {
private:
    std::array<T, 3> coords;

public:
    static Vector Zero;

    Vector() : coords({}) {}

    Vector(T x, T y, T z) : coords({x, y, z}) {}

    T length() const {
        T res = 0;
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

    T operator[](size_t ind) const { return coords[ind]; }

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

    T operator*(const Vector &v) const {
        T res = 0;
        for (size_t i = 0; i < 3; ++i) {
            res += coords[i] * v.coords[i];
        }
        return res;
    }

    Vector operator*(T a) const {
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
        for (size_t i = 0; i < 3; ++i) {
            is >> v.coords[i];
        }
        return is;
    }
};

template<typename T>
Vector<T> Vector<T>::Zero{};

double get_min(const Vector<double> &a, const Vector<double> &b) {
    double t = a * b * -1 / (a * a);
    if (0 <= t && t <= 1) {
        return (a * t + b).length();
    } else {
        return std::min(b.length(), (a + b).length());
    }
}

void solve(std::istream &in, std::ostream &out) {
    Vector<double> s1, f1, s2, f2;
    in >> s1 >> f1 >> s2 >> f2;

    auto a = f1 - s1;
    auto b = f2 - s2;
    auto c = s1 - s2;
    double A = a * a;
    double B = a * b;
    double C = b * b;
    double D = a * c;
    double E = b * c;

    double det = A * C - B * B;

    double ans = 1e10;

    if (det == 0) {
        ans = get_min(a, c);
        ans = std::min(ans, get_min(a, c - a));
        ans = std::min(ans, get_min(a * (-1), c));
        ans = std::min(ans, get_min(a * (-1), a + c));
    } else {
        double det1 = E * B - C * D;
        double det2 = A * E - B * D;
        double t = det1 / det;
        double p = det2 / det;

        if (0 <= t && t <= 1 && 0 <= p && p <= 1) {
            ans = (a * t - b * p + c).length();
        } else {
            ans = get_min(b * (-1), c);
            ans = std::min(ans, get_min(a, c));
            ans = std::min(ans, get_min(a, c - b));
            ans = std::min(ans, get_min(b * (-1), a + c));
        }
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

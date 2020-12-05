#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <vector>

class Point2D {
public:
    using CoordType = int64_t;

    Point2D() : coords({0, 0}) {}

    Point2D(CoordType x, CoordType y) : coords({x, y}) {}

    bool operator==(const Point2D &other) const { return coords == other.coords; }

    bool operator<(const Point2D &other) const { return coords < other.coords; }

    CoordType operator*(const Point2D &other) const {
        return coords[0] * other.coords[0] + coords[1] * other.coords[1];
    }

    CoordType operator%(const Point2D &other) const {
        return coords[0] * other.coords[1] - coords[1] * other.coords[0];
    }

    Point2D operator+(const Point2D &other) const {
        return {coords[0] + other.coords[0], coords[1] + other.coords[1]};
    }

    Point2D operator-(const Point2D &other) const {
        return {coords[0] - other.coords[0], coords[1] - other.coords[1]};
    }

    CoordType operator[](size_t ind) const { return coords[ind]; }

    friend std::ostream &operator<<(std::ostream &os, const Point2D &p) {
        os << '{' << p[0] << ", " << p[1] << '}';
        return os;
    }

    friend std::istream &operator>>(std::istream &is, Point2D &p) {
        is >> p.coords[0] >> p.coords[1];
        return is;
    }

private:
    std::array<CoordType, 2> coords;
};

Point2D operator*(const Point2D &p, Point2D::CoordType a) {
    return {p[0] * a, p[1] * a};
}

Point2D operator*(Point2D::CoordType a, const Point2D &p) {
    return {p[0] * a, p[1] * a};
}

Point2D::CoordType ComputeOrientedArea(const Point2D &a,
                                       const Point2D &b,
                                       const Point2D &c) {
    Point2D v = b - a;
    Point2D u = c - a;
    return v % u;
}

struct Segment {
    Point2D start, end;
    size_t id;

    Segment(Point2D p1, Point2D p2, size_t id) : start(p1), end(p2), id(id) {
        if (end < start) {
            std::swap(start, end);
        }
    }

    long double ComputeY(Point2D::CoordType x) const {
        if (start[0] == end[0]) {
            return start[1];
        }
        return start[1] + 1.0L * (end[1] - start[1]) * (x - start[0]) / (end[0] - start[0]);
    }

    bool operator<(const Segment &other) const {
        Point2D::CoordType x = std::max(start[0], other.start[0]);
        return ComputeY(x) < other.ComputeY(x);
    }
};

bool AreProjectionsIntersect(const Segment &s1, const Segment &s2) {
    for (size_t i = 0; i < 2; ++i) {
        Point2D::CoordType left = std::max(std::min(s1.start[i], s1.end[i]), std::min(s2.start[i], s2.end[i]));
        Point2D::CoordType right = std::min(std::max(s1.start[i], s1.end[i]), std::max(s2.start[i], s2.end[i]));
        if (left > right) {
            return false;
        }
    }
    return true;
}

bool AreSegmentsIntersect(const Segment &s1, const Segment &s2) {
    return AreProjectionsIntersect(s1, s2) &&
           ComputeOrientedArea(s1.start, s1.end, s2.start) *
           ComputeOrientedArea(s1.start, s1.end, s2.end) <=
           0 &&
           ComputeOrientedArea(s2.start, s2.end, s1.start) *
           ComputeOrientedArea(s2.start, s2.end, s1.end) <=
           0;
}

struct Event {
    Point2D::CoordType x;
    bool is_end;
    size_t segment_id;

    Event(Point2D::CoordType x, bool is_end, size_t segment_id)
            : x(x), is_end(is_end), segment_id(segment_id) {}

    bool operator<(const Event &other) const {
        if (x == other.x) {
            return is_end < other.is_end;
        }
        return x < other.x;
    }
};

std::vector<Event> GetEvents(const std::vector<Segment> &segments) {
    size_t n_segments = segments.size();
    std::vector<Event> events;
    events.reserve(2 * n_segments);
    for (const auto &s : segments) {
        events.emplace_back(s.start[0], false, s.id);
        events.emplace_back(s.end[0], true, s.id);
    }
    std::sort(begin(events), end(events));
    return events;
}

std::optional<std::pair<size_t, size_t>> GetIntersection(
        const std::vector<Segment> &segments) {
    std::set<Segment> q;
    std::vector<std::set<Segment>::iterator> it_by_id(segments.size());
    for (const auto &e : GetEvents(segments)) {
        if (e.is_end) {
            auto it = it_by_id[e.segment_id];
            if (it != q.begin() && next(it) != q.end() &&
                AreSegmentsIntersect(*prev(it), *next(it))) {
                return {{prev(it)->id, next(it)->id}};
            }
            q.erase(it);
        } else {
            Segment curr = segments[e.segment_id];
            auto it = q.lower_bound(curr);
            if (it != q.begin() && AreSegmentsIntersect(curr, *prev(it))) {
                return {{curr.id, prev(it)->id}};
            }
            if (it != q.end() && AreSegmentsIntersect(curr, *it)) {
                return {{curr.id, it->id}};
            }
            it_by_id[curr.id] = q.insert(curr).first;
        }
    }

    return std::nullopt;
}

void solve(std::istream &in, std::ostream &out) {
    size_t n_segments;
    in >> n_segments;
    std::vector<Segment> segments;
    segments.reserve(n_segments);
    for (size_t i = 0; i < n_segments; ++i) {
        Point2D a, b;
        in >> a >> b;
        segments.emplace_back(a, b, i);
    }
    auto ans = GetIntersection(segments);
    if (ans.has_value()) {
        if (ans->first > ans->second) {
            std::swap(ans->first, ans->second);
        }
        out << "YES\n" << ans->first + 1 << ' ' << ans->second + 1;
    } else {
        out << "NO\n";
    }
}

int main() {
#ifdef LOCAL
    std::ifstream fin("input.txt");
    solve(fin, std::cout);
#else
    solve(std::cin, std::cout);
#endif
}

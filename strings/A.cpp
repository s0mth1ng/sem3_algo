#include <iostream>
#include <string>
#include <vector>

int GetNextPi(const std::string& text,
              const std::vector<int>& pi,
              int prev_pi,
              char cur_symbol) {
    int cur_pi = prev_pi;
    while (cur_pi > 0 && cur_symbol != text[cur_pi]) {
        cur_pi = pi[cur_pi - 1];
    }
    if (text[cur_pi] == cur_symbol) {
        cur_pi++;
    }
    return cur_pi;
}

std::vector<int> ComputePrefixFunction(const std::string& text) {
    std::vector<int> pi(text.size());
    for (size_t i = 1; i < text.size(); ++i) {
        pi[i] = GetNextPi(text, pi, pi[i - 1], text[i]);
    }
    return pi;
}

std::vector<int> GetAllOccurrences(const std::string& text,
                                   const std::string& pattern) {
    auto pi = ComputePrefixFunction(pattern);
    std::vector<int> beginIndices;
    beginIndices.reserve(text.size());
    int prev_pi = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        prev_pi = GetNextPi(pattern, pi, prev_pi, text[i]);
        if (prev_pi == (int)pattern.size()) {
            beginIndices.push_back(i + 1 - pattern.size());
        }
    }
    return beginIndices;
}

void solve(std::istream& in, std::ostream& out) {
    std::string text, pattern;
    in >> pattern >> text;
    for (auto ind : GetAllOccurrences(text, pattern)) {
        out << ind << ' ';
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    solve(std::cin, std::cout);
}

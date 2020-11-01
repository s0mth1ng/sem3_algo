#include <iostream>
#include <string>
#include <vector>

std::vector<int> ComputePrefixFunction(const std::string &text) {
    std::vector<int> pi(text.size());
    for (size_t i = 1; i < text.size(); ++i) {
        int j = pi[i - 1];
        while (j > 0 && text[i] != text[j])
            j = pi[j - 1];
        if (text[i] == text[j]) ++j;
        pi[i] = j;
    }
    return pi;
}

void solve(std::istream &in, std::ostream &out) {
    std::string text, pattern;
    in >> pattern >> text;
    auto pi = ComputePrefixFunction(pattern);
    int prev = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        int j = prev;
        while (j > 0 && text[i] != pattern[j])
            j = pi[j - 1];
        if (text[i] == pattern[j]) ++j;
        prev = j;
        if (prev == (int) pattern.size()) {
            out << i + 1 - pattern.size() << ' ';
        }
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    solve(std::cin, std::cout);
}

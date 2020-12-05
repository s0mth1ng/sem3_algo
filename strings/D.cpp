#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

#ifdef LOCAL

#include "test_runner.h"

#endif

std::vector<size_t> ComputeSuffixArrayBruteForce(std::string str) {
    str.push_back(0);

    size_t size = str.size();
    std::vector<std::string> suffixes;
    suffixes.reserve(size);
    for (size_t ind = 0; ind < size; ++ind) {
        suffixes.push_back(str.substr(ind));
    }
    std::sort(begin(suffixes), end(suffixes));
    std::vector<size_t> result(size);
    for (size_t i = 0; i < size; ++i) {
        result[i] = size - suffixes[i].size();
    }
    return {begin(result) + 1, end(result)};
}

void SortSuffixesByLength(std::vector<size_t>& suf,
                          const std::vector<size_t>& cls,
                          size_t length) {
    size_t size = suf.size();
    std::vector<size_t> tmp_suf(size);
    std::vector<size_t> tmp_counter(size);
    for (size_t i = 0; i < size; ++i) {
        tmp_suf[i] = (suf[i] - length + size) % size;
    }
    for (size_t i = 0; i < size; ++i) {
        tmp_counter[cls[tmp_suf[i]]]++;
    }
    for (size_t i = 1; i < size; ++i) {
        tmp_counter[i] += tmp_counter[i - 1];
    }
    for (int i = static_cast<int>(size) - 1; i >= 0; --i) {
        suf[--tmp_counter[cls[tmp_suf[i]]]] = tmp_suf[i];
    }
}

void UpdateClassesByLength(const std::vector<size_t>& suf,
                           std::vector<size_t>& cls,
                           size_t length) {
    size_t size = suf.size();
    std::vector<size_t> tmp_classes(size);
    tmp_classes[suf[0]] = 0;
    size_t classes_count = 1;
    for (size_t i = 1; i < size; ++i) {
        size_t lhs = (suf[i] + length) % size;
        size_t rhs = (suf[i - 1] + length) % size;
        if (cls[suf[i]] != cls[suf[i - 1]] || cls[lhs] != cls[rhs]) {
            ++classes_count;
        }
        tmp_classes[suf[i]] = classes_count - 1;
    }
    std::copy(begin(tmp_classes), end(tmp_classes), begin(cls));
}

std::vector<size_t> ComputeSuffixArray(std::string str) {
    str.push_back(0);

    size_t size = str.size();
    std::vector<size_t> cls(size), suf(size);

    // sorting single chars
    size_t classes_count = 0;
    size_t suf_ind = 0;
    std::vector<std::vector<size_t>> ind_by_char(256);
    for (size_t ind = 0; ind < size; ++ind) {
        ind_by_char[str[ind]].push_back(ind);
    }
    for (size_t i = 0; i < ind_by_char.size(); ++i) {
        for (size_t ind : ind_by_char[i]) {
            cls[ind] = classes_count;
            suf[suf_ind++] = ind;
        }
        classes_count += !ind_by_char[i].empty();
    }

    for (size_t length = 1; length < size; length <<= 1lu) {
        SortSuffixesByLength(suf, cls, length);
        UpdateClassesByLength(suf, cls, length);
    }

    return {begin(suf) + 1, end(suf)};
}

std::vector<size_t> ComputeLCP(const std::string& str,
                               const std::vector<size_t>& suf_array) {
    size_t size = suf_array.size();
    std::vector<size_t> reverse_suf(size);
    for (size_t i = 0; i < size; ++i) {
        reverse_suf[suf_array[i]] = i;
    }

    std::vector<size_t> lcp(size);
    size_t cur_lcp = 0;
    for (size_t i = 0; i < size; ++i) {
        if (reverse_suf[i] + 1 == size) {
            cur_lcp = 0;
            continue;
        }
        size_t start = suf_array[reverse_suf[i] + 1];
        while (std::max(i, start) + cur_lcp < size &&
               str[i + cur_lcp] == str[start + cur_lcp]) {
            cur_lcp++;
        }
        lcp[reverse_suf[i]] = cur_lcp;
        if (cur_lcp) {
            cur_lcp--;
        }
    }

    return lcp;
}

void test_suffix_computation() {
    ASSERT_EQUAL(ComputeSuffixArray(""), ComputeSuffixArrayBruteForce(""))
    ASSERT_EQUAL(ComputeSuffixArray("a"), ComputeSuffixArrayBruteForce("a"))

    srand(time(nullptr));
    size_t const alphabet_size = 26;
    std::vector<char> alphabet(alphabet_size);
    for (size_t i = 0; i < alphabet_size; ++i) {
        alphabet[i] = 'a' + i;
    }

    size_t const size = 1000;
    std::string s;
    for (size_t i = 0; i < size; ++i) {
        s.push_back(alphabet[rand() % alphabet_size]);
    }
    ASSERT_EQUAL(ComputeSuffixArray(s), ComputeSuffixArrayBruteForce(s))
}

void run_tests() {
    TestRunner tr;
    RUN_TEST(tr, test_suffix_computation);
}

void solve(std::istream& in, std::ostream& os) {
    std::string s;
    in >> s;
    auto suf_arr = ComputeSuffixArray(s);
    auto lcp = ComputeLCP(s, suf_arr);
    size_t size = s.size();
    uint64_t ans = static_cast<uint64_t>(size) * (size + 1) / 2 -
                   std::accumulate(begin(lcp), end(lcp), 0ul);
    os << ans;
}

int main() {
#ifdef LOCAL
    std::ifstream fin("input.txt");
    run_tests();
    solve(fin, std::cout);
#else
    // ~~~~~~~~~~~~~~~ fast io ~~~~~~~~~~~~~~~~~~
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    solve(std::cin, std::cout);
#endif
    return 0;
}

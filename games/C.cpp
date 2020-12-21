#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <ostream>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

int mex(const std::set<int> &a) {
  int res = 0;
  while (a.count(res)) {
    res++;
  }
  return res;
}

int FindWinner(size_t n, std::vector<size_t> &moves) {
  if (n == 2) {
    moves.push_back(1);
    moves.push_back(2);
    return 1;
  }
  std::vector<int> winning(n + 1, 0);
  winning[0] = 0;
  winning[1] = 0;
  winning[2] = 1;
  for (size_t k = 3; k <= n; ++k) {
    std::set<int> a;
    for (size_t aim = 1; aim <= k; ++aim) {
      int curr = 0;
      if (!std::min(aim - 1, k - aim)) {
        curr = winning[k - 1];
      } else if (std::min(aim - 1, k - aim) == 1) {
        curr = (k == 3 ? 0 : winning[k - 2]);
      } else {
        curr = winning[aim - 1] ^ winning[k - aim];
      }
      a.insert(curr);
      if (!curr && k == n) {
        moves.push_back(aim);
      }
    }
    winning[k] = mex(a);
  }
  return winning[n];
}

void solve(std::istream &in, std::ostream &out) {
  size_t n;
  in >> n;
  std::vector<size_t> moves;
  int winner = FindWinner(n, moves);
  if (winner) {
    out << "Schtirlitz\n";
    for (size_t m : moves) {
      out << m << '\n';
    }
  } else {
    out << "Mueller\n";
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
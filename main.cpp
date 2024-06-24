#include <iostream>
#include <ostream>

#include "algorithm/dpll.hpp"

int main(int argc, char **argv)
{
  if (argc <= 1) {
    std::cout << "fatal error: no input files\n";
    return 47;
  }

  auto cnf = load_cnf(argv[1]);
  if (!cnf) {
    std::cout << "fatal error: file not existing or not satisfying DIMACS CNF format\n";
    return 47;
  }
  auto answer = dpll_algorithm(cnf.value());

  if (!answer) {
    std::cout << "NOT SATISFIABLE\n";
  }
  else {
    std::cout << "SATISFIABLE\n";
    for (auto [variable, value] : answer.value()) {
      std::cout << (value ? "" : "-") << variable << ' ';
    }
    std::cout << "\n";
  }
  return 0;
}

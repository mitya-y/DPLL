#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

struct Clause {
  std::vector<std::pair<bool, unsigned int>> variables;
  unsigned int number_of_free_variables = 0;
  bool satisfied = false;
};

struct CNF {
  std::vector<unsigned int> variables;
  std::vector<Clause> clauses;
};

std::optional<CNF> load_cnf(const std::string_view filename);
void print_dimacs_format(const CNF &dimacs);

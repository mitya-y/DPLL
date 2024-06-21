#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

struct Conjunction {
  std::vector<std::pair<bool, uint>> disjunctions;
  uint number_of_free_elements = 0;
  bool satisfied = false;
  bool operator==(const Conjunction &other) const;
};

struct DimacsFormat {
  std::vector<uint> variables;
  std::vector<Conjunction> conjunctions;
  bool operator==(const DimacsFormat &other) const;
};

std::optional<DimacsFormat> load_cnf(std::string_view filename);
void print_dimacs_format(const DimacsFormat &dimacs);

#include "dimacs_loader.hpp"

#include <cctype>
#include <fstream>
#include <iostream>
#include <ranges>
#include <set>
#include <sstream>

static std::string strip(const std::string &s)
{
  auto start = s.find_first_not_of(" \t");
  if (start == s.npos) {
    return "";
  }
  auto end = s.find_last_not_of(" \t");
  size_t size = end - start + 1;
  return s.substr(start, size);
}

std::optional<CNF> load_cnf(std::string_view filename)
{
  std::ifstream file(filename.data());
  if (!file) {
    return std::nullopt;
  }

  // find p string
  std::string line;
  while (std::getline(file, line)) {
    line = strip(line);
    if (line.size() == 0 || line[0] == 'c') {
      continue;
    }
    if (line[0] == 'p') {
      break;
    }
    return std::nullopt;
  }

  unsigned int variables_number, clauses_number;
  if (sscanf(line.data(), "p cnf %d %d", &variables_number, &clauses_number) != 2) {
    return std::nullopt;
  }

  std::set<unsigned int> variables;
  std::vector<Clause> clauses;
  unsigned int i = 0;
  while (std::getline(file, line)) {
    line = strip(line);
    if (line.size() == 0 || line[0] == 'c') {
      continue;
    }
    if (line[0] == 'p') {
      return std::nullopt;
    }

    std::istringstream stream(line);
    Clause clause;
    int var;
    while (true) {
      stream >> var;
      if (var == 0) {
        break;
      }
      variables.insert(std::abs(var));
      clause.variables.push_back({var >= 0, std::abs(var)});
    }

    clause.number_of_free_variables = clause.variables.size();
    clauses.push_back(std::move(clause));
  }

  return CNF{
      std::vector(variables.begin(), variables.end()),
      std::move(clauses),
  };
}

void print_dimacs_format(const CNF &dimacs)
{
  std::cout << "variables: ";
  for (int v : dimacs.variables) {
    std::cout << v << " ";
  }
  std::cout << "\n";
  for (auto &dis : dimacs.clauses) {
    for (auto &&[pos, v] : dis.variables) {
      std::cout << (pos ? "" : "!") << v << " ";
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

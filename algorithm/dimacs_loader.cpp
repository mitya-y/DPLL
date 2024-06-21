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

std::optional<DimacsFormat> load_cnf(std::string_view filename)
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

  // parse and check p string
  std::istringstream stream(line);
  std::string p, cnf;
  stream >> p >> cnf;
  if (p != "p" || cnf != "cnf") {
    return std::nullopt;
  }

  // auto vec = std::ranges::istream_view<int>(stream) | std::ranges::to<std::vector>();
  std::vector<int> vec;
  auto readed_range = std::ranges::istream_view<int>(stream);
  // std::ranges::copy(readed_range, std::back_inserter(vec));
  for (auto &&i : readed_range) {
    vec.push_back(i);
  }

  if (vec.size() != 2) {
    return std::nullopt;
  }
  uint variables_number = vec[0];
  uint conjunctions_number = vec[1];
  if (variables_number < 0 || conjunctions_number < 0) {
    return std::nullopt;
  }

  std::set<uint> variables;
  std::vector<Conjunction> conjunctions;
  uint i = 0;
  while (std::getline(file, line)) {
    line = strip(line);
    if (line.size() == 0 || line[0] == 'c') {
      continue;
    }
    if (line[0] == 'p') {
      return std::nullopt;
    }

    std::istringstream stream(line);
    Conjunction conjunction;

    // conjunction.disjunctions =
    //   std::ranges::istream_view<int>(stream) | std::views::take_while([](int var) { return var != 0; }) |
    //   std::views::transform([&variables](int var) { return variables.insert(std::abs(var)), var; }) |
    //   std::views::transform([](int var) { return std::make_pair<bool, uint>(var >= 0, std::abs(var)); }) |
    //   std::ranges::to<std::vector>();

    auto readed_range =
        std::ranges::istream_view<int>(stream) | std::views::take_while([](int var) { return var != 0; }) |
        std::views::transform([&variables](int var) { return variables.insert(std::abs(var)), var; }) |
        std::views::transform([](int var) { return std::make_pair<bool, uint>(var >= 0, std::abs(var)); });
    // std::ranges::copy(readed_range, std::back_inserter(conjunction.disjunctions));
    for (auto &&pair : readed_range) {
      conjunction.disjunctions.push_back(pair);
    }

    conjunction.number_of_free_elements = conjunction.disjunctions.size();
    conjunctions.push_back(std::move(conjunction));
  }

  if (variables.size() > variables_number || conjunctions.size() != conjunctions_number) {
    return std::nullopt;
  }
  return DimacsFormat{
      std::vector(variables.begin(), variables.end()),
      std::move(conjunctions),
  };
}

void print_dimacs_format(const DimacsFormat &dimacs)
{
  std::cout << "variables: ";
  for (int v : dimacs.variables) {
    std::cout << v << " ";
  }
  std::cout << "\n";
  for (auto &dis : dimacs.conjunctions) {
    for (auto &&[pos, v] : dis.disjunctions) {
      std::cout << (pos ? "" : "!") << v << " ";
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

bool Conjunction::operator==(const Conjunction &other) const
{
  return other.disjunctions == disjunctions && other.number_of_free_elements == number_of_free_elements &&
         other.satisfied == satisfied;
}

bool DimacsFormat::operator==(const DimacsFormat &other) const
{
  return other.variables == variables && other.conjunctions == conjunctions;
}

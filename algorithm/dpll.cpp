#include "dpll.hpp"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <set>
#include <unordered_map>
#include <unordered_set>

struct Variable {
  unsigned int original_number;
  bool value = false;
  bool used = false;
  // indexes of conjustions in which this variable occurence and is positive in this
  std::vector<std::pair<unsigned int, bool>> occurrence;
};

struct ChangedVariable {
  unsigned int index;
  bool restore = false;
};

static void set_satisfied_flag(const std::vector<Variable> &variables, Clause &clause)
{
  clause.satisfied = std::any_of(clause.variables.begin(), clause.variables.end(), [&variables](const auto &v) {
    return (!(v.first ^ variables[v.second].value)) && variables[v.second].used;
  });
}

static bool unit_propagation(CNF &cnf, std::vector<Variable> &variables, std::unordered_set<unsigned int> &all,
                             std::vector<ChangedVariable> &changed_variables)
{
  unsigned int cnt = 0;
  bool changed = false;
  unsigned int index = 0;
  for (auto &clause : cnf.clauses) {
    if (clause.number_of_free_variables == 1 && !clause.satisfied) {
      cnt++;
      auto &dis = clause.variables;
      auto index_iter = std::find_if(dis.begin(), dis.end(), [&](auto &pair) { return !variables[pair.second].used; });

      Variable &var = variables[index_iter->second];
      var.used = true;
      var.value = index_iter->first;
      changed_variables.push_back(ChangedVariable{index_iter->second, true});

      clause.satisfied = true;

      all.erase(index_iter->second);

      for (auto &[cl, _] : var.occurrence) {
        cnf.clauses[cl].number_of_free_variables--;
        set_satisfied_flag(variables, cnf.clauses[cl]);
      }

      changed = true;
    }
  }
  return changed;
}

static bool pure_literal_elimination(CNF &cnf, std::vector<Variable> &variables, std::unordered_set<unsigned int> &all,
                                     std::vector<ChangedVariable> &changed_variables)
{
  unsigned int index = 0;
  bool changed = false;
  for (auto &variable : variables) {
    if (variable.used) {
      continue;
    }

    auto &occur = variable.occurrence;
    bool all_positive = std::all_of(occur.begin(), occur.end(), [](auto &pair) { return pair.second; });
    bool all_negative = std::all_of(occur.begin(), occur.end(), [](auto &pair) { return !pair.second; });

    if (all_positive || all_negative) {
      changed = true;
      variable.value = all_positive;
      variable.used = true;

      all.erase(index);
      changed_variables.push_back(ChangedVariable{index, true});

      for (auto &&[clause, _] : occur) {
        cnf.clauses[clause].number_of_free_variables--;
        set_satisfied_flag(variables, cnf.clauses[clause]);
      }
    }
    index++;
  }
  return changed;
}

static bool check_unsat(const CNF &cnf, const std::vector<Variable> &variables)
{
  for (auto &clause : cnf.clauses) {
    bool value = std::all_of(clause.variables.begin(), clause.variables.end(), [&variables](const auto &v) {
      return (v.first ^ variables[v.second].value) && variables[v.second].used;
    });
    if (value) {
      return false;
    }
  }
  return true;
}

static bool check_success(const CNF &cnf)
{
  return std::all_of(cnf.clauses.begin(), cnf.clauses.end(), [](const auto &clause) { return clause.satisfied; });
}

static void restore_variable(CNF &cnf, std::vector<Variable> &variables, std::unordered_set<unsigned int> &all,
                             ChangedVariable &var)
{
  bool is_positive = variables[var.index].value;
  variables[var.index].used = false;
  if (var.restore) {
    all.insert(var.index);
  }
  for (auto &&[clause, _] : variables[var.index].occurrence) {
    cnf.clauses[clause].number_of_free_variables++;
    set_satisfied_flag(variables, cnf.clauses[clause]);
  }
}

static std::optional<std::vector<Variable>> recursive_dpll(CNF &cnf, std::vector<Variable> &variables,
                                                           std::unordered_set<unsigned int> &all,
                                                           std::vector<ChangedVariable> &changed_variables)
{
  while (unit_propagation(cnf, variables, all, changed_variables)) {
  }
  while (pure_literal_elimination(cnf, variables, all, changed_variables)) {
  }

  if (!check_unsat(cnf, variables)) {
    return std::nullopt;
  }

  if (check_success(cnf)) {
    return variables;
  }

  if (all.empty()) {
    return std::nullopt;
  }

  unsigned int var = *all.begin();
  all.erase(all.begin());

  for (unsigned int i = 0; i < 2; i++) {
    bool is_positive = i == 0;

    // change variable
    auto &variable = variables[var];
    variable.used = true;
    variable.value = is_positive;
    changed_variables.push_back(ChangedVariable{var, false});
    for (auto &&[clause, _] : variable.occurrence) {
      cnf.clauses[clause].number_of_free_variables--;
      set_satisfied_flag(variables, cnf.clauses[clause]);
    }

    auto result = recursive_dpll(cnf, variables, all, changed_variables);
    if (result) {
      return result.value();
    }

    // restore value for next iteration
    while (changed_variables.back().index != var) {
      restore_variable(cnf, variables, all, changed_variables.back());
      changed_variables.pop_back();
    }
    restore_variable(cnf, variables, all, changed_variables.back());
    changed_variables.pop_back();
  }

  return std::nullopt;
}

std::optional<DPLLResult> dpll_algorithm(const CNF &cnf)
{
  // array of variables
  size_t n = cnf.variables.size();
  auto iter = cnf.variables.begin();
  std::vector<Variable> variables(n);
  for (unsigned int i = 0; i < n; i++) {
    variables[i].original_number = *(iter++);
  }

  // mapping from all natural numbers to [0; N) for indexing in array
  std::unordered_map<unsigned int, unsigned int> variables_map;
  unsigned int id = 0;
  for (unsigned int v : cnf.variables) {
    if (!variables_map.contains(v)) {
      variables_map.insert(std::pair{v, id++});
    }
  }

  auto cnf_copy = cnf;
  auto &formula = cnf_copy.clauses;
  for (auto &clause : formula) {
    for (auto &[_, v] : clause.variables) {
      v = variables_map.find(v)->second;
    }
  }

  // calculate occurrence of each variable
  unsigned int index = 0;
  for (auto &clause : formula) {
    for (auto &&[value, v] : clause.variables) {
      variables[v].occurrence.push_back(std::pair{index, value});
    }
    index++;
  }

  std::unordered_set<unsigned int> all;
  for (unsigned int i = 0; i < n; i++) {
    all.insert(i);
  }

  // pure_literal_elimination(cnf_copy, variables, all);
  // if (!check_unsat(cnf_copy, variables)) {
  //   return std::nullopt;
  // }

  std::vector<ChangedVariable> changed_variables;
  auto result = recursive_dpll(cnf_copy, variables, all, changed_variables);
  if (!result) {
    return std::nullopt;
  }

  std::map<unsigned int, bool> answer;
  for (const auto var : result.value()) {
    answer.insert(std::pair{var.original_number, var.value});
  }
  return answer;
}

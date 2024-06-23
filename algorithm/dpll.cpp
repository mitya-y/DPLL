#include "dpll.hpp"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <set>
#include <unordered_map>

struct Variable {
  unsigned int original_number;
  bool value = false;
  bool used = false;
  // indexes of conjustions in which this variable occur and is positive in this
  std::vector<std::pair<unsigned int, bool>> occurence;
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

static bool unit_propagation(CNF &cnf, std::vector<Variable> &variables, std::set<unsigned int> &positive,
                             std::set<unsigned int> &negative, std::set<unsigned int> &all,
                             std::vector<ChangedVariable> &changed_variables)
{
  unsigned int cnt = 0;
  bool changed = false;
  unsigned int index = 0;
  // for (auto [index, clause] : std::views::enumerate(sat.conjunctions)) {
  for (auto clause : cnf.clauses) {
    if (clause.number_of_free_variables == 1 && !clause.satisfied) {
      cnt++;
      auto &dis = clause.variables;
      auto index_iter = std::find_if(dis.begin(), dis.end(), [&](auto &pair) { return !variables[pair.second].used; });

      Variable &var = variables[index_iter->second];
      var.used = true;
      var.value = index_iter->first;
      changed_variables.push_back(ChangedVariable{index_iter->second, true});

      clause.satisfied = true;

      (var.value ? positive : negative).insert(index_iter->second);
      all.erase(index_iter->second);

      for (auto &[cl, _] : var.occurence) {
        cnf.clauses[cl].number_of_free_variables--;
        set_satisfied_flag(variables, cnf.clauses[cl]);
      }

      changed = true;
    }
  }
  return changed;
}

static void pure_variable_elimination(CNF &cnf, std::vector<Variable> &variables, std::set<unsigned int> &positive,
                                      std::set<unsigned int> &negative, std::set<unsigned int> &all)
{
  // for (const auto [index, variable] : std::views::enumerate(variables)) {
  unsigned int index = 0;
  for (auto variable : variables) {
    auto &occur = variable.occurence;
    bool all_positive = std::all_of(occur.begin(), occur.end(), [](auto &pair) { return pair.second; });
    bool all_negative = std::all_of(occur.begin(), occur.end(), [](auto &pair) { return !pair.second; });

    if (all_positive || all_negative) {
      variable.value = all_positive;
      variable.used = true;

      all.erase(index);
      (all_positive ? positive : negative).insert(index);

      for (auto &&[clause, _] : occur) {
        cnf.clauses[clause].number_of_free_variables--;
        set_satisfied_flag(variables, cnf.clauses[clause]);
      }
    }
    index++;
  }
}

static bool check_satisfaing(const CNF &cnf, const std::vector<Variable> &variables)
{
  for (auto &clause : cnf.clauses) {
    // if (clause.satisfied)
    //   continue;
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

static void restore_variable(CNF &cnf, std::vector<Variable> &variables, std::set<unsigned int> &positive,
                             std::set<unsigned int> &negative, std::set<unsigned int> &all, ChangedVariable &var)
{
  bool is_positive = variables[var.index].value;
  (is_positive ? positive : negative).erase(var.index);
  variables[var.index].used = false;
  if (var.restore) {
    all.insert(var.index);
  }
  for (auto &&[clause, _] : variables[var.index].occurence) {
    cnf.clauses[clause].number_of_free_variables++;
    set_satisfied_flag(variables, cnf.clauses[clause]);
  }
}

static std::optional<std::vector<Variable>> recursive_dpll(CNF &cnf, std::vector<Variable> &variables,
                                                           std::set<unsigned int> &positive, std::set<unsigned int> &negative,
                                                           std::set<unsigned int> &all,
                                                           std::vector<ChangedVariable> &changed_variables)
{
  while (unit_propagation(cnf, variables, positive, negative, all, changed_variables)) {
  }

  if (!check_satisfaing(cnf, variables)) {
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
    (is_positive ? positive : negative).insert(var);
    auto &variable = variables[var];
    variable.used = true;
    variable.value = is_positive;
    changed_variables.push_back(ChangedVariable{var, false});
    for (auto &&[clause, _] : variable.occurence) {
      cnf.clauses[clause].number_of_free_variables--;
      set_satisfied_flag(variables, cnf.clauses[clause]);
    }

    // TODO : dont copy, make instuctions for backup (set of chaned variables)
    auto result = recursive_dpll(cnf, variables, positive, negative, all, changed_variables);
    if (result) {
      return result.value();
    }

    // restore value for next iteration
    while (changed_variables.back().index != var) {
      restore_variable(cnf, variables, positive, negative, all, changed_variables.back());
      changed_variables.pop_back();
    }
    restore_variable(cnf, variables, positive, negative, all, changed_variables.back());
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
      // variables_map[v] = id++;
      variables_map.insert(std::pair{v, id++});
    }
  }

  auto cnf_copy = cnf;
  auto &formula = cnf_copy.clauses;
  for (auto &clause : formula) {
    for (auto &[_, v] : clause.variables) {
      // v = variables_map[v];
      v = variables_map.find(v)->second;
    }
  }

  // calculate occurance of each variable
  // handled variant if in one conjucion variable occur twice and more
  // for (const auto [index, clause] : std::views::enumerate(formula)) {
  unsigned int index = 0;
  for (auto clause : formula) {
    std::unordered_map<unsigned int, std::pair<unsigned int, unsigned int>> counts;
    for (auto &&[positive, v] : clause.variables) {
      if (!counts.contains(v)) {
        // counts[v] = {0, 0};
        counts.insert(std::pair{v, std::pair{0, 0}});
      }
      // (positive ? counts[v].first : counts[v].second)++;
      (positive ? counts.find(v)->second.first : counts.find(v)->second.second)++;
    }

    for (auto &&[v, pair] : counts) {
      auto [poscnt, negcnt] = pair;
      if (poscnt + negcnt == 1) {
        variables[v].occurence.push_back(std::pair{index, poscnt != 0});
      }
      else {
        auto &dis = clause.variables;
        dis.erase(std::remove_if(dis.begin(), dis.end(), [v](auto &pair) { return pair.second == v; }), dis.end());

        if (poscnt == 0 || negcnt == 0) {
          dis.push_back({poscnt != 0, v});
          variables[v].occurence.push_back({index, poscnt != 0});
        }

        clause.number_of_free_variables = dis.size();

        if (clause.number_of_free_variables == 0) {
          clause.satisfied = true;
        }
      }
    }
    index++;
  }

  std::set<unsigned int> positive, negative, all;
  for (unsigned int i = 0; i < n; i++) {
    all.insert(i);
  }

  pure_variable_elimination(cnf_copy, variables, positive, negative, all);
  if (!check_satisfaing(cnf_copy, variables)) {
    return std::nullopt;
  }

  std::vector<ChangedVariable> changed_variables;
  auto result = recursive_dpll(cnf_copy, variables, positive, negative, all, changed_variables);
  if (!result) {
    return std::nullopt;
  }
  std::map<unsigned int, bool> answer;
  // TODO : reverse mappimg
  for (const auto var : result.value()) {
    // answer[var.index] = var.value; // todo:
    answer.insert(std::pair{var.original_number, var.value});
  }

  return answer;
}

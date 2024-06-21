#include <algorithm>
#include <filesystem>

#include "../algorithm/dpll.hpp"
#include "googletest/googletest/include/gtest/gtest.h"

static bool satisfy(const DimacsFormat &sat, DPLLResult &answer)
{
  return std::all_of(sat.conjunctions.begin(), sat.conjunctions.end(), [&answer](const Conjunction &con) {
    return std::any_of(con.disjunctions.begin(), con.disjunctions.end(),
                       // [&answer](const auto &var) { return !(var.first ^ answer[var.second]); });
                       [&answer](const auto &var) { return !(var.first ^ answer.find(var.second)->second); });
  });
}

bool check_satisfied_cnf(std::string_view filename)
{
  auto cnf = load_cnf(filename);
  if (!cnf) {
    std::cout << filename << " not existing or dont satisfy to dimacs format\n";
    return false;
  }
  auto answer = dpll_algorithm(cnf.value());
  if (!answer) {
    std::cout << "answer is bad\n";
    return false;
  }
  return satisfy(cnf.value(), answer.value());
}

bool check_unsatisfied_cnf(std::string_view filename)
{
  auto cnf = load_cnf(filename);
  if (!cnf) {
    std::cout << filename << " not existing or dont satisfy to dimacs format\n";
    return false;
  }
  auto answer = dpll_algorithm(cnf.value());
  return !answer;
}

TEST(TestAlgorithm, TestExampleCnf)
{
  ASSERT_TRUE(check_satisfied_cnf("files/example.cnf"));
}

TEST(TestAlgorithm, TestLightSatisfied)
{
  const size_t number_of_light_datasets = 3;
  for (size_t i = 0; i < number_of_light_datasets; i++)
    // ASSERT_TRUE(check_satisfied_cnf(std::format("files/light/{}.cnf", i + 1)));
    ASSERT_TRUE(check_satisfied_cnf("files/light/" + std::to_string(i + 1) + ".cnf"));
}

TEST(TestAlgorithm, TestLightUnsatisfied)
{
  const size_t number_of_light_unsat_datasets = 2;
  for (size_t i = 0; i < number_of_light_unsat_datasets; i++)
    // ASSERT_TRUE(check_unsatisfied_cnf(std::format("files/light/unsat{}.cnf", i + 1)));
    ASSERT_TRUE(check_unsatisfied_cnf("files/light/unsat" + std::to_string(i + 1) + ".cnf"));
}

TEST(TestAlgorithm, TestSat3Satisfied)
{
  for (const auto &filename : std::filesystem::directory_iterator("files/heavy/aim/sat"))
    ASSERT_TRUE(check_satisfied_cnf(filename.path().string()));
}

TEST(TestAlgorithm, TestSat3Unsatisfied)
{
  for (const auto &filename : std::filesystem::directory_iterator("files/heavy/aim/unsat"))
    ASSERT_TRUE(check_unsatisfied_cnf(filename.path().string()));
}

TEST(TestAlgorithm, TestSatSatisfied)
{
  for (const auto &filename : std::filesystem::directory_iterator("files/heavy/jnh/sat"))
    ASSERT_TRUE(check_satisfied_cnf(filename.path().string()));
}

TEST(TestAlgorithm, TestSatUnsatisfied)
{
  for (const auto &filename : std::filesystem::directory_iterator("files/heavy/jnh/unsat"))
    ASSERT_TRUE(check_unsatisfied_cnf(filename.path().string()));
}

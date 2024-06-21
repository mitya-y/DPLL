#include "../algorithm/dimacs_loader.hpp"
#include "googletest/googletest/include/gtest/gtest.h"

TEST(TestLoading, TestNotExistingFile)
{
  ASSERT_FALSE(load_cnf("not_exist.cnf"));
}

bool check_default_data(DimacsFormat &cnf)
{
  if (cnf.conjunctions.size() != 2) return false;
  DimacsFormat format{
      {1, 2, 3},  // set of variables
      {
          {{{true, 1}, {true, 2}, {false, 3}}, 3, false},
          {{{false, 2}, {true, 3}}, 2, false},
      }  // disjunctions
  };
  return cnf == format;
}

TEST(TestLoading, TestCorrectLoadingSimpleFile)
{
  auto cnf = load_cnf("files/example.cnf");
  ASSERT_TRUE(cnf);
  ASSERT_TRUE(check_default_data(cnf.value()));
}

TEST(TestLoading, TestComments)
{
  auto cnf = load_cnf("files/comments.cnf");
  ASSERT_TRUE(cnf);
  ASSERT_TRUE(check_default_data(cnf.value()));
}

TEST(TestLoading, TestEmptyLines)
{
  auto cnf = load_cnf("files/empty_lines.cnf");
  ASSERT_TRUE(cnf);
  ASSERT_TRUE(check_default_data(cnf.value()));
}

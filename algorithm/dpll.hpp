#pragma once

#include <map>

#include "dimacs_loader.hpp"

using DPLLResult = std::map<unsigned int, bool>;

std::optional<DPLLResult> dpll_algorithm(const CNF &sat);

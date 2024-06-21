#pragma once

#include <map>

#include "dimacs_loader.hpp"

using DPLLResult = std::map<uint, bool>;

std::optional<DPLLResult> dpll_algorithm(const DimacsFormat &sat);

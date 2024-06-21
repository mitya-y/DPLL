import os
import filecmp

files = [
    "algorithm/dpll.cpp",
    "algorithm/dpll.hpp",
    "algorithm/dimacs_loader.cpp",
    "algorithm/dimacs_loader.hpp",

    "tests/test_algorithm.cpp",
    "tests/test_loading.cpp"
]

exit_code = 0
for file in files:
    os.system(f"clang-format {file} > res.txt")
    if not filecmp.cmp(file, "res.txt"):
        print(f"file {file} dont satisfy clang-format")
        exit_code = 47
exit(exit_code)

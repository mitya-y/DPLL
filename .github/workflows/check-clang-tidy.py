import os

files = [
    "utils/dpll.cpp",
    "utils/dpll.hpp",
    "utils/dimacs_loader.cpp",
    "utils/dimacs_loader.hpp",

    "tests/test_algorithm.cpp",
    "tests/test_loading.cpp"
]

path = os.path.dirname(__file__)
print(f"PATH: {path}")

exit_code = 0
for file in files:
    dir = os.path.dirname(file)
    name = os.path.basename(file)
    os.system(f"cd {path}/{dir}")
    print(f"cd {path}/{dir}")
    os.system(f'clang-tidy {file} -extra-arg=-std=c++23 -p="../build" > res.txt')
    if os.stat("res.txt").st_size != 0:
        print(f"file {file} dont satisfy clang-format")
        exit_code = 47
os.system(f"cd {path}")
exit(exit_code)

import os

files = [
    "algorithm/dpll.cpp",
    "algorithm/dpll.hpp",
    "algorithm/dimacs_loader.cpp",
    "algorithm/dimacs_loader.hpp",

    "main.cpp"
]

exit_code = 0
for file in files:
    os.system(f'clang-tidy {file} -extra-arg=-std=gnu++2b -p="build" > res.txt')
    if os.stat("res.txt").st_size != 0:
        print(f"file {file} dont satisfy clang-format")
        exit_code = 47
exit(exit_code)

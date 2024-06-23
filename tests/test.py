import os
import sys
import subprocess
from pysat.formula import CNF

SOLVER = "./dpll"

def check_satisfy(file, answer):
    cnf = CNF(from_file = file)
    vars = {abs(var) : var >= 0 for var in map(int, answer.split())}
    assert all(any(not (int(v >= 0) ^ int(vars[abs(v)])) for v in clause) for clause in cnf.clauses)

def test_on_not_existing_file():
    result = subprocess.run([SOLVER, "not_existing.cnf"], capture_output=True, text=True)
    assert result.returncode != 0

def test_cnf_sat(file):
    print(f"Testing file: {file}")
    result = subprocess.run([SOLVER, file], capture_output=True, text=True)
    output = result.stdout.strip().split('\n')
    assert len(output) == 2
    assert output[0] == "SATISFIABLE"
    check_satisfy(file, output[1])

def test_cnf_unsat(file):
    print(f"Testing file: {file}")
    result = subprocess.run([SOLVER, file], capture_output=True, text=True)
    output = result.stdout.strip()
    assert output == "NOT SATISFIABLE"

def test_sat_solver(dataset_directory, satisfy):
    cnf_files = [f for f in os.listdir(dataset_directory) if f.endswith('.cnf')]
    for cnf_file in cnf_files:
        cnf_file_path = os.path.join(dataset_directory, cnf_file)
        if satisfy:
            test_cnf_sat(cnf_file_path)
        else:
            test_cnf_unsat(cnf_file_path)
        print("-" * 40)

if __name__ == "__main__":
    files = "files"
    if len(sys.argv) >= 2:
        SOLVER = sys.argv[1]
    if len(sys.argv) >= 3:
        files = sys.argv[2]
    test_on_not_existing_file()
    test_cnf_sat(f"{files}/example.cnf")

    test_sat_solver(f"{files}/light/sat", True)
    test_sat_solver(f"{files}/light/unsat", False)

    test_sat_solver(f"{files}/heavy/aim/sat", True)
    test_sat_solver(f"{files}/heavy/aim/unsat", False)

    test_sat_solver(f"{files}/heavy/jnh/sat", True)
    test_sat_solver(f"{files}/heavy/jnh/unsat", False)

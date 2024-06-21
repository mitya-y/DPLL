from os import listdir
from os.path import isfile, join
files = [f for f in listdir(".") if isfile(join(".", f))]

import os
dir_path = os.path.dirname(os.path.realpath(__file__))
from pathlib import Path
for file in files:
    f = open(file)
    if ".py" not in file and "Not sat" in f.read():
        Path(dir_path + '/' + file).rename(dir_path + "/unsat/" + file)

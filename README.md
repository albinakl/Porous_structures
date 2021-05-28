# Porous_structures
HPC final project repo

Data for the project is [here](https://drive.google.com/drive/u/4/folders/17-M5lLhykkmSsEyreI-F6MglTHKL-ou3)

Files in the repo:
* PorousStructures.cpp -- file with code of analysis of a porous structure in format of .map file

* filemapnames.txt -- file with all filenames of .map files

* readmapfiles.py -- code to create filemapnames.txt

* CALC.bash -- script for parallel runnihg on sandbox.zhores.net

* walks.ipynb -- reading and processing .av files

* slopes.txt -- output of walks.ipynb

* pore_distribution.ipynb -- reads the final outputs (slopes.txt and .txt files of  porous structures) to merge the data and generate predictions of diffusion coefficients

#!/bin/bash

#SBATCH --job-name=ABP4
#SBATCH -p cpu
#SBATCH --array=0-24
#SBATCH -N 1
#SBATCH --ntasks=4

module load compilers/gcc-7.3.1

srun -n 1 ./out $SLURM_ARRAY_TASK_ID -o outfilehere.txt

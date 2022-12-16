#!/bin/bash

module load intel/2020-u4
g++ -std=c++17 -fopenmp -O2 -o ff-omp ff-omp.cpp


#SBATCH --job-name= ff-omp
#SBATCH --partition=test
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=28
#SBATCH --mem=10000M
#SBATCH --array=1-5

export SEED="${SLURM_ARRAY_TASK_ID}"
export WIND=1

export OMP_NUM_THREADS="${SLURM_CPUS_PER_TASK}"
srun ./ff-omp

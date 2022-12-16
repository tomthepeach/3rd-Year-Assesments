#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <assert.h>
#include <mpi.h>
#include <cmath>
#include <unistd.h>

/* This code implements a simple C++ version of the forest fire model, as described in the week 9 video.
Recall that each point on the grid represents a possible site for a tree:
- 0: no tree
- 1: tree is alive
- 2: tree is burning
- 3: tree is burnt*/

// define arguments of the forest fire function
int forest_fire(int N, double p, int iproc, int nproc);

int main(int argc, char **argv)
{
	
	// initialise MPI
	MPI_Init(&argc, &argv);

	// Get the number of processes in MPI_COMM_WORLD
	int nproc;
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	// get the rank of this process in MPI_COMM_WORLD
	int iproc;
	MPI_Comm_rank(MPI_COMM_WORLD, &iproc);
				 
	// read the size of the (square) grid, the probability of filling a grid point with a tree and the random seed
	// check that we have 3 arguments (in addition to the program name)
	assert (argc == 4);
	 
	int N = atoi(argv[1]);
	double p = atof(argv[2]);
	int seed = atoi(argv[3]);
	
	// initialise the random number generator using a fixed seed for reproducibility
	// note: in real applications it may be better to e.g. use the system clock to initialise the random number generator
	srand(seed); 

	// start timer
	struct timespec start, finish;
	clock_gettime(CLOCK_MONOTONIC, &start);

	// print a message so we know the commandline arguments were processed correctly
	if (iproc == 0){
		std::cout << "Simulating forest fire with N = " << N << ", p = " << p << " and seed = " << seed << std::endl;  
	}
	// call the forest fire function
	int nsteps = forest_fire(N, p, iproc, nproc);
	
	// end timer
	clock_gettime(CLOCK_MONOTONIC, &finish);
	double time_elapsed = (finish.tv_sec - start.tv_sec);
	time_elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	if (iproc == 0){
		std::cout << "Simulation took " << time_elapsed << " s" << std::endl;
	}
				
	// finalise MPI
	MPI_Finalize();
				
	return 0;
}
	
// function which implements the forest fire model
int forest_fire(int N, double p, int iproc, int nproc){

	/* we want 2 grids - the currently active one, and the one from the previous time step -
		 we call those grid_new and grid_old
		 note: we could have generalised this to be a single 3 dimensional array/vector, with the 3rd index over time*/  
	std::vector<std::vector<int>> grid_old;
	// randomly fill the initial grid with live trees, using probability p
	for (unsigned int i=0;i<N;i++){
		grid_old.push_back(std::vector<int>());

		for (unsigned int j=0;j<N;j++){
			// generate a random floating point number between 0 and 1
			// note: random number generation is non-trivial - in real applications this approach may not be sufficiently random
			double rn = ((float)rand()/(float)(RAND_MAX));
			
			// if the random number is less than our probability p, we fill the site with a tree
			if (rn <= p){
				grid_old[i].push_back(1);
			}
			// otherwise, the site remains empty
			else{
				grid_old[i].push_back(0);
			}
			
		}
	}
	
	// set the top row of trees on fire
	for (unsigned int i=0;i<N;i++){
		if (grid_old[0][i] == 1) grid_old[0][i] = 2;
	}
	
	// initialise the new grid to an empty array
	std::vector < std::vector < int > > grid_new;
	for (unsigned int i=0;i<N;i++){
		grid_new.push_back(std::vector<int>());
		for (unsigned int j=0;j<N;j++){
			grid_new[i].push_back(0);
		}
	}
	
	// divide the loop over MPI tasks using slices
	int i0, i1;
	// first find the minimum number of columns each task should have 
	int dN = floor(N / nproc);
	// then work out the remainder of columns
	int Nr = N%nproc;

	// the first Nr tasks should have dN + 1 columns
	// the remaining tasks should have dN columns
	if (iproc <= Nr - 1){
		i0 = iproc * (dN + 1);
		i1 = i0 + dN + 1;
	}
	else if (iproc == Nr){
		i0 = iproc * (dN + 1);
		i1 = i0 + dN;
	}
	else{
		i0 = Nr * (dN + 1) + (iproc - Nr) * dN;
		i1 = i0 + dN;
	}
	std::cout << "MPI task " << iproc << " of " << nproc << " is responsible for " << i0 << "<=i<" << i1 << " (" << i1 - i0 << " columns)" << std::endl;
	
	// which strategy we will use to handle the communication of a 2D vector
	int strategy = 1;
	
	// for strategy 2:
	std::vector < int > grid_new_1d, grid_old_1d;
	if (strategy == 2){ 
		for (unsigned int i=0;i<N*N;i++){
			grid_old_1d.push_back(0);
			grid_new_1d.push_back(0);
		}
	}
	
	// loop over time - this loop will continue until there are no more burning trees
	int t = 0;
	bool burning_global = true;
	while (burning_global){
	
		// assume nothing is burning, unless we find otherwise below
		bool burning_local = false;
	 
		// loop over grid points
		for (unsigned int i=i0;i<i1;i++){
			for (unsigned int j=0;j<N;j++){
				// for testing the balance of computation and communication: sleep for 1 micro second
				//usleep(1);
			
				// first check if this is a previously burning tree, which should change from burning (2) to burnt (3)
				// note: this operation depends only on the tree itself, i.e. the data at this grid point only
				if (grid_old[i][j] == 2){
					grid_new[i][j] = 3;
					burning_local = true;
				}
				
				// now we check whether this tree is alive
				else if (grid_old[i][j] == 1){
					/* in this case, we need to check the status of its neighbours, paying attention to whether or not we are at the edge
						note: this operation depends on data from other cells          
						we first set each variable to false, to cover the case where we are at the edge of the cell*/
					bool left_burning = false;
					if (j > 0) left_burning = (grid_old[i][j-1] == 2);
						
					bool right_burning = false;
					if (j < N-1) right_burning = (grid_old[i][j+1] == 2);
					
					bool below_burning = false;
					if (i < N-1) below_burning = (grid_old[i+1][j] == 2);
					
					bool above_burning = false;
					if (i > 0) above_burning = (grid_old[i-1][j] == 2);
					
					// if any of the neighbours are burning, set this tree on fire
					if (left_burning || right_burning || above_burning || below_burning){
						grid_new[i][j] = 2;
					}
					else{
						grid_new[i][j] = 1;
					}
				}
				
				// if there is no tree or it is already burnt, the status remains the same
				else{
					grid_new[i][j] = grid_old[i][j];
				}
				
			}
		}
		
		// increase the time counter
		t++;
		
		// the current grid will become the old grid in the next step
		// communicate the grid
		if (nproc > 1){
			// approach 1: perform the communication separately for each column of the grid
			if (strategy == 1){
				for (unsigned int i=0;i<N;i++){
					int ierr = MPI_Allreduce(grid_new[i].data(), grid_old[i].data(), N, MPI_INTEGER, MPI_SUM, MPI_COMM_WORLD);
				}
			}
			// approach 2: copy the grid into a 1D vector and use a single MPI call
			else{
				int ii = 0;
				for (unsigned int i=0;i<N;i++){
					for (unsigned int j=0;j<N;j++){
						grid_new_1d[ii] = grid_new[i][j];
						ii++;
					}
				}      
				int ierr = MPI_Allreduce(grid_new_1d.data(), grid_old_1d.data(), N*N, MPI_INTEGER, MPI_SUM, MPI_COMM_WORLD);
				ii = 0;
				for (unsigned int i=0;i<N;i++){
					for (unsigned int j=0;j<N;j++){
						grid_old[i][j] = grid_old_1d[ii];
						ii++;
					}
				}    
			}       
			
			// set grid_new to zero for next iteration
			for (unsigned int i=0;i<N;i++){
				for (unsigned int j=0;j<N;j++){
					grid_new[i][j] = 0;
				}
			}
		}
		else{
			// we only have 1 MPI task, so we can copy as before
			grid_old = grid_new;  
		}
					
		// communicate the status of any burning trees
		if (nproc > 1){
			int ierr = MPI_Allreduce(&burning_local, &burning_global, 1, MPI_CXX_BOOL, MPI_LOR, MPI_COMM_WORLD);
		}
		else{
			burning_global = burning_local;
		}     
	} 
	
	if (iproc == 0){
		std::cout << "Fire burned out after " << t << " steps" << std::endl;
	}

	return t;

}



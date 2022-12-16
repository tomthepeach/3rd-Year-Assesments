#include <stdlib.h>
#include <iostream>
#include <vector>
#include <omp.h>
#include <fstream>
#include <string>

using std::vector, std::cout, std::endl, std::tuple, std::getenv, std::ofstream, std::string, std::stoi, std::srand;

/* This code implements a simple C++ version of the forest fire model, as described in the week 9 video.
Recall that each point on the grid represents a possible site for a tree:
- 0: no tree
- 1: tree is alive
- 2: tree is burning
- 3: tree is burnt*/


// function which implements the forest fire model
bool forest_fire(int N, double p, int &steps)
{
	/* 
	we want 2 grids - the currently active one, and the one from the previous time step -
	we call those grid_new and grid_old
	note: we could have generalised this to be a single 3 dimensional array/vector, with the 3rd index over time
	*/ 

	vector<vector<int>> grid_old;
	// randomly fill the initial grid with live trees, using probability p
	for (unsigned int i=0; i<N; i++)
	{
		grid_old.push_back(vector<int>());

		for (unsigned int j=0;j<N;j++)
		{
	    	// generate a random floating point number between 0 and 1
	    	// note: random number generation is non-trivial - in real applications this approach may not be sufficiently random
	    	double rn = ((float)rand()/(float)(RAND_MAX));
	
			// if the random number is less than our probability p, we fill the site with a tree
	    	if (rn <= p) grid_old[i].push_back(1);

	    	// otherwise, the site remains empty
	    	else grid_old[i].push_back(0);
		}
	}

	// set the top row of trees on fire
	for (unsigned int i=0; i<N; i++) if (grid_old[0][i] == 1) grid_old[0][i] = 2;

	// initialise the new grid to an empty array
	vector<vector<int>> grid_new;

	for (unsigned int i=0;i<N;i++)
	{
		grid_new.push_back(vector<int>());
		for (unsigned int j=0; j<N; j++) grid_new[i].push_back(0);
	}

	// loop over time - this loop will continue until there are no more burning trees
	int t = 0;
	bool burning = true;
	while (burning)
	{
		// assume nothing is burning, unless we find otherwise below
		burning = false;

		// we want the following variables to be private:
		// i, j, left_burning, right_burning, above_burning and below_burning
		// and the following to be shared:
		// n, grid_old, grid_new, burning
		// by default, loop iteration variables and variables declared locally within the parallel loop are private
		// -> this covers all the private variables
		// variables declared outside the loop are shared
		// -> this covers all the shared variables
		// therefore we do not need to be explicit about any of the variables
		// loop over grid points
		for (unsigned int i=0; i<N; i++)
		{
			for (unsigned int j=0; j<N; j++)
			{
		    	// first check if this is a previously burning tree, which should change from burning (2) to burnt (3)
		    	// note: this operation depends only on the tree itself, i.e. the data at this grid point only
		    	if (grid_old[i][j] == 2)
				{
		    		grid_new[i][j] = 3;
		    		burning = true;
		    	}

		    	// now we check whether this tree is alive
				else if (grid_old[i][j] == 1)
				{
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
					if (left_burning || right_burning || above_burning || below_burning) grid_new[i][j] = 2;

					else grid_new[i][j] = 1; // this probs aint needed
				}

				// if there is no tree or it is already burnt, the status remains the same
				else grid_new[i][j] = grid_old[i][j];
			}
		}
	
		// increase the time counter
		t++;
	
		// the current grid will become the old grid in the next step
		grid_old = grid_new;
	}

	bool bottom_reached = false;
	for (unsigned int i=0; i<N; i++) if (grid_old[N-1][i] == 3) bottom_reached = true;
	steps = t;
	return bottom_reached;	
}


int main()
{
	// initialise the random number generator using a fixed seed for reproducibility
	string strseed = getenv("SEED");
	// string strseed = "1"; // for testing
	int intseed = stoi(strseed);
	srand(intseed);

	ofstream output;
	string filename = "data/" + strseed + "_output.csv";
    output.open(filename);
	output << "n" <<','<< "p" <<','<< "n_steps" <<','<< "time" <<','<< "bottom_reached" << "\n";

	//decided to parallelise this loop instead of the forest fire loop to avoid lots of race conditions, however there is one with the output file
	#pragma omp parallel for
	for (int N=100; N<=1000; N+=300)
	{
		for (float p=0.01; p<1.0; p+=0.01)
		{
			double startTime = omp_get_wtime();
			
			int nsteps;
			bool bottom_reached = forest_fire(N, p, nsteps);

			double time_elapsed = omp_get_wtime() - startTime;
			#pragma omp critical
			{
				output << N  <<','<< p <<','<< nsteps <<','<< time_elapsed <<','<< bottom_reached <<"\n";
			}
		}
	}

	output.close();
	return 0;
}

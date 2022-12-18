#include <vector>
#include <stdlib.h>

using std::vector, std::srand;

/* This code implements a simple C++ version of the forest fire model, as described in the week 9 video.
Recall that each point on the grid represents a possible site for a tree:
- 0: no tree
- 1: tree is alive
- 2: tree is burning
- 3: tree is burnt*/

// function which implements the forest 	fire model, wind direction is optional choose from: 0(no wind) N, S, E, or W 
bool forest_fire(int N, double p, int &steps,  int Nthreads, char wind_direction= '0')
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

		#pragma omp parallel for num_threads(Nthreads) reduction(||:burning) 
		for (unsigned int i=0; i<N; i++)
		{
			for (unsigned int j=0; j<N; j++)
			{
				// // intialise local version of grid_old and to avoid race conditions
				// //this should use more memory but the old version barely used any and hopefully this should run faster
				// vector<vector<int>> grid_old(grid_old);

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
					bool wind_burning = false;

					bool west_burning = false;
					if (j > 0) west_burning = (grid_old[i][j-1] == 2);

					bool east_burning = false;
					if (j < N-1) east_burning = (grid_old[i][j+1] == 2);

					bool south_burning = false;
					if (i < N-1) south_burning = (grid_old[i+1][j] == 2);

					bool north_burning = false;
					if (i > 0) north_burning = (grid_old[i-1][j] == 2);

					// Implementing wind
					if (wind_direction != '0')
					{
						if ( i > 1 && wind_direction == 'S') wind_burning = (grid_old[i-2][j] == 2);
						if ( j > 1 && wind_direction == 'E') wind_burning = (grid_old[i][j-2] == 2);
						if ( j < N-2 && wind_direction == 'W') wind_burning = (grid_old[i+2][j] == 2);
						if ( i < N-2 && wind_direction == 'N') wind_burning = (grid_old[i+2][j] == 2);
					}

					// if any of the neighbours are burning / wind is blowing emebers set this tree on fire
					if (west_burning || east_burning || north_burning || south_burning || wind_burning) grid_new[i][j] = 2;

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

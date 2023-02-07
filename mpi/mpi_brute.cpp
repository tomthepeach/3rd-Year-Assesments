#include <vector>
#include <mpi.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>

using std::vector, std::cout, std::endl, std::ifstream, std::string, std::min, std::max, std::accumulate;


// I'm aware that some of you are uncertain about how the MPI implementation should work for the assessed exercise, 
// in particular why reduction is a good approach for this problem, and the impact on load balancing.
// We went through this in the office hours, but I know that not all of you were able to attend,
// so in the interest of fairness I've put together a few slides explaining in a bit more detail how this works for the
// brute force approach.  A very similar approach can be used for the cell list 
// (i.e. the same reduction approach can be used, but the loop structure and load balancing will be different).
// There is no video to accompany it, but hopefully the slides are complete enough as a standalone - 
// I have added these to week 14 on blackboard.  I hope you find this useful!

// In addition, the smallest problem size should run very quickly, so if you are uncertain about whether or not you
// are getting the correct answers and feel more comfortable writing in python, I recommend you implement the simple
// brute force approach in python as a sanity check.  This should not take many lines of code.

// Finally, to clarify the mark scheme - this has been designed so that increasing levels of complexity are needed to
// achieve a higher grade, so in the first instance you should start with the simplest approach, prioritising correctness,
// and only implement a more complicated approach once you are sure this is correct.  The hardest part of this assessment
// is the cell list approach, however this is only necessary if you are aiming to achieve a 1st. In order to get a 2:1
// you need to implement either a cell list approach or a brute force approach with optimal MPI load balancing.  
// In other words, you can still achieve a good 2:1 without even attempting the cell list approach.








// function to read in a list of 3D coordinates from an .xyz file
// input: the name of the file
vector< vector<double> > read_xyz_file(string filename, int& N, double& L)
{

	cout << "Reading in xyz coordinates from " << filename << endl;

	// open the file
	ifstream xyz_file(filename);

	// read in the number of atoms
	xyz_file >> N;
	
	// read in the cell dimension
	xyz_file >> L;
	
	// now read in the positions, ignoring the atomic species
	vector< vector<double> > positions;
	vector<double> pos = {0, 0, 0};
	string dummy;

	for (int i=0; i<N; i++)
	{
		xyz_file >> dummy >> pos[0] >> pos[1] >> pos[2];
		positions.push_back(pos);           
	}
	
	// close the file
	xyz_file.close();
	cout << "Read in " << N << " atoms" << endl;
	return positions;
	
}

double calc_r2 (vector<double> v1, vector<double> v2)
{
			double dx = v1[0] - v2[0];
			double dy = v1[1] - v2[1];
			double dz = v1[2] - v2[2];
			
			// compute the distance
			return dx*dx + dy*dy + dz*dz;
}


void neighbor_stats(vector<int> neighbors, int& max, int& min, double &avg)
{
	max = *max_element(neighbors.begin(), neighbors.end());
	min = *min_element(neighbors.begin(), neighbors.end());
	avg = accumulate(neighbors.begin(), neighbors.end(), 0.0) / neighbors.size();
}


vector<int> neighbors_bruteforce(int N, double rc, vector< vector<double> > positions, int iproc, int nproc)
{
	vector<int> neighbors(N, 0);

	for (int i=0+iproc; i<N; i+=nproc)
	{
		for (int j=i+1; j<N; j++)
		{
			// compute the distance between the two particles
			double r2 = calc_r2(positions[i], positions[j]);
			
			// check if the distance is less than the cutoff
			if (r2 < rc*rc)
			{
				neighbors[i]++;
				neighbors[j]++;
			}
			
		}
	}
	return neighbors;
}



int main()
{
	// read in the xyz coordinates
	int N;
	double L;
	MPI_Init(nullptr, nullptr);

	// Get the number of processes in MPI_COMM_WORLD
	int nproc;
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	// get the rank of this process in MPI_COMM_WORLD
	int iproc;
	MPI_Comm_rank(MPI_COMM_WORLD, &iproc);

	vector< vector<double> > positions;

	if (iproc == 0)
	{
		cout << "Running on " << nproc << " processes" << endl;
		cout << "Reading in xyz coordinates" << endl;
		positions = read_xyz_file("argon120.xyz", N, L);
		MPI_Bcast(positions.data(), N, MPI_INT, 0, MPI_COMM_WORLD);
	}

	// initialise MPI
	vector<int>	neighbors_sum(N, 0);

	
	vector<int> neighbors_part = neighbors_bruteforce(N, 8.0, positions, iproc, nproc);

	MPI_Reduce(neighbors_part.data(), neighbors_sum.data(), N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	// finalise MPI

	// compute the min, max and average number of neighbors
	int max, min;
	double avg;
	neighbor_stats(neighbors_sum, max, min, avg);

	cout << "Max number of neighbors: " << max << endl;
	cout << "Min number of neighbors: " << min << endl;
	cout << "Average number of neighbors: " << avg << endl;
	MPI_Finalize();

	return 0;
}




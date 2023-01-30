#include <vector>
#include <mpi.h>
#include <stdlib.h>
#include <fstream>


// function to read in a list of 3D coordinates from an .xyz file
// input: the name of the file
std::vector< std::vector<double> > read_xyz_file(std::string filename, int& N, double& L)
{

	std::cout << "Reading in xyz coordinates from " << filename << std::endl;

	// open the file
	std::ifstream xyz_file(filename);

	// read in the number of atoms
	xyz_file >> N;
	
	// read in the cell dimension
	xyz_file >> L;
	
	// now read in the positions, ignoring the atomic species
	std::vector< std::vector<double> > positions;
	std::vector<double> pos = {0, 0, 0};
	std::string dummy;

	for (int i=0; i<N; i++)
	{
		xyz_file >> dummy >> pos[0] >> pos[1] >> pos[2];
		positions.push_back(pos);           
	}
	
	// close the file
	xyz_file.close();
	return positions;
	
}

void neighbors_bruteforce(int N, double L, double rc)
{
	for (int i=0; i<N; i++)
	{
		for (int j=i+1; j<N; j++)
		{
			if (i==j) continue;

			else
			{
				// compute the distance between the two particles
				double dx = positions[i][0] - positions[j][0];
				double dy = positions[i][1] - positions[j][1];
				double dz = positions[i][2] - positions[j][2];
				
				// compute the distance
				double r = sqrt(dx*dx + dy*dy + dz*dz);
				
				// check if the distance is less than the cutoff
				if (r < rc) count += 1;
			}
		}
	}
}

void neigbors_cellist()
{

}


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
	
	// read in the xyz coordinates
	int N;
	double L;
	std::vector< std::vector<double> > positions = read_xyz_file("argon120.xyz", N, L);

	// finalise MPI
	MPI_Finalize();
				
	return 0;
}




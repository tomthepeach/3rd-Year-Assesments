#include <iostream>
#include <omp.h>
#include <fstream>
#include <string>
#include "forest_fire.hpp"

using std::cout, std::endl, std::getenv, std::ofstream, std::string, std::stoi;



int main()
{
	// initialise the random number generator using a fixed seed for reproducibility
	string strseed = getenv("SEED");
	// string strseed = "1"; // for testing
	int intseed = stoi(strseed);
	srand(intseed);
	char wind_direction = getenv("WIND")[0];
	ofstream output;
	string filename = strseed + "_" + wind_direction + "_output.csv";
    output.open(filename);
	output << "n,p,n_steps,time,bottom_reached\n";

	//decided to parallelise this loop instead of the forest fire loop to avoid lots of race conditions, however there is one with the output file
	#pragma omp parallel for
	for (int N=1; N<=100; N++)
	{
		for (float p=0.01; p<1.0; p+=0.01)
		{
			double startTime = omp_get_wtime();
			
			int nsteps;
			bool bottom_reached = forest_fire(N, p, nsteps, wind_direction);

			double time_elapsed = omp_get_wtime() - startTime;
			#pragma omp critical
			{
				output << N  <<','<< p <<','<< nsteps <<','<< time_elapsed <<','<< bottom_reached << "\n";
			}
		}
	}

	output.close();
	return 0;
}
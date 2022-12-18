#include <iostream>
#include <omp.h>
#include <fstream>
#include <string>
#include <chrono>

#include "forest_fire_performance.hpp"

using std::cout, std::endl, std::getenv, std::ofstream, std::string, std::stoi;



int main()
{
	auto start = std::chrono::high_resolution_clock::now();
	// initialise the random number generator using a fixed seed for reproducibility
	string strseed = getenv("SEED");
	// string strseed = "1"; // for testing
	int intseed = stoi(strseed);
	srand(intseed);
	char wind_direction = getenv("WIND")[0];
	ofstream output;
	string filename =  strseed + "_" + wind_direction + "_output.csv";
    output.open("per_data/" +filename);
	output << "n,p,n_steps,time,bottom_reached,n_threads\n";

	
	for (int threads=1; threads <= omp_get_max_threads(); threads++)
	{

		#pragma omp parallel for num_threads(threads) 
		for (int N=1; N<=100; N++) 
		{
			for(auto p=0.1; p<1.0; p+=0.1)
			{
				auto startTime = omp_get_wtime();
				int nsteps;
				bool bottom_reached = forest_fire(N, p, nsteps, wind_direction, threads);
				auto finishTime = omp_get_wtime();

				double time_elapsed = (finishTime-startTime);
				output << N  <<','<< p <<','<< nsteps <<','<< time_elapsed <<','<< bottom_reached << ',' <<threads <<"\n";
			}
		}

	}
	output.close();

    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();

	return 0;
}
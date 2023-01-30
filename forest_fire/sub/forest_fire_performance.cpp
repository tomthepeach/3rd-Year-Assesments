#include <iostream>
#include <omp.h>
#include <fstream>
#include <string>
#include <chrono>

#include "forest_fire.hpp"

using std::cout, std::endl, std::getenv, std::ofstream, std::string, std::stoi;



int main()
{
	auto start = std::chrono::high_resolution_clock::now();
	// initialise the random number generator using a fixed seed for reproducibility
	char wind_direction ='0';
	ofstream output;
	string filename = "per_output.csv";
    output.open("per_data/" +filename);
	output << "n_threads,iteration,time\n";

	for (int threads=2; threads <= omp_get_max_threads(); threads++)
	{

		#pragma omp parallel for schedule(dynamic, 1) num_threads(threads)
		for (int reps = 0; reps < 100; reps++)
		{
			auto startTime = omp_get_wtime();
			for (int N=1; N<=100; N++) 
			{
				for(auto p=0.1; p<1.0; p+=0.1)
				{
					int nsteps;
					bool bottom_reached = forest_fire(N, p, nsteps, wind_direction, threads);

				}
			}
			auto finishTime = omp_get_wtime();
			double time_elapsed = (finishTime-startTime);
			#pragma omp critical
			{
				output <<threads << ','  << reps << ',' << time_elapsed <<"\n";
			}
		}



	}
	output.close();

    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();

	return 0;
}
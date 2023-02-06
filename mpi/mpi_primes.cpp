#include <iostream>
#include <iomanip>
#include <mpi.h>

int partialCountPrimes(int n, int procId, int numProcs)
{
	int count = 0;

	// Here is where the work is divided between the processes.
	for (int i = 2 + procId; i <= n; i += numProcs)
	{
		// Check to see if i is prime
		bool prime = true;
		for (int j = 2; j < i; ++j) if (i % j == 0) prime = false;
		count += prime;
	}
	return count;
}


int main()
{
	int nLo = 1;
	int nHi = 131072;
	int nFactor = 2;
	int numProcs;
	int procId;

	MPI_Init(nullptr, nullptr);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &procId);

	if (procId == 0) // ’Manager’ process only
	{
		cout << "Counting primes with " << numProcs << " processes."
		<< endl;
		cout << " N Primes Time" << endl;
	}

	int n = nLo;
	while (n <= nHi)
	{
		double wTime;
		if (procId == 0) wTime = MPI_Wtime();

		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

		int partialCount = partialCountPrimes(n, procId, numProcs);
		int fullCount = 0;
		MPI_Reduce(&partialCount, &fullCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

		if (procId == 0)
		{
			wTime = MPI_Wtime() - wTime;
			cout << std::fixed << std::setprecision(3);
			cout << std::setw(8) << n << std::setw(8) << fullCount << std::setw(8) << wTime << endl;

			n *= nFactor;
		}
	MPI_Finalize();
	return 0;
}
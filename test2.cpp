#include <iostream>
#include <omp.h>

using namespace std;

int main()
{

int count = 0; 
omp_set_num_threads(4);

#pragma omp parallel for schedule(static, 3)
for (int j = 2; j < 100; ++j)
{

    if(j==18){ cout << "Iter18_thread: " << omp_get_thread_num() << endl; }
    if (omp_get_thread_num() == 2){ count += 1;}
}
cout << count << endl;
}
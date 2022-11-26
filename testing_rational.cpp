#include "rational.hpp"

long long gcf(long long a, long long b)
{
// If you don’t want negative answers, don’t give me negative arguments!
long long c = a % b;
if (c == 0){ return b;} 
return gcf(b, c);
}

int main()
{

    Rational frac1 = Rational(1,2);
    Rational frac2 = Rational(1,2);

    Rational frac3 = frac1 + 1;
    std::cout << frac3 <<std::endl;

    frac3 = frac1 - 1;
    std::cout << frac3 <<std::endl;

    frac3 = frac1 * frac2;
    std::cout << frac3 <<std::endl;

    frac3 = frac1 / frac2;
    std::cout << frac3 <<std::endl;

    

    std::cout << (frac1 == frac2) <<std::endl;
    std::cout << (frac1 != frac2) <<std::endl;
    std::cout << (frac1 < frac2) << std::endl;
    std::cout << (frac1 > frac2) << std::endl;
    std::cout << (frac2 > frac1) << std::endl;


    return 0;
}
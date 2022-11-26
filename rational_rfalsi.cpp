#include <iostream>
#include <cmath>
#include "rational.hpp"

using std::cout, std::endl;

Rational myQuadratic(Rational x)
{
    return x * x - 2ll;
}



bool differentSign(Rational rhs, Rational lhs)
{
    return std::signbit(rhs.num_) != std::signbit(lhs.num_);
}


bool rootExists(Rational (*f)(Rational), Rational lower, Rational upper)
{
    return differentSign(f(lower), f(upper));
}


Rational findKnownRoot(Rational (*f)(Rational), Rational lower, Rational upper, Rational tolerance)
{
    // First check to see whether 'lower' or 'upper' are already roots of the equation.
    Rational fLower = f(lower);
    if (fLower.abs() <= tolerance)
    {
        return lower;
    }

    Rational fUpper = f(upper);
    if (fUpper.abs() <= tolerance)
    {
        return upper;
    }

    // Iteratively find the new guess, and the value of the function there.
    // Check if that is a root and, if not, decide whether it should become the
    // new upper, or lower bound.
    Rational guess = (lower * fUpper - upper * fLower) / (fUpper - fLower);
    Rational fGuess = f(guess);
    
    while (fGuess.abs() > tolerance)
    {
        if (differentSign(fGuess, fLower))
        {
            upper = guess;
            fUpper = fGuess;
        }
        else
        {
            lower = guess;
            fLower = fGuess;
        }
        cout << "guess: " << guess<< endl;
        cout << "f(guess): " << fGuess << endl;
        guess = (lower * fUpper - upper * fLower) / (fUpper - fLower);
        fGuess = f(guess);
    }

    return guess;
}


Rational findRoot(Rational (*f)(Rational), Rational lower, Rational upper, Rational tolerance = Rational(1ll, 100000000ll))
{
    if (rootExists(f, lower, upper))
    {
        return findKnownRoot(f, lower, upper, tolerance);
    }

    std::cout << "f(lower) and f(upper) have the same sign. Root might not exist in [lower, upper]." << std::endl;
    exit(0);
}


int main()
{
    using std::cout, std::endl;

    cout << "Root of x^2 - 2 is :" << endl;
    Rational sol = findRoot(myQuadratic, Rational(0ll,1ll), Rational(2ll,1ll));
    cout << sol << endl;
    return EXIT_SUCCESS;
}
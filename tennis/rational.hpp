//C++ 20
#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>
using namespace std::rel_ops;

class Rational
{
    public:
    long long num_, div_;

    Rational(long long num, long long div)
    {
        if (div == 0){ throw std::domain_error{"Division by zero not allowed"}; }
        div_ = div;
        num_ = num;
        simplify();
    }

    const Rational abs() const
    {
        return Rational{std::abs(num_), std::abs(div_)};
    }

    const Rational& operator=(const Rational& rhs)
    {
        num_ = rhs.num_;
        div_ = rhs.div_;
        return *this;
    }

    const Rational& operator+=(const Rational& rhs)
    {
        num_ = num_ * rhs.div_ + div_ * rhs.num_;
        div_ *= rhs.div_;
        return simplify();
    }

    const Rational& operator-=(const Rational& rhs)
    {
        num_ = num_ * rhs.div_ - div_ * rhs.num_;
        div_ *= rhs.div_;
        return simplify();
    }

    const Rational& operator*=(const Rational& rhs)
    {
        num_ *= rhs.num_;
        div_ *= rhs.div_;
        return simplify();
    }

    const Rational& operator/=(const Rational& rhs)
    {
        if (rhs.num_ == 0)
            throw std::domain_error{"Division by zero not allowed"};
        return *this *= rhs.inverse();
    }

    bool operator==(const Rational&) const = default;

    friend bool operator<(const Rational& lhs, const Rational& rhs);

    friend std::ostream& operator<<(std::ostream &stream, Rational &frac);

    private:

    const long long gcf(long long a, long long b)
    {
    // If you don’t want negative answers, don’t give me negative arguments!
    const long long c = a % b;
    if (c == 0){ return b;} 
    return gcf(b, c);
    }

    const Rational& simplify()
    // function to simplify the fraction and correct negative divisors
    {
        if (div_ < 0) 
        { 
            num_ = -num_;
            div_ = -div_;
        }

        const long long factor = gcf(num_, div_);
        num_ = num_/factor;
        div_ = div_/factor;

        return *this;
    }

    const Rational inverse() const
    // function to return inverse of the fraction
    {
        return Rational(div_, num_);
    }
};

// operators below are defined outside the class and depend on the assignment version
const Rational operator+(const Rational& lhs, const Rational& rhs)
{
    return Rational{lhs} += rhs;
}

const Rational operator+(const Rational& lhs, const long long rhs)
{
    return Rational{lhs} + Rational(rhs,1);
}

const Rational operator-(const Rational& lhs, const Rational& rhs)
{
    return Rational{lhs} -= rhs;
}

const Rational operator-(const Rational& lhs, const long long rhs)
{
    return Rational{lhs} - Rational(rhs,1);
}

const Rational operator*(const Rational& lhs, const Rational& rhs)
{
    return Rational{lhs} *= rhs;
}

const Rational operator*(const Rational& lhs, const long long rhs)
{
    return Rational{lhs} *  Rational(rhs, 1);

}

const Rational operator/(const Rational& lhs, const Rational& rhs)
{
    return Rational{lhs} /= rhs;
}

const Rational operator/(const Rational& lhs, const long long rhs)
{
    return Rational{lhs} * Rational(1, rhs);
}

std::ostream& operator<<(std::ostream &stream, Rational &frac) 
{
    std::string str = std::to_string(frac.num_) + "/" + std::to_string(frac.div_);
    return stream << str;
}
// Other comaparison operators are automatically derived from the <  and == operators using the rel_ops functionality in the utility header
bool operator<(const Rational& lhs, const Rational& rhs)
{
    return (lhs.num_*rhs.div_ < lhs.div_*rhs.num_);
}
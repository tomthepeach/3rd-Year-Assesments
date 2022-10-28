class Rational
{
    public:

    Rational(int num, int div)
    {
        int factor = gcf(num, div);
        num_ = num/factor;
        div_ = div/factor;
    }

    Rational operator+(const Rational& rhs) const
    {
        return Rational(num_ + rhs.num_, div_ + rhs.div_);
    }

    Rational operator*(const Rational& rhs) const
    {
        
    }

    private:

    int num_;
    int div_;

    int gcf(int a, int b)
    {
    // If you don’t want negative answers, don’t give me negative arguments!
    int c = a % b;
    if (c == 0){ return b;} 
    return gcf(b, c);
    }

};




#include <iostream>
#include <vector>
using namespace std;

class Dart
{
public:
    Dart(int modifier, int number):
    modifier_(modifier),
    number_(number)
    {
    }

    int score() const
    {
        return modifier_ * number_;
    }

private:
    int modifier_;
    int number_;
};

int scoreThrow(const vector<Dart>& darts)
{
    return darts[0].score() + darts[1].score() + darts[2].score();
}

int main()
{
    vector<Dart> darts {Dart(3, 20), Dart(1, 20), Dart(1, 5)};
    cout << scoreThrow(darts) << endl;
    return EXIT_SUCCESS;
} 

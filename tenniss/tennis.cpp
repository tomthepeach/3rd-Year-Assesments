#include <iostream>
#include <string>

using namespace std;


class Tennis_match
{
public:
    Tennis_match(string player_1, string player_2):
    
        p1_(player_1),
        p2_(player_2)
    {
    }

    void update_score(int who)
    {
        cout << "score updated" << endl;
        display_board();
    }

    void reset_scores()
    {
        cout << "score reset" << endl;
    }

private:

    int score_1 = 0;
    int score_2 = 0;
    string p1_;
    string p2_;

    void display_board()
    {   
        cout << "Score:" << endl;
        cout << "Player 1:  " << " Player 2" << endl;
        cout << "(" << p1_ << ")  "<< "  (" << p2_ << ")"<< endl;
        cout << score_1 << " - " << score_2 << endl;
    }
};

int main()
{ 

    Tennis_match a("Steve", "Laybes");
    a.update_score(1);

    return 0;
} 
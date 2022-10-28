#include <iostream>
#include <string>
#include <map>

using namespace std;


class Tennis_game
{
public:
    Tennis_game(string player_1, string player_2):
    
        p1_(player_1),
        p2_(player_2)
    {
    scores[0], scores[1] = 0;
    advantage[0], advantage[1] = 0;
    }

    void update_score(bool who)
    {

        if (scores[0] == scores[1] == 40){deuce(who);}
        else if (scores[who] == 40) {}
        if (scores[who] == 30){scores[who] += 10;}

        else{scores[who] += 15;}
        
        cout << "score updated" << endl;
        display_board();
    }

    void reset_scores()
    {
        switch(serving)
        cout << "score reset" << endl;
    }

private:

    map<bool, int> scores;
    const string p1_;
    const string p2_;
    bool serving = 0;
    map<bool, int> advantage;
    

    void display_board()
    {   
        cout << "Score:" << endl;
        cout << "Player 1:  " << " Player 2" << endl;
        cout << "(" << p1_ << ")  "<< "  (" << p2_ << ")"<< endl;
        cout << scores[0] << " - " << scores[1] << endl;
    }

    void deuce(bool who)
    {
        advantage[who] += 1;
        if (advantage[who] > 1){ end_game(); }
        if (advantage[0] && advantage[1] == 1){ advantage[0], advantage[1] = 0; }
    }

    void end_game()
    {

    }
};

int main()
{ 

    Tennis_game a("Steve", "Laybes");
    a.update_score(1);

    return 0;
} 
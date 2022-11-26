#include <iostream>
#include <string>
#include <map>

using namespace std;

// Does what it says on the tin, used to test the tennis class
bool random_bool() 
{
    return (rand() % 2 == 0);
}

class Tennis_game
{
public:
    Tennis_game(string player_1, string player_2, bool server = 0)
    {
    players[0] = player_1;
    players[1] = player_2;
    scores[0], scores[1] = 0;
    announce_score(0);
    display_board(server);
    }

    void update_score(bool who)
    {
        if (!deuce)
        {
            if (scores[who] >= 30){ scores[who] += 10; }
            else{scores[who] += 15;}
            if (scores[who] == 50)
            {
                end_game(who); 
            }
            if ((scores[0] == 40) && (scores[1] == 40)){ deuce = true; }
        }
        else
        {
            if (deuce_score[who] == 1){ end_game(who); }
            else{ deuce_score[who] += 1; }

            if (deuce_score[0] && deuce_score[1])
            {
                deuce_score[0] = 0;
                deuce_score[1] = 0; 
            }
        }
        announce_score(who);
        display_board(who);
    }

    void reset_scores()
    {
        scores[0] = 0;
        scores[1] = 0;
        deuce_score[0] = 0;
        deuce_score[1] = 0;
        deuce = false;
        std::swap(serving[0], serving[1]);
        cout << "Score reset\n" << endl;
    }


private:

    map<bool, int> scores;
    map<bool, string> players;
    map<bool, bool> deuce_score = { {0, 0}, {1, 0} };
    map<int, string> score_names = {{0, "love"}, {15, "fifteen"}, {30, "thirty"}, {40, "forty"}};
    map<bool, bool> serving = { {0, 1}, {1, 0} };
    bool deuce = false;
    
    const void announce_score(bool who)
    {
        if (deuce) 
        { 
            if(deuce_score[who] == 1){ cout << "Umpire: 'Advantage " << players[who] << "!'"<< endl; }
            else{ cout << "Umpire: 'Deuce!'\n" << endl; }
        }
        else if (scores[0] == scores[1]){ cout << "Umpire: '" << score_names[scores[0]] << "-all!'\n" << endl; }
        else { cout << "Umpire: '" << score_names[scores[serving[who]]] << "-" << score_names[scores[serving[!who]]] <<"!'\n"<< endl; }
    }

    const void display_board(bool who)
    {   

        cout << "Score:" << endl;
        cout << "Player 1:  " << " Player 2:" << endl;

        // print out a star next to the players name if they are serving
        cout << "(" << players[0] << ")" << (serving[0] ? "*" : "  ") << (serving[1] ? "*" : "  ") << "("<< players[1] << ")" << endl;
        // Hackiest code I've ever written, please don't judge me Alan

        if (deuce_score[0] == 1){ cout <<"   adv    -\n" << endl; }

        else if (deuce_score[1]== 1){ cout <<"         -    adv\n" << endl; }

        else{ cout <<"   " << scores[0] << "    -    " << scores[1] << "\n" << endl; }
    }

    const void end_game(bool who)
    {
        cout << "Umpire: 'Game over, " << players[who] << " wins!'\n" << endl;
        reset_scores();
        return;
    }
};

int main()
{ 

    Tennis_game a("Steve", "Laybes");
    for (int i = 0; i < 50; i++)
    {
        a.update_score(random_bool());
    }

    return 0;
} 
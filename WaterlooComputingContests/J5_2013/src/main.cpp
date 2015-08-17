#include <iostream>

using namespace std;

#define NUMTEAMS 4
#define NUMMATCHES 6

void setMatch(int* matches, int team1, int team2, int team1_score, int team2_score);
bool isValidMatch(int* matches, int team1, int team2);

int main() {
  int favorite_team = 0;
  cout << "which is your favorite team? (1, 2, 3, or 4)" << endl;
  cin >> favorite_team;
  while(favorite_team < 1 || favorite_team > 4) {
    cout << "the number you gave is out of range" << endl;
    cout << " which is your favorite team? (1, 2, 3, or 4)" << endl;
    cin >> favorite_team;
  }

  int num_matches_already_played = 0;
  cout << "how many matches have already been played? (0-5)" << endl;
  cin >> num_matches_already_played;
  while(num_matches_already_played < 0 || num_matches_already_played > 5) {
    cout << "the number you gave is out of range" << endl;
    cout << "how many matches have already been played? (0-5)" << endl;
    cin >> num_matches_already_played;
  }

  int matches[NUMTEAMS * NUMTEAMS];
  for(int i = 0, length =  NUMTEAMS * NUMTEAMS; i < length; i++) {
    matches[i] = 0;
  }

  for(int i = 0; i < num_matches_already_played; i++) {
    cout << "enter the game results for game " << i + 1 << endl;
    cout << "([first team] [second team] [first team score] [second team score])" << endl;
    int team1 = 0;
    int team2 = 0;
    int team1_score = 0;
    int team2_score = 0;

    cin >> team1;
    cin >> team2;
    cin >> team1_score;
    cin >> team2_score;

    bool valid_team_input = (team1 >= 1 && team1 <= 4 && team2 >= 1 && team2 <= 4 && team1 < team2);
    bool valid_team_score_input = (team1_score >= 0 && team2_score >= 0);
    bool valid_match = isValidMatch(matches, team1, team2);

    while(!valid_team_input || !valid_team_score_input || !valid_match) {
      cout << "invalid input, please re-enter the game results for game " << i + 1 << endl;
      cout << "([first team] [second team] [first team score] [second team score])" << endl;

      cin >> team1;
      cin >> team2;
      cin >> team1_score;
      cin >> team2_score;

      valid_team_input = (team1 >= 1 && team1 <= 4 && team2 >= 1 && team2 <= 4 && team1 < team2);
      valid_team_score_input = (team1_score >= 0 && team2_score >= 0);
      valid_match = isValidMatch(matches, team1, team2);
    }

    setMatch(matches, team1, team2, team1_score, team2_score);
  }

  int num_matches_left = NUMMATCHES - num_matches_already_played;

  //possible_outcomes == 3 to the power of num_matches_left (3 possible outcomes per game for the number of matches left)
  int possible_outcomes = 1;
  for(int i = 0; i < num_matches_left; i++) {
    possible_outcomes *= 3;
  }

  //calculate outcomes in which x team wins
  int favorite_team_points = 0;
  for(int i = 0; i < NUMTEAMS; i++) {
    favorite_team_points += matches[(favorite_team - 1) + i * NUMTEAMS];
  }

  if(favorite_team_points == 9) {
    cout << "chances of your favorite team (" << favorite_team << ") winning is " << possible_outcomes << "/" << possible_outcomes;
  }
  else if(favorite_team_points == 7) { //can tie with team favorite tied with
    //if the team favorite team tied with tied or lost any other games, favorite team wins all possibilities

    //if the team favorite team tied with won other 2 games, they are tied with them in every case

  }
  else if(favorite_team_points == 6) {
    //check team they haven't played or lost to to see if they can win
    int team_yet_to_play = 0;
    for(int i = 0; i < NUMTEAMS; i++) {
      if(matches[(favorite_team - 1) + i * NUMTEAMS] == 0) {
        if(matches[i + (favorite_team - 1) * NUMTEAMS] == 0) {
          //other team hasn't played favorite team yet

        }
        else { //other team must've won vs favorite_team

        }
      }
    }
  }
  else if(favorite_team_points == 4) {

  }
  else if(favorite_team_points == 3) {

  }
  else if(favorite_team_points == 1) {

  }
  else {

  }

  return 0;
}

void setMatch(int* matches, int team1, int team2, int team1_score, int team2_score) {
  matches[(team1 - 1) + (team2 - 1) * NUMTEAMS] = team1_score;
  matches[(team2 - 1) + (team1 - 1) * NUMTEAMS] = team2_score;
}

bool isValidMatch(int* matches, int team1, int team2) {
  return matches[(team1 - 1) + (team2 - 1) * NUMTEAMS] == 0;
}

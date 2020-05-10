//20200501 10:28 PM 完成程式基本架構, main(), PrintResult()
//20200509 09:55 AM 完成isValid(), GameControlModule()
//20200509 03:25 PM 完成PlayerModule()架構, 完成修改PrintResult()(為了要讓unmarked的狀況也能印出來)
//20200509 11:11 PM 完成ClauseCheck(), SingleMatching()
//20200510 10:43 PM 完成PairwiseMatching()
//20200510 10:58 PM 完成GenerateClause()裡面(n==m)以及(n==0)的狀況
//20200510 11:50 PM 完成GenerateClause()
//20200511 01:34 AM 完成debug?? (Easy:9X9 地雷*10 順利完成, Medium:16X16 地雷*25 順利完成)
#include <iostream>     //for I/O
#include <stdlib.h>     //for rand(), srand()
#include <time.h>       //for time()
#include <vector>       //for STL vector
#include <cmath>        //for sqrt()
#include <algorithm>    //for sort()

//Define the size and the number of mine of the map which is easy to modified
#define ROW 9
#define COLUMN 9
#define MINE 10

using namespace std;
//Class of literals
class node{
public:    
    int x=0, y=0;
    bool assignment=false;
};

//These 2 for Game Control Module
int map[ROW][COLUMN];
bool mine[ROW][COLUMN];

//These 2 for Player Module
bool player_mine[ROW][COLUMN];
bool player_visited[ROW][COLUMN];

//These 2 for finding the neighbor of the current cell
int d_row[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
int d_column[8] = {1, 1, 1, 0, 0, -1, -1, -1};

//sorting comparison
bool comparison(vector<node> &a, vector<node> &b){
    return a.size() < b.size();
}

//Check if the point is in the map or not
bool isValid(int a, int b){
    if(a<0 || a>=ROW ||  b<0 || b>=COLUMN){
        return false;
    }
    return true;
}

//Build Up the Mine Map
void GameControlModule(vector<vector<node>> &initial_KB){
    int initial_nodes = (int)round(sqrt(ROW * COLUMN));
    bool visited[ROW][COLUMN];
    //initialize the map
    for (int i = 0; i < ROW;i++){
        for (int j = 0; j < COLUMN;j++){
            map[i][j] = 0;
            mine[i][j] = false;
            visited[i][j] = false;
        }
    }
    //randomly generate the mine
    int recent_mine = 0;
    srand(time(NULL));
    while(recent_mine<MINE){
        int i = rand()%ROW;
        int j = rand()%COLUMN;
        if(!mine[i][j]){
            map[i][j] = -1;
            mine[i][j] = true;
            recent_mine++;
        }
    }
    
    //calculate the hints
    for (int i = 0; i < ROW;i++){
        for (int j = 0; j < COLUMN;j++){
            if(mine[i][j]){
                continue;
            }
            for (int k = 0; k < 8;k++){
                if(isValid(i+d_row[k], j+d_column[k])){
                    if(mine[i+d_row[k]][j+d_column[k]]){
                        map[i][j]++;
                    }
                }
            }
        }
    }
    //print out the map and hints
    cout << "Mine Map:" << endl;
    for (int i = 0; i < ROW;i++){
        for (int j = 0; j < COLUMN;j++){
            if(mine[i][j]){
                cout << "* ";
            }
            else{
                cout << map[i][j] << " ";
            }
        }
        cout << endl;
    }
    cout << endl;
    //pick up the initial KB
    recent_mine = 0;
    while(recent_mine<initial_nodes){
        int i = rand()%ROW;
        int j = rand()%COLUMN;
        if(!mine[i][j] && !visited[i][j]){
            vector<node> clause;
            node literal;
            literal.x = i;
            literal.y = j;
            literal.assignment = false;
            clause.push_back(literal);
            initial_KB.push_back(clause);
            visited[i][j] = true;
            recent_mine++;
        }
    }
}

//check for duplication or subsumption
bool ClauseCheck(vector<vector<node>> &KB){
    bool no_change = true;
    for (int i = 0; i < KB.size();i++){
        for (int j = 0; j < KB[i].size();j++){
            int x = KB[i][j].x, y = KB[i][j].y;
            if(player_visited[x][y]){
                KB[i].erase(KB[i].begin() + j);
                no_change = false;
                j--;
            }
        }
        if(KB[i].size()==0){
            KB.erase(KB.begin() + i);
            no_change = false;
            i--;
        }
    }
    //sort the clauses by their size
    sort(KB.begin(), KB.end(), comparison);
    for (int i = 0; i < KB.size();i++){
        for (int j = i+1; j < KB.size();j++){
            //check for duplication
            if(KB[i].size()==KB[j].size()){
                int same = 0;
                for (int m = 0; m < KB[i].size() && same<KB[i].size();m++){
                    for (int n = 0; n < KB[j].size() && same<KB[j].size();n++){
                        if(KB[i][m].x==KB[j][n].x && KB[i][m].y==KB[j][n].y && KB[i][m].assignment==KB[j][n].assignment){
                            same++;
                            break;
                        }
                    }
                }
                if (same == KB[j].size()){
                    KB.erase(KB.begin() + j);
                    no_change = false;
                    j--;
                }
            }
            //check for subsumption
            else if(KB[i].size()<KB[j].size()){
                int same = 0;
                for (int m = 0; m < KB[i].size() && same<KB[i].size();m++){
                    for (int n = 0; n < KB[j].size() && same<KB[j].size();n++){
                        if(KB[i][m].x==KB[j][n].x && KB[i][m].y==KB[j][n].y && KB[i][m].assignment==KB[j][n].assignment){
                            same++;
                            break;
                        }
                    }
                }
                if (same == KB[i].size()){
                    KB.erase(KB.begin() + j);
                    no_change = false;
                    j--;
                }
            }
        }
    }
    return no_change;
}

//Single Matching for there is a single-literal clause in KB
void SingleMatching(vector<vector<node>> &KB, int x, int y, bool assignment){
    //check for duplication or subsumption
    ClauseCheck(KB);
    //Sigle-literal clause matching
    for (int i = 0; i < KB.size();i++){
        for (int j = 0; j < KB[i].size();j++){
            if(KB[i][j].x==x && KB[i][j].y==y && KB[i][j].assignment!=assignment){
                KB[i].erase(KB[i].begin() + j);
                j--;
                break;
            }
        }
    }
}

//Generate Clause if the Single-literal Clause implies that t is clear
void GenerateClause(vector<vector<node>> &KB, int x, int y, bool assignment){
    int n = map[x][y], m = 0;
    vector<node> unmarked_cells;
    for (int i = 0; i < 8;i++){
        if(isValid(x+d_row[i],y+d_column[i])){
            //cheking the number of surrounding unmarked cells
            if(!player_visited[x+d_row[i]][y+d_column[i]]){
                node tmp;
                tmp.x = x + d_row[i];
                tmp.y = y + d_column[i];
                tmp.assignment = false;
                unmarked_cells.push_back(tmp);
                m++;
            }
            //checking the number of surrounding marked mines
            else if(player_mine[x+d_row[i]][y+d_column[i]]){
                n--;
            }
        }
    }
    //all unmarked cells are mines
    if(n==m){
        for (int i = 0; i < unmarked_cells.size();i++){
            unmarked_cells[i].assignment = true;
            vector<node> tmp;
            tmp.push_back(unmarked_cells[i]);
            KB.push_back(tmp);
            ClauseCheck(KB);
        }
    }
    //all unmarked cells are clear
    else if(n==0){
        for (int i = 0; i < unmarked_cells.size();i++){
            unmarked_cells[i].assignment = false;
            vector<node> tmp;
            tmp.push_back(unmarked_cells[i]);
            KB.push_back(tmp);
            ClauseCheck(KB);
        }
    }
    //other situation
    else{
        int all_positive = m - n + 1, all_negative = n + 1;
        //generate all-positive assignment clause
        if(all_positive==2){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    vector<node> clause;
                    unmarked_cells[i_1].assignment = true;
                    clause.push_back(unmarked_cells[i_1]);
                    unmarked_cells[i_2].assignment = true;
                    clause.push_back(unmarked_cells[i_2]);
                    KB.push_back(clause);
                    ClauseCheck(KB);
                }
            }
        }
        else if(all_positive==3){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    for(int i_3=i_2+1; i_3 < unmarked_cells.size();i_3++){
                        vector<node> clause;
                        unmarked_cells[i_1].assignment = true;
                        clause.push_back(unmarked_cells[i_1]);
                        unmarked_cells[i_2].assignment = true;
                        clause.push_back(unmarked_cells[i_2]);
                        unmarked_cells[i_3].assignment = true;
                        clause.push_back(unmarked_cells[i_3]);
                        KB.push_back(clause);
                        ClauseCheck(KB);
                    }
                }
            }
        }
        else if(all_positive==4){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    for(int i_3=i_2+1; i_3 < unmarked_cells.size();i_3++){
                        for(int i_4=i_3+1; i_4 < unmarked_cells.size();i_4++){
                            vector<node> clause;
                            unmarked_cells[i_1].assignment = true;
                            clause.push_back(unmarked_cells[i_1]);
                            unmarked_cells[i_2].assignment = true;
                            clause.push_back(unmarked_cells[i_2]);
                            unmarked_cells[i_3].assignment = true;
                            clause.push_back(unmarked_cells[i_3]);
                            unmarked_cells[i_4].assignment = true;
                            clause.push_back(unmarked_cells[i_4]);
                            KB.push_back(clause);
                            ClauseCheck(KB);
                        }
                    }
                }
            }
        }
        else if(all_positive==5){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    for(int i_3=i_2+1; i_3 < unmarked_cells.size();i_3++){
                        for(int i_4=i_3+1; i_4 < unmarked_cells.size();i_4++){
                            for(int i_5=i_4+1; i_5 < unmarked_cells.size();i_5++){
                                vector<node> clause;
                                unmarked_cells[i_1].assignment = true;
                                clause.push_back(unmarked_cells[i_1]);
                                unmarked_cells[i_2].assignment = true;
                                clause.push_back(unmarked_cells[i_2]);
                                unmarked_cells[i_3].assignment = true;
                                clause.push_back(unmarked_cells[i_3]);
                                unmarked_cells[i_4].assignment = true;
                                clause.push_back(unmarked_cells[i_4]);
                                unmarked_cells[i_5].assignment = true;
                                clause.push_back(unmarked_cells[i_5]);
                                KB.push_back(clause);
                                ClauseCheck(KB);
                            }
                        }
                    }
                }
            }
        }
        else if(all_positive==6){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    for(int i_3=i_2+1; i_3 < unmarked_cells.size();i_3++){
                        for(int i_4=i_3+1; i_4 < unmarked_cells.size();i_4++){
                            for(int i_5=i_4+1; i_5 < unmarked_cells.size();i_5++){
                                for(int i_6=i_5+1; i_6 < unmarked_cells.size();i_6++){
                                    vector<node> clause;
                                    unmarked_cells[i_1].assignment = true;
                                    clause.push_back(unmarked_cells[i_1]);
                                    unmarked_cells[i_2].assignment = true;
                                    clause.push_back(unmarked_cells[i_2]);
                                    unmarked_cells[i_3].assignment = true;
                                    clause.push_back(unmarked_cells[i_3]);
                                    unmarked_cells[i_4].assignment = true;
                                    clause.push_back(unmarked_cells[i_4]);
                                    unmarked_cells[i_5].assignment = true;
                                    clause.push_back(unmarked_cells[i_5]);
                                    unmarked_cells[i_6].assignment = true;
                                    clause.push_back(unmarked_cells[i_6]);
                                    KB.push_back(clause);
                                    ClauseCheck(KB);
                                }
                            }
                        }
                    }
                }
            }
        }
        else if(all_positive==7){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    for(int i_3=i_2+1; i_3 < unmarked_cells.size();i_3++){
                        for(int i_4=i_3+1; i_4 < unmarked_cells.size();i_4++){
                            for(int i_5=i_4+1; i_5 < unmarked_cells.size();i_5++){
                                for(int i_6=i_5+1; i_6 < unmarked_cells.size();i_6++){
                                    for(int i_7=i_6+1; i_7 < unmarked_cells.size();i_7++){
                                        vector<node> clause;
                                        unmarked_cells[i_1].assignment = true;
                                        clause.push_back(unmarked_cells[i_1]);
                                        unmarked_cells[i_2].assignment = true;
                                        clause.push_back(unmarked_cells[i_2]);
                                        unmarked_cells[i_3].assignment = true;
                                        clause.push_back(unmarked_cells[i_3]);
                                        unmarked_cells[i_4].assignment = true;
                                        clause.push_back(unmarked_cells[i_4]);
                                        unmarked_cells[i_5].assignment = true;
                                        clause.push_back(unmarked_cells[i_5]);
                                        unmarked_cells[i_6].assignment = true;
                                        clause.push_back(unmarked_cells[i_6]);
                                        unmarked_cells[i_7].assignment = true;
                                        clause.push_back(unmarked_cells[i_7]);
                                        KB.push_back(clause);
                                        ClauseCheck(KB);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if(all_positive==8){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    for(int i_3=i_2+1; i_3 < unmarked_cells.size();i_3++){
                        for(int i_4=i_3+1; i_4 < unmarked_cells.size();i_4++){
                            for(int i_5=i_4+1; i_5 < unmarked_cells.size();i_5++){
                                for(int i_6=i_5+1; i_6 < unmarked_cells.size();i_6++){
                                    for(int i_7=i_6+1; i_7 < unmarked_cells.size();i_7++){
                                        for(int i_8=i_7+1; i_8 < unmarked_cells.size();i_8++){
                                            vector<node> clause;
                                            unmarked_cells[i_1].assignment = true;
                                            clause.push_back(unmarked_cells[i_1]);
                                            unmarked_cells[i_2].assignment = true;
                                            clause.push_back(unmarked_cells[i_2]);
                                            unmarked_cells[i_3].assignment = true;
                                            clause.push_back(unmarked_cells[i_3]);
                                            unmarked_cells[i_4].assignment = true;
                                            clause.push_back(unmarked_cells[i_4]);
                                            unmarked_cells[i_5].assignment = true;
                                            clause.push_back(unmarked_cells[i_5]);
                                            unmarked_cells[i_6].assignment = true;
                                            clause.push_back(unmarked_cells[i_6]);
                                            unmarked_cells[i_7].assignment = true;
                                            clause.push_back(unmarked_cells[i_7]);
                                            unmarked_cells[i_8].assignment = true;
                                            clause.push_back(unmarked_cells[i_8]);
                                            KB.push_back(clause);
                                            ClauseCheck(KB);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        //generate all-negative assignment clause
        if(all_negative==2){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    vector<node> clause;
                    unmarked_cells[i_1].assignment = false;
                    clause.push_back(unmarked_cells[i_1]);
                    unmarked_cells[i_2].assignment = false;
                    clause.push_back(unmarked_cells[i_2]);
                    KB.push_back(clause);
                    ClauseCheck(KB);
                }
            }
        }
        else if(all_negative==3){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    for(int i_3=i_2+1; i_3 < unmarked_cells.size();i_3++){
                        vector<node> clause;
                        unmarked_cells[i_1].assignment = false;
                        clause.push_back(unmarked_cells[i_1]);
                        unmarked_cells[i_2].assignment = false;
                        clause.push_back(unmarked_cells[i_2]);
                        unmarked_cells[i_3].assignment = false;
                        clause.push_back(unmarked_cells[i_3]);
                        KB.push_back(clause);
                        ClauseCheck(KB);
                    }
                }
            }
        }
        else if(all_negative==4){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    for(int i_3=i_2+1; i_3 < unmarked_cells.size();i_3++){
                        for(int i_4=i_3+1; i_4 < unmarked_cells.size();i_4++){
                            vector<node> clause;
                            unmarked_cells[i_1].assignment = false;
                            clause.push_back(unmarked_cells[i_1]);
                            unmarked_cells[i_2].assignment = false;
                            clause.push_back(unmarked_cells[i_2]);
                            unmarked_cells[i_3].assignment = false;
                            clause.push_back(unmarked_cells[i_3]);
                            unmarked_cells[i_4].assignment = false;
                            clause.push_back(unmarked_cells[i_4]);
                            KB.push_back(clause);
                            ClauseCheck(KB);
                        }
                    }
                }
            }
        }
        else if(all_negative==5){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    for(int i_3=i_2+1; i_3 < unmarked_cells.size();i_3++){
                        for(int i_4=i_3+1; i_4 < unmarked_cells.size();i_4++){
                            for(int i_5=i_4+1; i_5 < unmarked_cells.size();i_5++){
                                vector<node> clause;
                                unmarked_cells[i_1].assignment = false;
                                clause.push_back(unmarked_cells[i_1]);
                                unmarked_cells[i_2].assignment = false;
                                clause.push_back(unmarked_cells[i_2]);
                                unmarked_cells[i_3].assignment = false;
                                clause.push_back(unmarked_cells[i_3]);
                                unmarked_cells[i_4].assignment = false;
                                clause.push_back(unmarked_cells[i_4]);
                                unmarked_cells[i_5].assignment = false;
                                clause.push_back(unmarked_cells[i_5]);
                                KB.push_back(clause);
                                ClauseCheck(KB);
                            }
                        }
                    }
                }
            }
        }
        else if(all_negative==6){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    for(int i_3=i_2+1; i_3 < unmarked_cells.size();i_3++){
                        for(int i_4=i_3+1; i_4 < unmarked_cells.size();i_4++){
                            for(int i_5=i_4+1; i_5 < unmarked_cells.size();i_5++){
                                for(int i_6=i_5+1; i_6 < unmarked_cells.size();i_6++){
                                    vector<node> clause;
                                    unmarked_cells[i_1].assignment = false;
                                    clause.push_back(unmarked_cells[i_1]);
                                    unmarked_cells[i_2].assignment = false;
                                    clause.push_back(unmarked_cells[i_2]);
                                    unmarked_cells[i_3].assignment = false;
                                    clause.push_back(unmarked_cells[i_3]);
                                    unmarked_cells[i_4].assignment = false;
                                    clause.push_back(unmarked_cells[i_4]);
                                    unmarked_cells[i_5].assignment = false;
                                    clause.push_back(unmarked_cells[i_5]);
                                    unmarked_cells[i_6].assignment = false;
                                    clause.push_back(unmarked_cells[i_6]);
                                    KB.push_back(clause);
                                    ClauseCheck(KB);
                                }
                            }
                        }
                    }
                }
            }
        }
        else if(all_negative==7){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    for(int i_3=i_2+1; i_3 < unmarked_cells.size();i_3++){
                        for(int i_4=i_3+1; i_4 < unmarked_cells.size();i_4++){
                            for(int i_5=i_4+1; i_5 < unmarked_cells.size();i_5++){
                                for(int i_6=i_5+1; i_6 < unmarked_cells.size();i_6++){
                                    for(int i_7=i_6+1; i_7 < unmarked_cells.size();i_7++){
                                        vector<node> clause;
                                        unmarked_cells[i_1].assignment = false;
                                        clause.push_back(unmarked_cells[i_1]);
                                        unmarked_cells[i_2].assignment = false;
                                        clause.push_back(unmarked_cells[i_2]);
                                        unmarked_cells[i_3].assignment = false;
                                        clause.push_back(unmarked_cells[i_3]);
                                        unmarked_cells[i_4].assignment = false;
                                        clause.push_back(unmarked_cells[i_4]);
                                        unmarked_cells[i_5].assignment = false;
                                        clause.push_back(unmarked_cells[i_5]);
                                        unmarked_cells[i_6].assignment = false;
                                        clause.push_back(unmarked_cells[i_6]);
                                        unmarked_cells[i_7].assignment = false;
                                        clause.push_back(unmarked_cells[i_7]);
                                        KB.push_back(clause);
                                        ClauseCheck(KB);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if(all_negative==8){
            for (int i_1=0; i_1 < unmarked_cells.size();i_1++){
                for (int i_2=i_1+1; i_2 < unmarked_cells.size();i_2++){
                    for(int i_3=i_2+1; i_3 < unmarked_cells.size();i_3++){
                        for(int i_4=i_3+1; i_4 < unmarked_cells.size();i_4++){
                            for(int i_5=i_4+1; i_5 < unmarked_cells.size();i_5++){
                                for(int i_6=i_5+1; i_6 < unmarked_cells.size();i_6++){
                                    for(int i_7=i_6+1; i_7 < unmarked_cells.size();i_7++){
                                        for(int i_8=i_7+1; i_8 < unmarked_cells.size();i_8++){
                                            vector<node> clause;
                                            unmarked_cells[i_1].assignment = false;
                                            clause.push_back(unmarked_cells[i_1]);
                                            unmarked_cells[i_2].assignment = false;
                                            clause.push_back(unmarked_cells[i_2]);
                                            unmarked_cells[i_3].assignment = false;
                                            clause.push_back(unmarked_cells[i_3]);
                                            unmarked_cells[i_4].assignment = false;
                                            clause.push_back(unmarked_cells[i_4]);
                                            unmarked_cells[i_5].assignment = false;
                                            clause.push_back(unmarked_cells[i_5]);
                                            unmarked_cells[i_6].assignment = false;
                                            clause.push_back(unmarked_cells[i_6]);
                                            unmarked_cells[i_7].assignment = false;
                                            clause.push_back(unmarked_cells[i_7]);
                                            unmarked_cells[i_8].assignment = false;
                                            clause.push_back(unmarked_cells[i_8]);
                                            KB.push_back(clause);
                                            ClauseCheck(KB);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    ClauseCheck(KB);
}

//Pairwise Matching for there is no single-literal clause in KB
bool PairwiseMatching(vector<vector<node>> &KB){
    bool no_change = true;
    //check for duplication or subsumption
    no_change=ClauseCheck(KB);
    //check for complementary literal
    for (int i = 0; i < KB.size();i++){
        for (int j = i+1; j < KB.size();j++){
            //only do maching when there is at least one clause has only two literals
            if(KB[i].size()>2 && KB[j].size()>2){
                break;
            }
            int same = 0;
            int same_x = 0, same_y = 0;
            vector<node> matching_clause;
            //check that two clauses can be matched
            for (int m = 0; m < KB[i].size() && same<KB[i].size();m++){
                if(same>1){
                    break;
                }
                for (int n = 0; n < KB[j].size() && same<KB[j].size();n++){
                    if(KB[i][m].x==KB[j][n].x && KB[i][m].y==KB[j][n].y && KB[i][m].assignment!=KB[j][n].assignment){
                        same_x = KB[i][m].x;
                        same_y = KB[i][m].y;
                        same++;
                        break;
                    }
                }
            }
            if(same==1){
                for (int m = 0; m < KB[i].size();m++){
                    if(KB[i][m].x==same_x && KB[i][m].y==same_y){
                        continue;
                    }
                    else{
                        matching_clause.push_back(KB[i][m]);
                    }
                }
                for (int m = 0; m < KB[j].size();m++){
                    if(KB[j][m].x==same_x && KB[j][m].y==same_y){
                        continue;
                    }
                    else{
                        matching_clause.push_back(KB[j][m]);
                    }
                }
                no_change = false;
                KB.erase(KB.begin() + j);
                KB.erase(KB.begin() + i);
                KB.push_back(matching_clause);
                i--;
                j--;
            }
        }
    }
    return no_change;
}

//The Main Part of This Assignment
void PlayerModule(vector<vector<node>> &KB, vector<node> &KB_0){
    int index = 0, iteration=0;
    //Initialize the map
    for (int i = 0; i < ROW;i++){
        for (int j = 0; j < COLUMN;j++){
            player_mine[i][j] = false;
            player_visited[i][j] = false;
        }
    }
    //Start the Game Flow
    while(KB.size()!=0){
        //Search for single-literal clause
        int i = 0;
        bool No_New_Clause = false;
        for (i = 0; i < KB.size();i++){
            if(KB[i].size()==1){
                index = i;
                break;
            }
        }
        //There exist a single-literal clause
        if(i<KB.size()){
            //Mark the cell as its assignment
            int x = KB[i][0].x;
            int y = KB[i][0].y;
            player_mine[x][y] = KB[index][0].assignment;
            player_visited[x][y] = true;
            //Put it into KB_0
            KB_0.push_back(KB[index][0]);
            //Remove the single-literal clause from KB
            KB.erase(KB.begin() + index);
            SingleMatching(KB,x,y,player_mine[x][y]);
            if(!player_mine[x][y]){
                GenerateClause(KB,x,y,player_mine[x][y]);
            }
        }
        //There is no single-literal clause
        else{
            No_New_Clause=PairwiseMatching(KB);
        }
        if(No_New_Clause){
            cout << "No new clause" << endl;
            break;
        }
        iteration++;
        if(iteration>1000000){
            cout << "over the threshold" << endl;
            break;
        }
    }
}

//Print out the Result
void PrintResult(vector<node> KB_0){
    //build up the result table
    bool result[ROW][COLUMN];
    bool unmarked[ROW][COLUMN];
    for (int i = 0; i < ROW;i++){
        for (int j = 0; j < COLUMN;j++){
            result[i][j] = false;
            unmarked[i][j] = true;
        }
    }
    for (int i = 0; i < KB_0.size(); i++){
        int x = KB_0[i].x, y = KB_0[i].y;
        bool assignment = KB_0[i].assignment;
        result[x][y] = assignment;
        unmarked[x][y] = false;
    }
    //print out the result table
    for (int i = 0; i < ROW;i++){
        for (int j = 0; j < COLUMN;j++){
            //if the cell is unmarked
            if(unmarked[i][j]){
                cout << "X ";
            }
            //if the cell is marked
            else{
                //if the cell is mined
                if(result[i][j]){
                    cout << "* ";
                }
                //if the cell is clear
                else{
                    cout << "_ ";
                }
            }
        }
        cout << endl;
    }
}

int main(){
    vector<vector<node>> initial_KB;
    vector<node> KB_0;
    GameControlModule(initial_KB);
    PlayerModule(initial_KB,KB_0);
    cout << "The Result Map:" << endl;
    PrintResult(KB_0);
    return 0;
}
/*
    Group ID    : 26
    Group Name  : 阿緹菲修·因貼勒炯斯
    Group Member: 0716012 魏致遠, 0716019 詹昀銘, 0716236 劉耀文
*/
#include "STcpClient.h"
#include <stdlib.h>
#include <iostream>

#define MAX_DEPTH 6

enum{CORNER=-1, SPACE=0, BLACK, WHITE};

// check that the step is legal or not
bool islegal(int board[8][8], int row, int column, bool is_black){
    // check that the step is not in the board
    if(row<0 || row >=8 || column <0 || column >=8){
        return false;
    }
    // check that the step is on the 4 corners
    if(board[row][column] == CORNER){
        return false;
    }
    // check if it is already assigned
    if(board[row][column]==WHITE || board[row][column]==BLACK){
        return false;
    }
    // check if it is in 6x6 range
    if(row>0 && row<7 && column>0 && column<7){
        return true;
    }

    // if not in 6x6 range
    int color = is_black?BLACK:WHITE;
    int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    // all 8 directions
    for(int i=0; i<8; i++){
    	bool find = false;
        int next_row = row + dr[i];
        int next_column = column + dc[i];
        while(next_row>=0 && next_column>=0 && next_row<8 && next_column<8){
            int board_color = board[next_row][next_column];
            // if there is space between we do not flip
            if(board_color == SPACE || board_color == CORNER) break;
            // find the different color
            if(board_color != color) find = true;
            // if there is different color between two same color return ture
            if(board_color == color){
                if(find) return true;
                break;
            }
            // move to next
            next_row += dr[i];
            next_column += dc[i]; 
        }
    }
    
    return false;
}

// used to flip the chesses
void Flipping(int board[8][8], int row, int column, bool is_black){
    int color = is_black?BLACK:WHITE;
    int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    // flip all 8 directions
    for(int i=0; i<8; i++){
        int next_row = row + dr[i];
        int next_column = column + dc[i];
        while(next_row>=0 && next_column>=0 && next_row<8 && next_column<8){
            int board_color = board[next_row][next_column];
            // if there is space between we do not flip
            if(board_color == SPACE || board_color == CORNER) break;
            // find the same color
            if(board_color == color){
                // filp all between the two
                next_row -= dr[i];
                next_column -= dc[i];				
                while(next_row!=row || next_column!=column){
                    board[next_row][next_column] = is_black?BLACK:WHITE;
                    next_row -= dr[i];
                    next_column -= dc[i];
                }
                break;
            }
            // move to next
            next_row += dr[i];
            next_column += dc[i]; 
        }
    }
}

// calculate the recent board score (maybe # of black - # of white)
int CalculateScore(int board[8][8]){
    int score = 0;
    for (int row = 0; row < 8;row++){
        for (int column = 0; column < 8;column++){
            // if the current block is black
            if(board[row][column]==BLACK){
                score++;
            }
            // if the current block is white
            else if(board[row][column]==WHITE){
                score--;
            }
        }
    }
    return score;
}

// the main part of searching (using DFS)
int GameProcess(int board[8][8], int depth, bool is_black, int alpha, int beta){
    // used to check if there is any valid move
    bool valid = false;
    int temp_board[8][8];

    // initialize the score based on is_black
    int score = is_black?-100:100;
    if(depth >= MAX_DEPTH){
        return CalculateScore(board);
    }

    // consider the best move
    for (int next_row = 0; next_row < 8; next_row++){
        for (int next_column = 0; next_column < 8;next_column++){
            if(islegal(board, next_row, next_column, is_black)){
                valid = true;
                // duplicate the board
                std::copy(&board[0][0], &board[0][0]+64, &temp_board[0][0]);

                // put the new step into board and flip the chesses
                if(is_black){
                    temp_board[next_row][next_column] = BLACK;
                }
                else if(!is_black){
                    temp_board[next_row][next_column] = WHITE;
                }
                Flipping(temp_board, next_row, next_column, is_black);

                // get the value of next step
                int tmp = GameProcess(temp_board, depth+1, !is_black, alpha, beta);
                
                // calculate the minimax value
                if(is_black){
                    if(tmp>score){
                        score = tmp;
                    }
                    if(score>=beta){
                        return score;
                    }
                    alpha = std::max(alpha, score);
                }
                else if(!is_black){
                    if(tmp<score){
                        score = tmp;
                    }
                    if(score<=alpha){
                        return score;
                    }
                    beta = std::min(beta, score);
                }
            }
        }
    }

    // if no valid move
    // previous player keep playing
    if(!valid) return GameProcess(board, depth+1, !is_black, alpha, beta);
    return score;
}

/*
    輪到此程式移動棋子
    board : 棋盤狀態(vector of vector), board[i][j] = i row, j column 棋盤狀態(i, j 從 0 開始)
            0 = 空、1 = 黑、2 = 白、-1 = 四個角落
    is_black : True 表示本程式是黑子、False 表示為白子

    return Step
    Step : vector, Step = {r, c}
            r, c 表示要下棋子的座標位置 (row, column) (zero-base)
*/
std::vector<int> GetStep(std::vector<std::vector<int>>& board, bool is_black) {
    // choose the current best step by minimax {row, column} and initialize it as {0, 0}
    std::vector<int> return_step;
    return_step.resize(2,0);
    int score = is_black?-100:100;
    int alpha = -100, beta = 100;

    int temp_board[8][8], my_board[8][8];
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            my_board[i][j] = board[i][j];
        }
    }

    for (int next_row = 0; next_row < 8; next_row++){
        for (int next_column = 0; next_column < 8;next_column++){
            if(islegal(my_board, next_row, next_column, is_black)){
                // duplicate the board
                std::copy(&my_board[0][0], &my_board[0][0]+64, &temp_board[0][0]);

                // put the new step into board and flip the chesses
                if(is_black){
                    temp_board[next_row][next_column] = BLACK;
                }
                else if(!is_black){
                    temp_board[next_row][next_column] = WHITE;
                }
                Flipping(temp_board, next_row, next_column, is_black);

                // get the value of next step
                int tmp = GameProcess(temp_board, 1, !is_black, alpha, beta);
                
                // calculate the minimax value
                // set the best move to current move
                if(is_black){
                    if(tmp>score){
                        return_step[0] = next_row;
                        return_step[1] = next_column;
                        score = tmp;
                    }
                    if(score>=beta){
                        return return_step;
                    }
                    alpha = std::max(alpha, score);
                }
                else if(!is_black){
                    if(tmp<score){
                        return_step[0] = next_row;
                        return_step[1] = next_column;
                        score = tmp;
                    }
                    if(score<=alpha){
                        return return_step;
                    }
                    beta = std::min(beta, score);
                }
            }
        }
    }
    return return_step;
}

int main() {
	int id_package;
	std::vector<std::vector<int>> board;
	std::vector<int> step;
	
	bool is_black;
	while (true) {
		if (GetBoard(id_package, board, is_black))
			break;

		step = GetStep(board, is_black);
		SendStep(id_package, step);
	}
}
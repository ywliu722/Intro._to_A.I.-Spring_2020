//20200330 11:12 PM 完成main(), BFS(), DFS(), initialization(), finding_path(), valid()
//20200402 11:30 PM 完成IDS()
//20200404 12:30 AM 完成A_star()
//20200405 10:30 PM 完成IDA_star()
//20200406 12:50 AM 完成程式碼註解
#include <iostream>
#include <queue>        //BFS, A*會用到
#include <vector>       //A*的priority queue會用到
#include <algorithm>    //BFS, A*會用到 max()

using namespace std;
//建立node類別 以方便儲存各點資料
class node{
    public:
        int x, y;
        int heuristic_cost, current_cost, pre_x, pre_y;
};

//Priority Queue的比較函式 (以小排到大)
struct comparison{
    bool operator()(const node &a, const node &b)const{
        return a.heuristic_cost > b.heuristic_cost;
    }
};

int x_start = 0, y_start = 0, x_goal = 0, y_goal = 0;   //宣告起始點及目標點的座標變數
bool visited[8][8];                                     //紀錄此點是否被找過
node pre[8][8];                                         //紀錄是從哪個點走來這點的
int step_x[8] = {1,2,2,1,-1,-2,-2,-1};                  //騎士X軸所走的步數
int step_y[9] = {2,1,-1,-2,-2,-1,1,2};                  //騎士Y軸所走的步數

//初始化二維陣列
void initialize(){
    node tmp;
    tmp.x = -1;
    tmp.y = -1;
    tmp.current_cost = INT_MAX;
    for (int i = 0; i < 8;i++){
        for (int j = 0; j < 8;j++){
            visited[i][j] = false;
            pre[i][j] = tmp;
        }
    }
}

//根據pre[i][y]所記錄的資料 一步一步往回找路徑並印出
void finding_path(node current){
    
    //如果到達起始點 則印出座標後return至上一層遞迴函式
    if(current.x==x_start && current.y==y_start){
        cout << "(" << current.x << "," << current.y << ")";
        return;
    }
    finding_path(pre[current.x][current.y]);                //如果還不是起始點 則用遞迴方式往前找
    cout << "(" << current.x << "," << current.y << ")";    //印出當下點的座標位置
    return;                                                 //return至上一層遞迴函式
}

//檢查接下來的點是否超過棋盤範圍或是已經被找過了
bool valid(int x, int y){
    //如果這個點超過了棋盤範圍 則回傳false
    if(x<0 || x>7 || y<0 || y>7){
        return false;
    }
    //如果這個點已經被找過了 則回傳false
    if(visited[x][y]){
        return false;
    }
    //兩個條件都不符的話 表示這個點可以繼續找下去 回傳true
    return true;
}

//Heuristic Function for A* and IDA*
int heuristic(node tmp){
    return ((abs(tmp.x - x_goal) + abs(tmp.y - y_goal)) / 3);
}

//進行 Breadth First Search (會被修改的參數採Pass by Reference)
int BFS(node start, int &max_size){
    int counting=0;                     //宣告計算總共有幾個點被發現並裝進Queue裡面
    queue<node> q;                      //宣告frontier
    visited[start.x][start.y] = true;   //紀錄起始點已經被發現過了
    q.push(start);                      //把起始點丟進queue裡面
    while(!q.empty()){
        //接下來兩行是要記錄Queue最大的時候有多幾個點在裡面(用來表達Space Complexity)
        int siz = q.size();
        max_size = max(max_size, siz);
        
        node current = q.front();       //從queue取出下個點
        q.pop();
        
        if(current.x == x_goal && current.y==y_goal){
            break;                      //如果已經到了目標點則終止迴圈
        }
        
        for (int i = 0; i < 8;i++){             //每個點都可以走向周圍8個點
            node tmp;
            tmp.x = current.x + step_x[i];      //新點的X座標
            tmp.y = current.y + step_y[i];      //新點的Y座標
            if(valid(tmp.x,tmp.y)){             //判斷新的點是否可以丟進queue裡面
                counting++;
                q.push(tmp);                    //把新的點丟進queue裡面
                pre[tmp.x][tmp.y] = current;    //紀錄是由哪個點走到新的點
                visited[tmp.x][tmp.y] = true;   //紀錄新的點已經被發現過了
            }
        }
    }
    return counting;
}

//進行 Depth First Search (會被修改的參數採Pass by Reference)
void DFS(node current, bool &flag, int &counting, int &depth, int &max_depth){
    depth++;                                //深度+1
    max_depth = max(max_depth, depth);      //記錄深度最大的時候有多深(用來表達Space Complexity 因為DFS的遞迴就像是Stack)
    
    if(current.x == x_goal && current.y == y_goal){
        flag = true;                        //如果找到目標點就將標記變成True
        depth--;                            //將深度減回上一層的深度
        return;
    }
    
    for (int i = 0; i < 8;i++){
        node tmp;
        tmp.x = current.x + step_x[i];
        tmp.y = current.y + step_y[i];
        if(valid(tmp.x,tmp.y) && !flag){
            counting++;                     //如果下個點沒超出棋盤, 還沒找到目標點 計數+1
            pre[tmp.x][tmp.y] = current;    //紀錄是由哪個點走到新的點
            visited[tmp.x][tmp.y] = true;   //紀錄新的點已經被發現過了
            DFS(tmp, flag, counting, depth, max_depth);       //呼叫下一層DFS
        }
    }
    depth--;                                //將深度減回上一層的深度
}

//進行 Iterative Deepening Search (會被修改的參數採Pass by Reference)
void IDS(node current, int depth, bool &flag, int &counting){
    if(depth<0){
        return;                         //如果到達最大深度則return
    }
    
    if(current.x == x_goal && current.y==y_goal){
        flag = true;                    //如果找到目標點就將標記變成True
        return;                         //如果已經到了目標點則跳出這層 回到上層
    }
    
    for (int i = 0; i < 8;i++){
        node tmp;
        tmp.x = current.x + step_x[i];
        tmp.y = current.y + step_y[i];
        if(valid(tmp.x,tmp.y) && !flag && depth>0){
            counting++;                             //如果下個點沒超出棋盤, 沒超過限制深度, 還沒找到目標點 計數+1
            pre[tmp.x][tmp.y] = current;            //紀錄是由哪個點走到新的點
            visited[tmp.x][tmp.y] = true;           //紀錄新的點已經被發現過了
            IDS(tmp, depth - 1, flag, counting);    //呼叫下一層IDS
        }
    }
}

//進行 A* Search (會被修改的參數採Pass by Reference)
int A_star(node start, int &max_size){
    int counting=0;                                     //宣告計算總共有幾個點被發現並裝進Priority Queue裡面
    priority_queue<node, vector<node>, comparison> q;   //宣告儲存各點的Priority Queue 排序方式為comparison(這個已經在上面被定義了)
    
    //先將起點的花費設定好
    start.current_cost = 0;
    start.heuristic_cost = start.current_cost + heuristic(start);
    q.push(start);
    
    while(!q.empty()){
        //接下來兩行是要記錄Priority Queue最大的時候有多幾個點在裡面(用來表達Space Complexity)
        int siz = q.size();
        max_size = max(max_size, siz);
        
        //拿出預估花費最小的那個點
        node current = q.top();
        q.pop();
        
        //先建立這個點的前一個點的物件
        node tmp;
        tmp.x = current.pre_x;
        tmp.y = current.pre_y;
        tmp.current_cost = current.current_cost - 1;
        
        if(pre[current.x][current.y].current_cost > current.current_cost-1){
            pre[current.x][current.y] = tmp;    //如果從起點到這個點的上一個點的實際花費比已經紀錄的路徑還短 就更新一下 像是Dijkstra's Algorithm的Relax一樣
        }
        else{
            continue;                           //承上 否則略過這個點 因為找下去一定會比原先已儲存的路徑還長
        }
        
        if(current.x==x_goal && current.y==y_goal){
            break;                              //如果找到目標點就跳出迴圈
        }
        
        for (int i = 0; i < 8;i++){
            node tmp;
            tmp.x = current.x + step_x[i];
            tmp.y = current.y + step_y[i];
            if(valid(tmp.x,tmp.y)){
                counting++;                                             //如果下個點沒有超出棋盤 就將計數+1
                tmp.current_cost = current.current_cost + 1;            //將從起點到接下來這個點的實際花費算出
                tmp.heuristic_cost = tmp.current_cost + heuristic(tmp); //將從起點經由接下來這個點到目標點的預計花費算出
                tmp.pre_x = current.x;                                  //因為是Tree Search 所以一個點會重複被找到 為了找到路徑 必須儲存上個點是什麼
                tmp.pre_y = current.y;                                  //因為是Tree Search 所以一個點會重複被找到 為了找到路徑 必須儲存上個點是什麼
                q.push(tmp);                                            //將接下來這個點丟入以總路徑長預計花費排序的Priority Queue
            }
        }
    }
    return counting;
}

//進行 Iterative Deepening A* Search (會被修改的參數採Pass by Reference)
void IDA_star(node current, int threshold, bool &flag, int &counting, int &depth, int &max_depth){
    depth++;                                    //深度+1
    max_depth = max(max_depth, depth);          //記錄深度最大的時候有多深(用來表達Space Complexity 因為DFS的遞迴就像是Stack)
    
    if(current.heuristic_cost>threshold){
        counting--;                             //如果預估花費比Threshold還大 就將計數-1(因為這個不應該不放進來)
        depth--;                                //將深度減回上一層的深度
        return;
    }

    //先建立這個點的前一個點的物件
    node tmp;
    tmp.x = current.pre_x;
    tmp.y = current.pre_y;
    tmp.current_cost = current.current_cost - 1;
    
    if(pre[current.x][current.y].current_cost > current.current_cost-1){
        pre[current.x][current.y] = tmp;        //如果從起點到這個點的上一個點的實際花費比已經紀錄的路徑還短 就更新一下 像是Dijkstra's Algorithm的Relax一樣
    }
    else{
        depth--;                                //承上 否則略過這個點 因為找下去一定會比原先已儲存的路徑還長 並且將深度減回上一層的深度
        return;
    }
    
    if(current.x==x_goal && current.y==y_goal){
        flag = true;                            //如果找到目標點就將標記變成True
        depth--;                                //將深度減回上一層的深度
        return;
    }
    
    for (int i = 0; i < 8;i++){
        node tmp;
        tmp.x = current.x + step_x[i];
        tmp.y = current.y + step_y[i];
        tmp.current_cost = current.current_cost + 1;
        tmp.heuristic_cost = tmp.current_cost + heuristic(tmp);
        if(valid(tmp.x,tmp.y) && !flag && current.heuristic_cost <= threshold){
            counting++;                                                 //如果下個點沒有超出棋盤 就將計數+1
            tmp.pre_x = current.x;                                      //因為是Tree Search 所以一個點會重複被找到 為了找到路徑 必須儲存上個點是什麼
            tmp.pre_y = current.y;                                      //因為是Tree Search 所以一個點會重複被找到 為了找到路徑 必須儲存上個點是什麼
            IDA_star(tmp,threshold,flag, counting, depth, max_depth);   //跳入下一層函式中(就像DFS的概念一樣)
        }
    }
    depth--;                                                            //將深度減回上一層的深度
}

int main(){
    //必要變數宣告
    int algorithm=0;    //演算法代號
    char non = 0;       //因為輸入的時候會有一些運算不必要的符號

    //輸入第一筆測資
    cout << "1: BFS, 2: DFS, 3: IDS, 4: A*, 5: IDA*" << endl;
    cout << "Please Enter the Algorithm You Want to Try (Enter 0 if You Want to Exit):";
    while(cin>>algorithm){
        //如果輸入的演算法代號為0, 則終止程式
        if(algorithm == 0){
            break;
        }
        
        initialize();   //初始化兩個二維陣列
        
        //輸入起始點及目標點的座標(x,y)
        cout << "Please Enter the Starting Point(x,y):";
        cin >> non >> x_start >> non >> y_start >> non;
        
        cout << "Please Enter the Goal Point(x,y):";
        cin >> non >> x_goal >> non >> y_goal >> non;

        //建立一個起始點的物件, 等一下丟進演算法函式呼叫裡
        node start;
        start.x = x_start;
        start.y = y_start;
        start.heuristic_cost = heuristic(start);

        //建立一個目標點的物件, 等一下要來找路徑
        node goal;
        goal.x = x_goal;
        goal.y = y_goal;

        //如果演算法代號輸入為1, 則進行BFS
        if(algorithm == 1){
            int max_size = 0;                           //宣告Queue最大大小
            int counting = BFS(start, max_size);        //呼叫BFS函式
            finding_path(goal);                         //找出從起始點到目標點的路徑並印出
            cout << endl;                               //印出換行
            cout << " Time Complexity: " << counting << " points were generated by BFS." << endl;
            cout << "Space Complexity: Maximum Size of Queue is " << max_size << "." << endl;
        }

        //如果演算法代號輸入為2, 則進行DFS
        else if(algorithm == 2){
            int counting=0, depth=-1, max_depth=0;                      //宣告起始計數, 目前深度, 最大深度
            bool flag = false;                                          //宣告是否找到的標記
            DFS(start, flag, counting, depth, max_depth);               //呼叫DFS函式
            finding_path(goal);                                         //找出從起始點到目標點的路徑並印出
            cout << endl;                                               //印出換行
            cout << " Time Complexity: " << counting << " points were generated by DFS." << endl;
            cout << "Space Complexity: Maximum Depth is " << max_depth << "." << endl;
        }

        //如果演算法代號輸入為3, 則進行IDS
        else if(algorithm == 3){
            int counting=0, depth = 0;                  //宣告起始計數, 最大深度
            bool flag = false;                          //宣告是否找到的標記
            while(!flag){
                initialize();                           //初始化二維陣列
                IDS(start, depth, flag, counting);      //呼叫IDS函式
                depth++;                                //將最大深度+1
            }
            finding_path(goal);                         //找出從起始點到目標點的路徑並印出
            cout << endl;                               //印出換行
            cout << " Time Complexity: " << counting << " points were generated by IDS." << endl;
            cout << "Space Complexity: Maximum Depth is " << depth << "." << endl;
        }

        //如果演算法代號輸入為4, 則進行A* Search
        else if(algorithm == 4){
            int max_size = 0;                           //宣告Priority Queue最大大小
            int counting = A_star(start, max_size);     //呼叫A*函式
            finding_path(goal);                         //找出從起始點到目標點的路徑並印出
            cout << endl;                               //印出換行
            cout << " Time Complexity: " << counting << " points were generated by A* Search." << endl;
            cout << "Space Complexity: Maximum Size of Queue is " << max_size << "." << endl;
        }

        //如果演算法代號輸入為5, 則進行IDA* Search
        else if(algorithm == 5){
            int threshold = heuristic(start);               //將起始點的預估花費設為初始Threshold
            int counting = 0, depth = -1, max_depth = 0;    //宣告起始計數, 目前深度, 最大深度
            bool flag = false;                              //宣告是否找到的標記
            while(!flag){
                initialize();                               //初始化二維陣列
                IDA_star(start, threshold, flag, counting, depth, max_depth); //呼叫IDA*函式
                threshold++;                                //加大Threshold
            }
            finding_path(goal);                             //找出從起始點到目標點的路徑並印出
            cout << endl;                                   //印出換行
            cout << " Time Complexity: " << counting << " points were generated by IDA* Search." << endl;
            cout << "Space Complexity: Maximum Depth is " << max_depth << "." << endl;
        }

        cout << endl;                   //多印出一行換行 以區分兩筆測資
        
        //輸入下一筆測資
        cout << "1: BFS, 2: DFS, 3: IDS, 4: A*, 5: IDA*" << endl;
        cout<<"Please Enter the Algorithm You Want to Try (Enter 0 if You Want to Exit):";
    }
    return 0;
}
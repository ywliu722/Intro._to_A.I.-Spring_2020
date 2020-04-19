//20200415 11:09 PM 完成main(), valid(), isComplete(), printResilt()
//20200416 11:30 PM 完成DegreeHeuristic(), MRV(), sorting()
//20200419 14:30 PM 完成invalidAssignment(), both(), LCV()
//20200419 14:45 PM 完成forwardChecking()
//20200419 14:50 PM 完成全部(?
#include <iostream>     //for I/O
#include <vector>       //for資料儲存
#include <algorithm>    //for domain拿出順序(sort())

#define ROW 6
#define COLUMN 6
#define MINE 10

int map[ROW][COLUMN];
int dx[8] = {-1, 0, 1,-1, 1,-1, 0, 1};
int dy[8] = {-1,-1,-1, 0, 0, 1, 1, 1};

using namespace std;
//for list of assignment
class node{
    public:
        node(int i, int j, int k){
            x = i;
            y = j;
            assignment = k;
        }
        int x, y, assignment;
};
//for the domain for all unassignment variables
class dom{
    public:
        dom(int i, int j, bool k, bool l, int m){
            x = i;
            y = j;
            mine = k;
            clear = l;
            domain_size = m;
        }
        int x, y;
        bool mine, clear;
        int degree, domain_size;
};
//檢查地雷周圍的8個點有沒有超出地圖
bool valid(int x, int y){
    if(x<0 || x>=COLUMN || y<0 || y>=ROW){
        return false;
    }
    if(map[x][y]==-1){
        return false;
    }
    return true;
}
//測試加入這個地雷之後 其他unassign variable的domain會不會改變(需修改)
void invalidAssignment(vector<node> variable, vector<dom> &domain){
    //初始化測試地圖
    int test[6][6];
    for (int i = 0; i < 6;i++){
        for (int j = 0; j < 6;j++){
            test[i][j] = 0;
        }
    }
    //把測試地圖地雷周圍8個點+1
    for (int i = 0; i < variable.size();i++){
        if(variable[i].assignment==1){
            for (int j = 0; j < 8;j++){
                if(valid(variable[i].x+dx[j],variable[i].y+dy[j])){
                    test[variable[i].x + dx[j]][variable[i].y + dy[j]]+=1;
                }
            }
        }
    }
    //檢查
    for (int i = 0; i < domain.size();i++){
        if(domain[i].mine){
            for (int j = 0; j < 8;j++){
                int tmp_x = domain[i].x + dx[j], tmp_y = domain[i].y + dy[j];
                if(valid(tmp_x,tmp_y)){
                    if(test[tmp_x][tmp_y]==map[tmp_x][tmp_y] && map[tmp_x][tmp_y]){
                        domain[i].mine = false;
                        if(domain[i].clear){
                            domain[i].domain_size = 1;
                        }
                        else{
                            domain[i].domain_size = 0;
                        }
                        break;
                    }
                }
            }
        }
    }
}
//測試現在的assignment是不是已經找到答案了
bool isComplete(vector<node> variable){
    int counting = 0;
    //初始化測試地圖
    int test[6][6];
    for (int i = 0; i < 6;i++){
        for (int j = 0; j < 6;j++){
            test[i][j] = 0;
        }
    }
    //把測試地圖地雷周圍8個點+1
    for (int i = 0; i < variable.size();i++){
        if(variable[i].assignment==1){
            counting++;
            for (int j = 0; j < 8;j++){
                if(valid(variable[i].x+dx[j],variable[i].y+dy[j])){
                    test[variable[i].x + dx[j]][variable[i].y + dy[j]]+=1;
                }
            }
        }
    }
    //檢查
    for (int i = 0; i < 6;i++){
        for (int j = 0; j < 6;j++){
            if(test[i][j]!=map[i][j] && map[i][j]>=0){
                return false;
            }
        }
    }
    //就算說local constrants都符合 但還有一個global constriant要檢查
    if(counting!=MINE){
        return false;
    }
    return true;
}
//印出結果
void printResult(vector<node> variable){
    bool assignment[ROW][COLUMN];
    for (int i = 0; i < variable.size();i++){
        if(variable[i].assignment==1){
            assignment[variable[i].x][variable[i].y] = true;
        }
        else{
            assignment[variable[i].x][variable[i].y] = false;
        }
    }
    for (int i = 0; i < ROW;i++){
        for (int j = 0; j < COLUMN;j++){
            if(map[i][j]>=0){
                cout << map[i][j] << " ";
                continue;
            }
            if(assignment[i][j]){
                cout << "* ";
            }
            else{
                cout << "_ ";
            }
        }
        cout << endl;
    }
}
//Forward Checking
bool forwardChecking(vector<node> variable, vector<dom> &domain){
    int mine = 0, mine_ready=0;
    //初始化測試地圖, upper bound
    int test[6][6];
    int upper[6][6];
    for (int i = 0; i < 6;i++){
        for (int j = 0; j < 6;j++){
            test[i][j] = 0;
            upper[i][j] = 0;
        }
    }
    //把測試地圖地雷周圍8個點+1
    for (int i = 0; i < variable.size();i++){
        if(variable[i].assignment==1){
            for (int j = 0; j < 8;j++){
                if(valid(variable[i].x+dx[j],variable[i].y+dy[j])){
                    test[variable[i].x + dx[j]][variable[i].y + dy[j]]+=1;
                }
            }
            mine++;
        }
    }
    //接下來要檢查unassign variable的upper and lower bound
    for (int i = 0; i < domain.size();i++){
        //如果domain可以選擇兩種assignment 就要去計算upper and lower bound
        if(domain[i].mine && domain[i].clear){
            for (int j = 0; j < 8;j++){
                if(valid(domain[i].x+dx[j],domain[i].y+dy[j])){
                    upper[domain[i].x + dx[j]][domain[i].y + dy[j]]+=1;
                }
            }
            mine_ready++;
        }
        //如果只剩下1可以assign 就直接加到test裡面
        else if(domain[i].mine && !domain[i].clear){
            for (int j = 0; j < 8;j++){
                if(valid(domain[i].x+dx[j],domain[i].y+dy[j])){
                    test[domain[i].x + dx[j]][domain[i].y + dy[j]]+=1;
                }
            }
            mine++;
        }
    }
    //如果目前的mine加上還可以被assign的mine不到global constraint return false(upper bound比constraint小)
    if(mine+mine_ready<MINE){
        return false;
    }
    //如果目前的mine(包含domain只能選1的)已經超過global constraint的話 return false(lower bound比constraint大)
    else if(mine>MINE){
        return false;
    }
    bool upperflag = false, lowerflag=false;
    for (int i = 0; i < ROW;i++){
        for (int j = 0; j < COLUMN;j++){
            if(map[i][j]>=0){
                //如果upper bound小於constraint return false
                if(test[i][j]+upper[i][j]<map[i][j]){
                    return false;
                }
                //如果lower bound大於constraint return false
                else if(test[i][j]>map[i][j]){
                    return false;
                }
                //如果所有都選domain最大值而且跟constraint相符的話 就把upperflag設成true
                else if(test[i][j]+upper[i][j]==map[i][j]){
                    upperflag = true;
                }
                //如果所有都選domain最小值而且跟constraint相符的話 就把lowerflag設成true
                else if(test[i][j]==map[i][j]){
                    lowerflag = true;
                }
                else{
                    upperflag = false;
                    lowerflag = false;
                    return true;
                }
            }
        }
    }
    if(mine+mine_ready==MINE && upperflag){
        for (int i = 0; i < domain.size();i++){
            if(domain[i].domain_size==2){
                domain[i].domain_size = 1;
                domain[i].clear = false;
            }
        }
    }
    else if(mine==MINE && lowerflag){
        for (int i = 0; i < domain.size();i++){
            if(domain[i].domain_size==2){
                domain[i].domain_size = 1;
                domain[i].mine = false;
            }
        }
    }
    return true;
}
//MRV跟Degree Heuristic都做
void both(vector<dom> &domain){
    sort(domain.begin(), domain.end(), [](const dom &a, const dom &b) { 
        if(a.domain_size==b.domain_size){
            return a.degree > b.degree;
        }
        return a.domain_size < b.domain_size;
    });
}
//Degree Heuristic本體
void DegreeHeuristic(vector<dom> &domain){
    sort(domain.begin(), domain.end(), [](const dom &a, const dom &b) { return a.degree > b.degree; });
}
//MRV本體
void MRV(vector<dom> &domain){
    sort(domain.begin(), domain.end(), [](const dom &a, const dom &b) { return a.domain_size < b.domain_size; });
}
//根據heuristic來排序還沒被指派值的變數
void sorting(vector<dom> &domain, bool MRVflag, bool DHflag){
    for (int i = 0; i < domain.size();i++){
        int siz = 0, constraint=0;
        //計算domain size
        if(domain[i].mine){
            siz++;
        }
        if(domain[i].clear){
            siz++;
        }
        //計算這個variable的constraint
        for (int j = 0; j < 8;j++){
            if(valid(domain[i].x+dx[j],domain[i].y+dy[j])){
                constraint++;
            }
        }
        domain[i].domain_size = siz;
        domain[i].degree = constraint;
    }
    //依照flag來決定要怎麼排序
    if(DHflag && MRVflag){
        both(domain);
    }
    else if(DHflag && ! MRVflag){
        DegreeHeuristic(domain);
    }
    else if(!DHflag && MRVflag){
        MRV(domain);
    }
}
//LCV本體
bool LCV(vector<node> variable, vector<dom> domain, int &less, int assignment){
    //初始化測試地圖
    int test[6][6];
    for (int i = 0; i < 6;i++){
        for (int j = 0; j < 6;j++){
            test[i][j] = 0;
        }
    }
    //把測試地圖地雷周圍8個點+1
    for (int i = 0; i < variable.size();i++){
        if(variable[i].assignment==1){
            for (int j = 0; j < 8;j++){
                if(valid(variable[i].x+dx[j],variable[i].y+dy[j])){
                    test[variable[i].x + dx[j]][variable[i].y + dy[j]]+=1;
                }
            }
        }
    }
    //如果要assign成1 就要在周圍8個點+1
    if(assignment==1){
        for (int j = 0; j < 8;j++){
            int tmp_x = domain[0].x + dx[j], tmp_y = domain[0].y + dy[j];
            if(valid(tmp_x,tmp_y)){
                test[tmp_x][tmp_y] += 1;
            }
        }
    }
    //檢查
    for(int i = 1; i < domain.size();i++){
        if(domain[i].mine){
            for (int j = 0; j < 8;j++){
                int tmp_x = domain[i].x + dx[j], tmp_y = domain[i].y + dy[j];
                if(valid(tmp_x,tmp_y)){
                    if(test[tmp_x][tmp_y]>=map[tmp_x][tmp_y] && map[tmp_x][tmp_y]){
                        if(domain[i].clear){
                            less++;
                        }
                        else{
                            return true;
                        }
                        break;
                    }
                }
            }
        }
    }
    return false;
}
//Backtrack Search的本體
void searching(vector<node> variable, vector<dom> domain, int &counting, bool &flag, bool MRVflag, bool DHflag, bool LCVflag, bool FC){
    //如果找到答案 就return
    if(flag){
        return;
    }
    //如果Forward Checking是enable 就進行Forward Checking
    if(FC){
        if(!forwardChecking(variable,domain)){
            return;
        }
    }
    //如果MRV或DH是enable就去排序 看哪一個要先拿出來
    if(MRVflag||DHflag){
        sorting(domain,MRVflag,DHflag);
    }
    //如果domain沒有數字的話 就return
    if(domain[0].domain_size==0){
        return;
    }
    //現在要assign的variable
    int current_x = domain[0].x, current_y = domain[0].y;
    if(domain[0].domain_size==2){
        if(LCVflag){
            int less_a = 0, less_b = 0;
            bool zero_a = LCV(variable,domain,less_a,1);
            bool zero_b = LCV(variable,domain,less_b,0);
            //如廣兩個都會讓之後的variable的domain沒東西的話 就return
            if(zero_a && zero_b){
                return;
            }
            //如果發現assign成1會讓後面的variable domain沒東西或是影響較大的話 就設成0
            else if(zero_a){
                for (int k = 0; k < variable.size();k++){
                    if(variable[k].x==current_x && variable[k].y==current_y){
                        variable[k].assignment = 0;
                    }
                }
            }
            //如果發現assign成0會讓後面的variable domain沒東西或是影響較大的話 就設成1
            else if(zero_b){
                for (int k = 0; k < variable.size();k++){
                    if(variable[k].x==current_x && variable[k].y==current_y){
                        variable[k].assignment = 1;
                    }
                }
            }
            else if(less_a>less_b){
                for (int k = 0; k < variable.size();k++){
                    if(variable[k].x==current_x && variable[k].y==current_y){
                        variable[k].assignment = 0;
                    }
                }
            }
            else if(less_a<=less_b){
                for (int k = 0; k < variable.size();k++){
                    if(variable[k].x==current_x && variable[k].y==current_y){
                        variable[k].assignment = 1;
                    }
                }
            }
        }
        //如果沒有enable LCV的話 就先assign成1
        else{
            for (int k = 0; k < variable.size();k++){
                if(variable[k].x==current_x && variable[k].y==current_y){
                    variable[k].assignment = 1;
                }
            }
        }
    }
    else if(domain[0].mine){
        for (int k = 0; k < variable.size();k++){
            if(variable[k].x==current_x && variable[k].y==current_y){
                variable[k].assignment = 1;
            }
        }
    }
    else if(domain[0].clear){
        for (int k = 0; k < variable.size();k++){
            if(variable[k].x==current_x && variable[k].y==current_y){
                variable[k].assignment = 0;
            }
        }
    }
    vector<dom> tmp_domain;
    //還沒有assign value的variables
    for (int j = 0; j < domain.size();j++){
        if(!(domain[j].x==current_x && domain[j].y==current_y)){
            tmp_domain.push_back(domain[j]);
        }
    }
    //這邊要更新unassign variable的domain
    invalidAssignment(variable, tmp_domain);
    //透過計算不是variable的格子有沒有和constraint有沒有一樣
    if(isComplete(variable)){
        flag = true;
        printResult(variable);
        cout << "Time Complexity: " << counting << endl;
        return;
    }
    //準備進到下一層DFS
    if(!flag){
        //走過幾個node
        counting++;
        //如果還有還沒被assign的variable就近到下一層
        if(tmp_domain.size()>0){
            searching(variable, tmp_domain,counting,flag,MRVflag,DHflag,LCVflag,FC);
        }
        else if(tmp_domain.size()==0){
            return;
        }
        //如果還沒有找到答案 而且另一個值還沒被assign
        if(!flag && domain[0].domain_size==2){
            for (int k = 0; k < variable.size();k++){
                if(variable[k].x==current_x && variable[k].y==current_y){
                    //如果原本設成0的話 就把他設成1 準備再往下一層下去
                    if(variable[k].assignment == 0){
                        variable[k].assignment = 1;
                    }
                    //如果原本設成1的話 就把他設成0 準備再往下一層下去
                    else{
                        variable[k].assignment = 0;
                    }
                }
            }
            tmp_domain.clear();
            //還沒有assign value的variables
            for (int j = 0; j < domain.size();j++){
                if(!(domain[j].x==current_x && domain[j].y==current_y)){
                    tmp_domain.push_back(domain[j]);
                }
            }
            invalidAssignment(variable, tmp_domain);
            counting++;     //走過的node+1
            //如果還有還沒被assign的variable就近到下一層
            if(tmp_domain.size()>0){
                searching(variable, tmp_domain,counting,flag,MRVflag,DHflag,LCVflag,FC);
            }
        }
    }
}
int main(){
    int row = 0, column = 0, mine = 0;
    bool MRV = false, DH = false, LCV = false, FC = false;
    //輸入各種configuration的設定值
    cout << "Enter enable/disavle of each method(MRV, Degree Heuristic, LCV, Forward Checking):";
    cin >> MRV >> DH >> LCV >> FC;
    //input number of row, column, mine
    cout << "Enter the map(0,0,0 for exiting the program):";
    while(cin>>row>>column>>mine){
        if(row<=0||column<=0||mine<=0){
            break;
        }
        int counting = 0;       //看總共走了幾個node(time complexity)
        bool flag = false;      //有沒有找到答案的標誌
        vector<node> variable;  //儲存variables跟他們被assign的值
        vector<dom> domain;     //儲存還沒被assign的variable
        //input data
        for (int i = 0; i < row;i++){
            for (int j = 0; j < column;j++){
                cin >> map[i][j];
                //if the current node is a variable, then save it into variable[]
                if(map[i][j]==-1){
                    node tmp1(i,j,-1);
                    dom tmp2(i, j, true, true, 2);
                    variable.push_back(tmp1);
                    domain.push_back(tmp2);
                }
            }
        }
        //先把不行的domain刪掉
        invalidAssignment(variable, domain);
        //進入本體
        searching(variable,domain,counting,flag,MRV,DH,LCV,FC);
        //如果沒有找到答案
        if(!flag){
            cout << counting << endl;
            cout << "There is no solution to this map." << endl;
        }
        cout << endl;   //分開兩筆測資
        cout << "Enter the map(0,0,0 for exiting the program):";
    }
    return 0;
}

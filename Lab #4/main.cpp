//20200626 11:00 AM 完成SampleProcess(), Input()
//20200626 11:15 AM 完成TrainingDataSplit()
//20200626 11:40 AM 完成Output(), main()
//20200626 11:14 PM 完成Validation()
//20200627 12:43 AM 完成BuildForest(), tree class大多函數
//20200627 11:37 PM 完成GiniImpurity(), 成功輸出(要試一下更高的樹為什麼跑不出來)
//20200628 09:43 PM 完成debug以及解決跑不出來的問題(因為沒有處理分出來的是空的子集合)

#include <iostream>
#include <iomanip>      //for Output()
#include <fstream>      //for data input
#include <cstdlib>      //for rand()
#include <ctime>        //for rand()
#include <vector>
#include <queue>        //for BFS
#include <string>
#include <algorithm>    //for sort()

#define TRANING_RATIO 0.8   //the ratio of training data
#define SELECT_RATIO 0.6    //the ratio of selected data when build up a tree
#define TREE_HEIGHT 5       //maximum height of the trees
#define TREE_NUMBER 10      //number of trees in the forest
#define EXPERIMENT_TIME 30  //how many times we do the experiment
#define OUTPUT_NUMBER 3     //number of expected output

using namespace std;

int current_attribute;      //used in the sort() comparison function

//class of each sample
class Sample{
public:
    vector<float> attribute;
    string output;
    int output_num = 0;
};

//class of node in the tree
class node{
public:
    //depth for check if the tree is tall enough or not when we build up the tree
    //attribute for check the selected attribute in this node
    //attribute == -1 for leaf node
    //value for the leaf node storing the predicted output
    int depth = 0, attribute = -1, threshold = 0, value=0;
    node *left_child, *right_child;
};

//class of tree
class tree{
public:
    //data
    node *root;                     //store the root node
    vector<Sample> selected_data;   //store the selected data for this tree

    //function

    //for sort()
    static bool comparison(Sample &a , Sample &b){
        return a.attribute[current_attribute] < b.attribute[current_attribute];
    }
    //randomly select data from training data to train the tree(repeat data is allowed)
    void Select_data(vector<Sample> &training_data){
        srand(time(NULL));
        int select_size = training_data.size() * SELECT_RATIO;
        while(select_size>0){
            int random_number = rand() % training_data.size();
            selected_data.push_back(training_data[random_number]);
            select_size--;
        }
    }
    //find the Gini's Impurity of current attribute
    float Gini_Impurity(vector<Sample> &split_data, int attribute, float &min_threshold){
        current_attribute = attribute;
        sort(split_data.begin(), split_data.end(), comparison);
        float min_purity = 1;
        for (int i = 0; i < split_data.size()-1;i++){
            vector<int> left_vote, right_vote;
            left_vote.resize(OUTPUT_NUMBER,0);
            right_vote.resize(OUTPUT_NUMBER,0);
            int left = 0, right = 0;
            float current_threshold = (split_data[i].attribute[attribute] + split_data[i+1].attribute[attribute])/2;
            
            //counting
            for (int j = 0; j < split_data.size();j++){
                if(split_data[j].attribute[attribute]<current_threshold){
                    left_vote[split_data[j].output_num]+=1;
                    left++;
                }
                else{
                    right_vote[split_data[j].output_num]+=1;
                    right++;
                }
            }
            float left_0=float(left_vote[0])/float(left), left_1=float(left_vote[1])/float(left), left_2=float(left_vote[2])/float(left);
            float right_0=float(right_vote[0])/float(right), right_1=float(right_vote[1])/float(right), right_2=float(right_vote[2])/float(right);
            float G_left = 1 - ((left_0 * left_0) + (left_1 * left_1) + (left_2 * left_2));
            float G_right = 1 - ((right_0 * right_0) + (right_1 * right_1) + (right_2 * right_2));
            float current_purity = (float(left)/float(left+right)) * G_left + (float(right)/float(left+right)) * G_right;
            if(!(current_purity>=0 && current_purity<=1)){
                continue;
            }
            if(current_purity<min_purity){
                min_purity = current_purity;
                min_threshold = current_threshold;
            }
        }
        return min_purity;
    }
    //split the data into two subset to the next depth
    void NodeSplit(vector<Sample> &current, vector<Sample> &left, vector<Sample> &right, int attribute, float threshold){
        int left_size = 0, right_size = 0;
        for (int i = 0; i < current.size();i++){
            if(current[i].attribute[attribute]<threshold){
                left_size++;
            }
            else{
                right_size++;
            }
        }
        left.resize(left_size);
        right.resize(right_size);
        int index1 = 0, index2 = 0;
        for (int i = 0; i < current.size();i++){
            if(current[i].attribute[attribute]<threshold){
                left[index1]=current[i];
                index1++;
            }
            else{
                right[index2]=current[i];
                index2++;
            }
        }
    }
    //build up the tree using BFS
    void BuildTree(){
        queue<node*> node_queue;            //frontier of BFS
        queue<vector<Sample>> sample_queue; //frontier of BFS

        node *tmp = new node;   //the root node
        tmp->depth = 0;
        root = tmp;

        //push the node and the data into frontier
        node_queue.push(tmp);
        sample_queue.push(selected_data);

        while(!node_queue.empty()){
            //get the current node's information and expand it
            node *current_node = node_queue.front();
            node_queue.pop();
            vector<Sample> current_data = sample_queue.front();
            sample_queue.pop();

            //if the current node reach the maximum height
            if(current_node->depth==TREE_HEIGHT){
                current_node->attribute = -1;
                vector<int> vote;
                vote.resize(OUTPUT_NUMBER, 0);
                for (int i = 0; i < current_data.size();i++){
                    vote[current_data[i].output_num]++;
                }
                int highest_vote = 0, highest=0;
                for (int i = 0; i < OUTPUT_NUMBER;i++){
                    if(vote[i]>highest_vote){
                        highest_vote = vote[i];
                        highest = i;
                    }
                }
                current_node->value = highest;
                continue;
            }

            //split the data into two subset
            int min_attribute=0;
            float min_impurity = 1, min_threshold=1;
            //find the best attribute to split
            for (int i = 0; i < current_data[0].attribute.size();i++){
                float current_threshold = 0;
                float current_impurity = Gini_Impurity(current_data, i, current_threshold);
                if(current_impurity<min_impurity){
                    min_impurity = current_impurity;
                    min_attribute = i;
                    min_threshold = current_threshold;
                }
            }
            //if the min_impurity==0 means that the current datas all have same output
            if(min_impurity==0){
                current_node->attribute = -1;
                current_node->value = current_data[0].output_num;
                continue;
            }
            current_node->attribute = min_attribute;
            current_node->threshold = min_threshold;

            //allocate new child nodes
            node *left_child = new node;
            node *right_child = new node;
            left_child->depth = current_node->depth + 1;
            right_child->depth = current_node->depth + 1;
            current_node->left_child = left_child;
            current_node->right_child = right_child;
            
            
            //split the data into two subset
            vector<Sample> left_data;
            vector<Sample> right_data;
            NodeSplit(current_data, left_data, right_data, current_node->attribute, current_node->threshold);

            //if one of the node's data has no data, then just push another one into frontier
            if(left_data.size()==0){
                node_queue.push(right_child);
                sample_queue.push(right_data);
                continue;
            }
            if(right_data.size()==0){
                node_queue.push(left_child);
                sample_queue.push(left_data);
                continue;
            }

            //push the child node into frontier
            node_queue.push(left_child);
            node_queue.push(right_child);
            sample_queue.push(left_data);
            sample_queue.push(right_data);
            current_data.clear();
        }
    }
};

//processing the input samples
vector<Sample> SampleProcess(vector<string> &sample_buffer){
    vector<Sample> result;
    result.resize(sample_buffer.size());
    //processing each sample
    for (int i = 0; i < sample_buffer.size();i++){
        Sample current;
        bool point = false;
        float tmp=0, current_precision=0.1;
        for (int j = 0; j < sample_buffer[i].size();j++){
            if(sample_buffer[i][j]=='.'){
                point = true;
            }
            else if(sample_buffer[i][j]==','){
                point = false;
                current.attribute.push_back(tmp);
                tmp=0, current_precision=0.1;
            }
            else if(sample_buffer[i][j]>='A'&&sample_buffer[i][j]<='Z'){
                string buffer = "";
                while(j<sample_buffer[i].size()){
                    buffer += sample_buffer[i][j];
                    j++;
                }
                current.output = buffer;
            }
            else if(sample_buffer[i][j]>='a'&&sample_buffer[i][j]<='z'){
                string buffer = "";
                while(j<sample_buffer[i].size()){
                    buffer += sample_buffer[i][j];
                    j++;
                }
                current.output = buffer;
            }
            else{
                if(point){
                    tmp += (sample_buffer[i][j] - '0') * current_precision;
                    current_precision *= 0.1;
                }
                else if(!point){
                    tmp *= 10;
                    tmp += (sample_buffer[i][j] - '0');
                }
            }
        }
        if(current.output=="Iris-setosa"){
            current.output_num = 0;
        }
        else if(current.output=="Iris-versicolor"){
            current.output_num = 1;
        }
        else if(current.output=="Iris-virginica"){
            current.output_num = 2;
        }
        result[i] = current;
    }
    return result;
}

//input data
vector<Sample> Input(){
    fstream file;
    string line;
    vector<string> sample_buffer;

    file.open("iris.data", ios::in);
    if(!file){
        cout << "Reading file failed!" << endl;
    }
    else{
        while(getline(file,line)){
            if(line==""){
                continue;
            }
            sample_buffer.push_back(line);
        }
        file.close();
    }
    return SampleProcess(sample_buffer);
}

//split data into two subset randomly
void TrainingDataSplit(vector<Sample> &origin, vector<Sample> &sub1, vector<Sample> &sub2){
    //set up random number seed
    srand(time(NULL));
    
    //determine the size of subset
    int subset_size = origin.size() * TRANING_RATIO;
    sub1.resize(subset_size);
    sub2.resize(origin.size() - sub1.size());

    //build up flag to check if the current 
    vector<bool> flag;
    flag.resize(origin.size(), false);
    //put data into subset 1
    int index = 0;
    while(index<subset_size){
        int random_number = rand() % origin.size();
        if(!flag[random_number]){
            sub1[index]=origin[random_number];
            flag[random_number] = true;
            index++;
        }
    }
    //put data into subset 2
    index = 0;
    for (int i = 0; i < origin.size();i++){
        if(!flag[i]){
            sub2[index]=origin[i];
            index++;
        }
    }
}

//build up random forest
void BuildForest(vector<tree> &forest, vector<Sample> &training_data){
    forest.resize(TREE_NUMBER);
    for (int i = 0; i < TREE_NUMBER;i++){
        tree current;                       //declear new tree object
        current.Select_data(training_data); //randomly select data from split training data
        current.BuildTree();                //build up the tree
        forest[i] = current;                //store the tree object into forest
    }
}

//validate the forest by validation data
float Validation(vector<tree> &forest, vector<Sample> &validation_data){
    int correct = 0, wrong = 0;
    for (int i = 0; i < validation_data.size();i++){
        vector<int> vote;
        vote.resize(OUTPUT_NUMBER, 0);
        //traverse the trees and predict the output
        for (int j = 0; j < forest.size();j++){
            node *current = forest[j].root;
            while(current->attribute!=-1){
                int selected_attribute = current->attribute;
                int threshold = current->threshold;
                if(validation_data[i].attribute[selected_attribute]>threshold){
                    current = current->right_child;
                }
                else{
                    current = current->left_child;
                }
            }
            vote[current->value]++;
        }
        //find the highest vote and check if it is correct or not
        int highest_times = 0, highest_output=0;
        for (int j = 0; j < vote.size();j++){
            if(vote[j]>highest_times){
                highest_times = vote[j];
                highest_output = j;
            }
        }
        if(validation_data[i].output_num==highest_output){
            correct++;
        }
        else{
            wrong++;
        }
    }
    return (float(wrong)/(float(correct+wrong)));
}

//output the experiment result
void Output(float validation_error){
    cout << endl;
    cout << "Training Ratio: " << TRANING_RATIO << endl;
    cout << "Maximum Tree Height: " << TREE_HEIGHT << endl;
    cout << "Number of Tree in Forest: " << TREE_NUMBER << endl;
    cout << "Experiment Time: " << EXPERIMENT_TIME << endl;
    cout << "Average Validation Error: " << fixed <<setprecision(2) << (validation_error / EXPERIMENT_TIME)*100 << "%" << endl;
}

//main function
int main(){
    vector<Sample> data=Input();
    int exp = 0;
    float validation_error = 0;
    while(exp<EXPERIMENT_TIME){
        //spliting data into two subset, one for training and another for validation
        vector<Sample> training_data, validation_data;
        TrainingDataSplit(data,training_data,validation_data);

        //build up the forest
        vector<tree> forest;
        BuildForest(forest, training_data);

        //find the current experiment's validation error
        validation_error += Validation(forest, validation_data);
        exp++;
    }
    Output(validation_error);
    return 0;
}
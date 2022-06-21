#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
using namespace std;
struct state
{
    int products_count;
    vector<string> products[100];
};
vector<string> non_terminal;
vector<string> terminal;
void find_branch(struct state *I, int index, int i){
    string left = I->products[index][0];
    I->products[index+1].push_back(left);
    I->products[index+1].push_back(I->products[index][1]);
    int size = I->products[index].size();
    for(int j=i; j<size-1; j++){
        string right = I->products[index][j+1];
        I->products[index+1].push_back(right);
        I->products[index].pop_back();
    }
    I->products[index].pop_back();
    I->products_count++;
    for(int i=0; i<I->products[I->products_count].size(); i++){
        if ((int)I->products[I->products_count][i][0] > 64 && (int)I->products[I->products_count][i][0] < 91)
            {
                non_terminal.push_back(I->products[I->products_count][i]);
            }
            else if((int)I->products[I->products_count][i][0] == 124){
                // '||' 등장시 문법 생성
                find_branch(I, I->products_count, i);
            }
            else if(I->products[I->products_count][i] == "==>"){
            }
            else{
                terminal.push_back(I->products[I->products_count][i]);
            }
    }
}
void state_setting(struct state *I)
{
    ifstream File;
    File.open("test.txt");
    if(!File){
        cerr << "파일을 찾을 수 없음" << endl;
        exit(100);
    }
    I->products_count = 0;
    while (!File.eof())
    {
        int index = I->products_count;
        string str, stringbuffer;
        getline(File, str);
        istringstream ss(str);
        // 공백 단위로 문법 분류 및 상태에 저장 ex> E/==>/E/+/T
        while (getline(ss, stringbuffer, ' '))  
        {
            I->products[index].push_back(stringbuffer);
        }
        for(int i=0; i<I->products[index].size(); i++){
            if ((int)I->products[index][i][0] > 64 && (int)I->products[index][i][0] < 91)
            {
                non_terminal.push_back(I->products[index][i]);
            }
            else if((int)I->products[index][i][0] == 124){
                // '||' 등장시 문법 생성
                find_branch(I, index, i);
                break;
            }
            else if(I->products[index][i] == "==>"){
            }
            else{
                terminal.push_back(I->products[index][i]);
            }
        }
        
        I->products_count++;
    }
    //논터미널 및 터미널 중복 제거
    sort(non_terminal.begin(), non_terminal.end());
    non_terminal.erase(unique(non_terminal.begin(), non_terminal.end()),non_terminal.end());

    sort(terminal.begin(), terminal.end());
    terminal.erase(unique(terminal.begin(), terminal.end()),terminal.end());
}

void get_first()
{
}
void get_follow(){
    
}
int main()
{
    struct state init;
    state_setting(&init);
    for(int i=0; i<init.products_count; i++){
        for(int j=0; j<init.products[i].size(); j++){
            string str = init.products[i][j];
            cout << str << " ";
        }
        cout << endl;
    }
    for(int i=0; i<non_terminal.size(); i++){
        cout << non_terminal[i] << " ";
    }
    for(int i=0; i<terminal.size(); i++){
        cout << terminal[i] << " ";
    }
    
}
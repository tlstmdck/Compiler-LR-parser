#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <string.h>
using namespace std;
struct state
{
    int products_count;
    vector<string> products[100];
};
vector<string> non_terminal;
vector<string> terminal;
vector<string> first[30];
vector<string> follow[30];
vector<state> Is;
struct state newI;
bool is_non_terminal(char ch){
    if((int)ch > 64 && (int)ch < 91)
        return true;
    else
        return false;
}
int find_index(string str){
    for(int i=0; i<non_terminal.size(); i++){
        if(non_terminal[i] == str)
            return i;
    }
    for(int i=0; i<terminal.size(); i++){
        if(terminal[i] == str)
            return i;
    }
}

int is_vector_have_Element(vector<string> vec, string str){
    int flag = 0;
    int index = 0;
    for(int i=0; i<vec.size(); i++){
        if(vec[i] == str){
            flag = 1;
            index = i;
            break;
        }
    }
    if(!flag)
        return -1;
    else
        return index;
}
vector<string> add_vector_Element(vector<string> vec, string str){
    if(is_vector_have_Element(vec,str) == -1 )
        vec.push_back(str);
    return vec;
}
vector<string> get_union(vector<string> vec1, vector<string> vec2){
    vector<string> result = vec1;
    for(int i=0; i<vec2.size(); i++){
        result = add_vector_Element(result, vec2[i]);
    }
    return result;
}
vector<string> ring_sum(vector<string> vec1, vector<string> vec2){
    if(is_vector_have_Element(vec1, "__") == -1){   //앱실론 없을때
        return vec1;
    }
    else{   //앱실론 있을때
        vector<string> temp = vec1;
        temp.erase(temp.begin()+is_vector_have_Element(temp,"__"));
        return get_union(temp, vec2); 
    }
}
int find_dot_index(vector<string> product){
    for(int i=0; i<product.size(); i++){
        if(product[i] == ".")
            return i;
    }
    return -1;
}
int find_state_index(struct state I, vector<string> product){
    for(int i=0; i<I.products_count; i++){
        if(I.products[i] == product){
            return i;
        }
    }
    return -1;
}
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
        if (is_non_terminal(I->products[I->products_count][i][0]))
            {
                non_terminal = add_vector_Element(non_terminal, I->products[I->products_count][i]);
            }
            else if((int)I->products[I->products_count][i][0] == 124){
                // '||' 등장시 문법 생성
                find_branch(I, I->products_count, i);
            }
            else if(I->products[I->products_count][i] == "==>"){
            }
            else{
                terminal = add_vector_Element(terminal, I->products[I->products_count][i]);
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
            if (is_non_terminal(I->products[I->products_count][i][0]))
            {
                non_terminal = add_vector_Element(non_terminal, I->products[index][i]);
            }
            else if((int)I->products[index][i][0] == 124){
                // '||' 등장시 문법 생성
                find_branch(I, index, i);
                break;
            }
            else if(I->products[index][i] == "==>"){
            }
            else{
                terminal = add_vector_Element(terminal, I->products[index][i]);
            }
        }
        
        I->products_count++;
    }
    //논터미널 및 터미널 중복 제거
    sort(terminal.begin(), terminal.end());
    terminal.erase(unique(terminal.begin(), terminal.end()),terminal.end());
}

void add_first(struct state *I, string first_arg, int product_index){
    for(int i=product_index+1; i<I->products_count; i++){
        if(first_arg == I->products[i][0]){
            if(!is_non_terminal(I->products[i][2][0])){
                first[find_index(I->products[i][0])] = add_vector_Element(first[find_index(I->products[i][0])], I->products[i][2]);
            }
            else{
                add_first(I, I->products[i][2], i);
                if(I->products[i].size() <= 3){
                    first[find_index(I->products[i][0])] = get_union(first[find_index(I->products[i][0])], first[find_index(I->products[i][2])]);
                }
                else{
                    vector<string> first_temp;
                    first_temp = first[find_index(I->products[i][2])];
                    for(int j=3; j<I->products[i].size(); j++){
                        first_temp = ring_sum(first_temp, first[find_index(I->products[i][j])]);
                    }
                }
            }
            //
            
        }      
    }
    
}
void add_follow(struct state *I, string follow_arg, int product_index){
    for(int i=product_index+1; i<I->products_count; i++){
        int prod_end_index = I->products[i].size() - 1;
        if(follow_arg == I->products[i][prod_end_index]){
            add_follow(I, I->products[i][prod_end_index], i);
            follow[find_index(I->products[i][prod_end_index])] = get_union(follow[find_index(I->products[i][prod_end_index])] , follow[find_index(I->products[i][0])]);
        }
    }
}
void get_first(struct state *I){
    for(int i=0; i<non_terminal.size(); i++){
        for(int j=0; j<I->products_count; j++){
            if(I->products[j][0] == non_terminal[i]){
                if(!is_non_terminal(I->products[j][2][0])){
                    first[i] = add_vector_Element(first[i], I->products[j][2]);
                }
            }
        }
    }
    for(int i=0; i<terminal.size(); i++){
        first[i+non_terminal.size()].push_back(terminal[i]);
    }
    for(int i=0; i<non_terminal.size(); i++){
        add_first(I, non_terminal[i], -1);
    }
    

}
void get_follow(struct state *I){
    follow[0].push_back("$");
    for(int i=0; i<non_terminal.size(); i++){
        for(int j=0; j<I->products_count; j++){
            for(int k=2; k<I->products[j].size(); k++){
                if(I->products[j][k] == non_terminal[i] && I->products[j].size() != k+1){
                    if(!is_non_terminal(I->products[j][k+1][0])){
                        follow[i] = add_vector_Element(follow[i], I->products[j][k+1]);
                    }
                    else{
                        follow[i] = get_union(follow[i], first[find_index(I->products[j][k+1])]);
                        for(int l=0; l<follow[i].size(); l++){
                            if(follow[i][l] == "__")
                                follow[i].erase(follow[i].begin()+l);
                        }
                        
                    }
                }
            }
            
        }
    }
    for(int i=0; i<terminal.size(); i++){
        follow[i+non_terminal.size()].push_back(terminal[i]);
    }

    for(int i=0; i<non_terminal.size(); i++){
        add_follow(I, non_terminal[i], -1);
    }
}
struct state increment(struct state I){
    newI.products->clear();
    newI.products_count=0;
    if(I.products[0][0] == "S"){
        vector<string> vec;
        vec.push_back("S`");
        vec.push_back("==>");
        vec.push_back("S");
        newI.products[0] = vec;
    }
    else{
        vector<string> vec;
        vec.push_back("S");
        vec.push_back("==>");
        vec.push_back(I.products[0][0]);
        newI.products[0] = vec;
    }
    newI.products[0].insert(newI.products[0].begin()+2, ".");
    for(int i=1; i<I.products_count; i++){
        newI.products[i] = I.products[i-1];
        newI.products[i].insert(newI.products[i].begin()+2, ".");
    }
    newI.products_count = I.products_count + 1;
    newI.products[newI.products_count-1] = I.products[I.products_count-1];
    newI.products[newI.products_count-1].insert(newI.products[newI.products_count-1].begin()+2, ".");
    
    return newI;
}
struct state ClOSURE(struct state I, vector<string> product){
    newI.products->clear();
    newI.products_count=0;
    newI.products[0] = I.products[0];
    newI.products_count++;
    for(int i=0; i<I.products_count; i++){  
        string after_dot = I.products[i][find_dot_index(I.products[i])+1];
        if(is_non_terminal(after_dot[0])){
            for(int j=0; j<I.products_count; j++){
                if(I.products[j][0]==after_dot){
                    if(find_state_index(newI,I.products[j]) == -1){
                        newI.products[newI.products_count] = I.products[j];
                        
                        newI.products_count++; 
                    }
                }
            }
        }
    }

    return newI;
}
struct state GOTO(struct state I, string goto_str){
    newI.products->clear();
    newI.products_count=0;
    for(int i=0; i<I.products_count; i++){
        if(find_dot_index(I.products[i]) != I.products[i].size()-1){
            if(I.products[i][find_dot_index(I.products[i])+1] == goto_str){
                ClOSURE(I,)
            }
        }
        
    }

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
    cout << endl;
    get_first(&init);
    for(int i=0; i<non_terminal.size(); i++){
        cout << "First(" << non_terminal[i] << ")" <<" : ";
        for(int j=0; j<first[i].size(); j++){
            cout << first[i][j] << " ";
        }
        cout << endl;
    }

    get_follow(&init);
    cout << endl;
    for(int i=0; i<non_terminal.size(); i++){
        cout << "Follow(" << non_terminal[i] << ")" <<" : ";
        for(int j=0; j<follow[i].size(); j++){
            cout << follow[i][j] << " ";
        }
        cout << endl;
    }
    for(int i=0; i<terminal.size(); i++){
        cout << terminal[i] << " ";
    }
    
    
    Is[0] = increment(init);
    
    Is[1] = ClOSURE(Is[0], Is[0].products[0]);
    cout << endl;
    for(int i=0; i<Is[1].products_count; i++){
        for(int j=0; j<Is[1].products[i].size(); j++){
            string str = Is[1].products[i][j];
            cout << str << " ";
        }
        cout << endl;
    }
}
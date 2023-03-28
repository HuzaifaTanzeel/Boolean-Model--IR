#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <stdio.h>
#include <stack>
#include <variant>
#include <bitset>
#include <cstdlib>
using namespace std;

string stem_porter(string word);

vector<int> intersection(const vector<int> &docid_list1, const vector<int> &docid_list2) // Find intersection of 2 lists
{
    vector<int> answer;
    int i = 0, j = 0;
    while (i < docid_list1.size() && j < docid_list2.size())
    {
        if (docid_list1[i] == docid_list2[j])
        {
            answer.push_back(docid_list1[i]);
            i += 1;
            j += 1;
        }
        else if (docid_list1[i] < docid_list2[j])
        {
            i += 1;
        }
        else
        {
            j += 1;
        }
    }
    return answer;
}

int find_index(const vector<pair<string, vector<pair<int, vector<int>>>>> &dic, const string &target) // Find index of term
{
    int index = -1;
    for (int i = 0; i < dic.size(); i++)
    {
        if (dic[i].first == target)
        {
            index = i;
            break;
        }
    }
    if (index != -1)
    {
        return index;
    }
    else
    {

        return -1;
    }
}

vector<int> proximity_query(const vector<pair<string, vector<pair<int, vector<int>>>>> &dic, const string &term1, string &term2, int k)
{
    int dic_ind1 = find_index(dic, term1);
    int dic_ind2 = find_index(dic, term2);

    vector<pair<int, vector<int>>> list1 = dic[dic_ind1].second;
    vector<pair<int, vector<int>>> list2 = dic[dic_ind2].second;

    vector<int> result;
    int i = 0, j = 0;
    while (i < list1.size() && j < list2.size())
    {
        if (list1[i].first == list2[j].first)
        {
            int fl = 0;
            // answer.push_back(docid_list1[i]);
            for (int m = 0; m < list1[i].second.size(); m++)
            {
                for (int n = 0; n < list2[j].second.size(); n++)
                {

                    if ((abs(list1[i].second[m] - list2[j].second[n]) - 1) == k || (abs(list1[i].second[m] - list2[j].second[n]) - 1) < k)
                    {
                        result.push_back(list1[i].first);
                        fl = 1;
                        break;
                    }
                }
                if (fl == 1)
                {
                    fl = 0;
                    break;
                }
            }
            i += 1;
            j += 1;
        }
        else if (list1[i].first < list2[j].first)
        {
            i += 1;
        }
        else
        {
            j += 1;
        }
    }

    return result;
}

vector<int> makeunion(const vector<int> &list1, const vector<int> &list2) // union of two lists
{
    vector<int> result;
    int i = 0, j = 0;
    while (i < list1.size() || j < list2.size())
    {
        if (i < list1.size() && j < list2.size() && list1[i] == list2[j])
        {
            result.push_back(list1[i]);
            i++;
            j++;
        }
        else if (i < list1.size() && (j == list2.size() || list1[i] < list2[j]))
        {
            result.push_back(list1[i]);
            i++;
        }
        else if (j < list2.size() && (i == list1.size() || list2[j] < list1[i]))
        {
            result.push_back(list2[j]);
            j++;
        }
    }
    return result;
}

vector<int> find_docs(const vector<pair<string, vector<pair<int, vector<int>>>>> &dic, const string &target) // Finding docids of list in logn
{
    int left = 0;
    int right = dic.size() - 1;
    vector<int> listt;

    while (left <= right)
    {
        int mid = (left + right) / 2;
        if (dic[mid].first == target)
        {
            for (int i = 0; i < dic[mid].second.size(); i++)
            {
                listt.push_back(dic[mid].second[i].first);
            }

            return listt;
        }
        else if (dic[mid].first < target)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }
    return listt;
}

bool is_operator(const string &token)
{
    return token == "and" || token == "or" || token == "not";
}

vector<int> query_process(const vector<pair<string, vector<pair<int, vector<int>>>>> &dic, vector<string> query_tokens)
{ // Processing query
    vector<int> result;
    stack<variant<vector<int>, string>> obj;

    for (int i = 0; i < query_tokens.size(); i++)
    {
        if (!is_operator(query_tokens[i])) // If token is an operand
        {
            vector<int> doc_ids = find_docs(dic, query_tokens[i]);
            obj.push(doc_ids);
        }
        else if (is_operator(query_tokens[i]))
        {
            if (query_tokens[i] == "not")
            {
                vector<int> notresult;
                vector<int> temp1 = get<vector<int>>(obj.top()); // Return the vector list from top of stack
                obj.pop();
                bitset<30> bits;
                for (int num : temp1) // This function is marking the nos which are present from 1-30 in O(n)
                {
                    bits.set(num - 1);
                }
                for (int i = 0; i < 30; i++)
                {
                    if (!bits.test(i))
                    {
                        notresult.push_back(i + 1);
                    }
                }
                obj.push(notresult);
            }
            else if (query_tokens[i] == "and")
            {
                vector<int> temp1 = get<vector<int>>(obj.top());
                obj.pop();
                vector<int> temp2 = get<vector<int>>(obj.top());
                obj.pop();
                vector<int> andresult = intersection(temp1, temp2);

                cout << endl;
                obj.push(andresult);
            }
            else if (query_tokens[i] == "or")
            {
                vector<int> temp1 = get<vector<int>>(obj.top());
                obj.pop();
                vector<int> temp2 = get<vector<int>>(obj.top());
                obj.pop();
                vector<int> orresult = makeunion(temp1, temp2);
                obj.push(orresult);
            }
        }
    }

    vector<int> final_answer = get<vector<int>>(obj.top()); // Return the vector list from top of stack
    obj.pop();

    return final_answer;
}

// Function to get the precedence of an operator
int precedence(const string &op)
{
    if (op == "not")
    {
        return 3;
    }
    else if (op == "and")
    {
        return 2;
    }
    else if (op == "or")
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Function to convert infix notation to postfix notation
vector<string> infix_to_postfix(const vector<string> &infix_tokens)
{
    vector<string> postfix_tokens;
    stack<string> op_stack;

    for (const string &token : infix_tokens)
    {
        if (!is_operator(token)) // If token is an operand
        {
            postfix_tokens.push_back(token);
        }
        else if (token == "(") // If token is a left parenthesis
        {
            op_stack.push(token);
        }
        else if (token == ")") // If token is a right parenthesis
        {
            while (!op_stack.empty() && op_stack.top() != "(")
            {
                postfix_tokens.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.pop(); // Pop the left parenthesis
        }
        else // If token is an operator
        {
            while (!op_stack.empty() && precedence(token) <= precedence(op_stack.top()))
            {
                postfix_tokens.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.push(token);
        }
    }
    while (!op_stack.empty()) // Pop remaining operators
    {
        postfix_tokens.push_back(op_stack.top());
        op_stack.pop();
    }

    return postfix_tokens;
}

bool is_stopword(const vector<string> &arr, const string &target)
{
    int left = 0;
    int right = arr.size() - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target)
        {

            return true;
        }
        else if (arr[mid] < target)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return false;
}

bool is_exist(const vector<pair<string, vector<pair<int, vector<int>>>>> &dic, const string &target)
{

    for (int i = 0; i < dic.size(); i++)
    {
        if (dic[i].first == target)
        {
            return true;
        }
    }

    return false;
}

vector<pair<string, vector<int>>> tokenize(string &data, const vector<string> &stop_word)
{
    bool flag = 0;
    int x;
    vector<pair<string, vector<int>>> tokens;
    // vector<string> tokens;
    string curr_token = "";
    transform(data.begin(), data.end(), data.begin(), ::tolower);

    int position = 1;
    vector<int> v;

    for (int i = 0; i < data.length(); i++)
    {
        int x = int(data[i]);

        if ((x >= 97 && x <= 122) && x != '\n')
        {

            curr_token += data[i];
        }
        else
        {
            if (is_stopword(stop_word, curr_token) && curr_token.length() > 1)
            {
                position++; // Mantaining positions by not neglecting stopwords
            }

            if ((!is_stopword(stop_word, curr_token)) && (curr_token != "") && curr_token.length() > 1)
            {

                string stemmed_token = stem_porter(curr_token);
                v.push_back(position);
                tokens.push_back(make_pair(stemmed_token, v)); // Add current token to list of tokens

                position++; // Mantaining positions by not neglecting stopwords
                v.clear();
            }

            curr_token = "";

            if (data[i] == '\0' || (data[i] == '.' && data[i + 1] == '\0'))
            {
                break;
            }
        }
    }

    return tokens;
}

bool ends_with(string word, string suffix)
{
    if (word.length() < suffix.length())
    {
        return false;
    }
    return equal(suffix.rbegin(), suffix.rend(), word.rbegin());
}

void replace_suffix(string &word, string suffix, string replacement)
{
    if (ends_with(word, suffix))
    {
        word.replace(word.length() - suffix.length(), suffix.length(), replacement);
    }
}

bool isvowel(char c)
{
    if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
    {
        return true;
    }
    else
    {
        return false;
    }
}

string stem_porter(string word)
{
    // Rule 1
    replace_suffix(word, "ses", "s");
    replace_suffix(word, "xes", "x");
    replace_suffix(word, "zes", "z");
    replace_suffix(word, "ches", "ch");
    replace_suffix(word, "shes", "sh");
    replace_suffix(word, "s", "");

    // Rule 2
    replace_suffix(word, "ied", "i");
    replace_suffix(word, "ed", "");
    // replace_suffix(word, "d", "");

    // Rule 3
    replace_suffix(word, "ying", "ie");
    replace_suffix(word, "ing", "");
    replace_suffix(word, "ly", "");
    replace_suffix(word, "east", "est");

    // Rule 4
    if (ends_with(word, "y") && word.length() > 2 && !string("aeiou").find(word[word.length() - 2]) != string::npos)
    {
        word[word.length() - 1] = 'i';
    }

    // Rule 5
    if (ends_with(word, "bb") || ends_with(word, "dd") || ends_with(word, "ff") || ends_with(word, "gg") || ends_with(word, "mm") || ends_with(word, "nn") || ends_with(word, "pp") || ends_with(word, "rr") || ends_with(word, "tt"))
    {
        word.erase(word.length() - 1, 1);
    }

    if (ends_with(word, "e") && word.length() > 4)
    {
        if (word.length() > 1 && !ends_with(word, "le"))
        {
            word.erase(word.length() - 1, 1);
        }
        else if (word.length() > 2 && ends_with(word, "le") && string("aeiou").find(word[word.length() - 3]) == string::npos)
        {
            word.erase(word.length() - 2, 2);
        }
    }

    if (ends_with(word, "ter") && word[word.length() - 4] != 't')
    {
        word.erase(word.length() - 2, 2);
    }

    if (ends_with(word, "ion") || (ends_with(word, "ain")))
    {
        word.erase(word.length() - 3, 3);
    }

    if (ends_with(word, "iliti") || ends_with(word, "le") && word.length() > 8)
    {
        if (ends_with(word, "iliti"))
        {
            word.erase(word.length() - 5, 5);
        }
        else
        {
            word.erase(word.length() - 2, 2);
        }
    }

    return word;
}

int main()
{
    vector<string> stop_word; // vector containg the stopwords
    string line;
    vector<pair<string, vector<int>>> tokens; // Temporary tokens vector to hold tokens of a document

    ifstream inFile;
    char chh;
    string stopdata = "";
    string token;

    // open the file
    inFile.open("Stopword-List.txt");

    // check if the file was opened successfully
    if (!inFile)
    {
        cout << "Unable to open file";
        return 0;
    }
    bool flagg = 0;
    // read the stopword file character by character
    while (inFile.get(chh))
    {
        if ((int(chh) < 97 || int(chh) > 122 || int(chh) == 32) && flagg == 0)
        {
            stop_word.push_back(stopdata);
            stopdata = "";
            flagg = 1;
        }
        else if (int(chh) >= 97 && int(chh) <= 122)
        {
            stopdata = stopdata + chh;
            flagg = 0;
        }
    }
    inFile.close();

    sort(stop_word.begin(), stop_word.end());

    // Document loop

    char ch;
    string filedata = "";

    vector<pair<string, vector<pair<int, vector<int>>>>> dic;

    for (int i = 1; i <= 30; i++)
    {
        filedata = "";
        string fileName = to_string(i) + ".txt";

        inFile.open(fileName);

        // check if the file was opened successfully
        if (!inFile)
        {
            cout << "Unable to open file " << i << endl;
            return 0;
        }

        // read the file character by character
        while (inFile.get(ch))
        {

            filedata = filedata + ch;
        }
        cout << "Readed file " << i << endl;
        inFile.close();

        tokens = tokenize(filedata, stop_word);
        //  cout << "Tokenized file " << i << " contents" << endl;

        // Making Inverted Index

        // vector<int> v;

        vector<pair<int, vector<int>>> v;

        for (int j = 0; j < tokens.size(); j++)
        {

            if (!is_exist(dic, tokens[j].first))
            {

                if (tokens[j].first.length() > 1)
                {
                    // vector<pair<string, vector<pair<int,vector<int> > > > > dic;
                    v.push_back(make_pair(i, tokens[j].second));
                    dic.push_back(make_pair(tokens[j].first, v));
                    // cout<<tokens[j].first<<endl;
                    v.clear();
                }
            }
            else
            {
                bool check = 0;
                int ind = find_index(dic, tokens[j].first);
                for (int l = 0; l < dic[ind].second.size(); l++)
                {
                    if (i == dic[ind].second[l].first)
                    {
                        for (int b = 0; b < tokens[j].second.size(); b++)
                        {
                            dic[ind].second[l].second.push_back(tokens[j].second[b]);
                        }

                        check = 1;
                        break;
                    }
                }

                if (check == 0)
                {
                    dic[ind].second.push_back(make_pair(i, tokens[j].second));
                }
            }
        }

        tokens.clear();

        //       cout << "FIle " << i << " contents inserted in dictionary" << endl;
    }

    sort(dic.begin(), dic.end()); // Sorting the dictionary so that we can apply binary search for logn Complexity

    /*for(int i=0;i<dic.size();i++)       //Iterating over terms in dictionary
    {
        cout<<"Term :  "<<dic[i].first<<endl;

        for(int j=0;j<dic[i].second.size();j++)         //terating over doc_ids of a particular term
        {
            cout<<"DOc Id : "<<dic[i].second[j].first<<"  ";
            cout<<"{ ";
            for(int k=0;k<dic[i].second[j].second.size();k++)
            {
                cout<<dic[i].second[j].second[k]<<" , ";

            }
            cout<<"} ";
            cout<<endl;
        }
    }*/

    while (1)
    {

        cout << "Enter type of query : " << endl;
        cout << "1) Boolean " << endl;
        cout << "2) Proximity Query " << endl;
        cout << "3)Exit " << endl;
        int ask;
        cin >> ask;
        cin.ignore();
        switch (ask)
        {
        case 1:
        {
            string query;
            cout << "Enter query : ";

            getline(cin, query);
            // cout<<query<<endl;
            string temp;
            vector<string> infix_tokens;
            for (int i = 0; i <= query.size(); i++)
            {
                int x = int(query[i]);
                if (((x >= 97 && x <= 122) || (x >= 65 && x <= 90)) && x != '\n')
                {

                    temp += tolower(query[i]);
                }
                else if (temp == "")
                {
                    cout << "Wrong format !!!" << endl;
                    break;
                }
                else
                {
                    infix_tokens.push_back(temp);
                    temp = "";
                }
            }

            for (int i = 0; i < infix_tokens.size(); i++)
            {
                cout << infix_tokens[i] << endl;
            }

            vector<string> postfix_tokens_stemmed;
            vector<string> postfix_tokens = infix_to_postfix(infix_tokens);
            for (int i = 0; i < postfix_tokens.size(); i++)
            {
                string tempp = postfix_tokens[i];
                if (!is_operator(tempp))
                {
                    string stemmed_post = stem_porter(tempp);

                    postfix_tokens_stemmed.push_back(stemmed_post);
                }
                else
                {
                    postfix_tokens_stemmed.push_back(tempp);
                }
            }

            for (int i = 0; i < postfix_tokens_stemmed.size(); i++)
            {
                cout << postfix_tokens_stemmed[i] << endl;
            }

            vector<int> answer = query_process(dic, postfix_tokens_stemmed);
            for (int i = 0; i < answer.size(); i++)
            {
                cout << answer[i] << " ,";
            }
            cout << endl;
            system("pause");
            break;
        }
        case 2:
        {
            cout << "Enter Proximity query : ";
            string prox_query;
            getline(cin, prox_query);
            string tempt;
            vector<string> terms;
            string distance = "";
            for (int i = 0; i <= prox_query.size(); i++)
            {
                int x = int(prox_query[i]);
                if ((x >= 97 && x <= 122) && x != '\n')
                {

                    tempt += tolower(prox_query[i]);
                }

                else if (x >= 48 && x <= 57)
                {
                    distance += prox_query[i];
                }
                else
                {
                    terms.push_back(tempt);
                    tempt = "";
                }
            }
            int k = std::stoi(distance);

            string t1 = stem_porter(terms[0]);
            string t2 = stem_porter(terms[1]);

            vector<int> prox_results = proximity_query(dic, t1, t2, k);

            cout << endl;
            for (int i = 0; i < prox_results.size(); i++)
            {
                cout << prox_results[i] << " , ";
            }
            cout << endl;
            system("pause");
            break;
        }
        default:
        {
            return 0;
            break;
        }
        }
    }

    return 0;
}

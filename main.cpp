#include <bits/stdc++.h>
#define ll long long int
#define dd long double
#define minsupport 0.40
using namespace std;
using namespace std::chrono;

void takeinputdataset(map<ll, set<int>> &mydataset)
{
    ifstream in("input.txt");
    string str;
    ll transid = 1;
    map<string, int> itemsmaping;
    int curr = 1;
    while (getline(in, str)) // Reads the file line by line  
    {
        if (str.size())
        {
            istringstream ss(str);  //Breaking the input string into word
            string word;
            ss >> word;
            while (ss)      //Loopung through each word in the line
            {
                int item = 0;
                if (word[0] >= '0' && word[0] <= '9')
                {
                    stringstream sst(word);
                    sst >> item;
                    mydataset[transid].insert(item); //Add each item in the set for corresponding transaction
                }
                ss >> word;
            }
            transid++;
        }
    }
}
//**************************************************************************************************************************************************
//**********************************************************FP-Growth***************************************************************************
struct node
{
    int count;
    int item;
    vector<node *> child;
    node *parent;
    map<int, int> childposition; //map<itemnumber, indexin_child_vector>
};

node *createnode()
{
    node *temp = new node;
    temp->count = 0;
    temp->parent = NULL;
    return temp;
}

void insertnode(node *&root, node *parent, vector<int> trans, int index, vector<node *> position[])
{
    if (index >= trans.size())
        return;
    else if (root == NULL)
    {
        root = createnode();
        root->item = trans[index];
        position[root->item].push_back(root);
        root->count++;
        root->parent = parent;
        if (index + 1 < trans.size())
        {
            root->child.push_back(NULL);
            root->childposition[trans[index + 1]] = (root->child.size()) - 1;
            insertnode(root->child[root->childposition[trans[index + 1]]], root, trans, index + 1, position);
        }
    }
    else
    {
        root->count++;
        if (index + 1 < trans.size())
        {
            if (root->childposition.count(trans[index + 1]) == 0)
            {
                root->child.push_back(NULL);
                root->childposition[trans[index + 1]] = (root->child.size()) - 1;
                insertnode(root->child[root->childposition[trans[index + 1]]], root, trans, index + 1, position);
            }
            else
                insertnode(root->child[root->childposition[trans[index + 1]]], root, trans, index + 1, position);
        }
    }
}
void showfptree(node *root)
{
    if (root == NULL)
        return;
    else
    {
        cout << "{ ";
        cout << root->item << " " << root->count << " }";
        for (int i = 0; i < root->child.size(); i++)
        {
            showfptree(root->child[i]);
        }
    }
}
bool compare(pair<int, ll> a, pair<int, ll> b)
{
    return a.second > b.second;
}

void takeinputfpdataset(map<ll, set<int>> mydataset, map<ll, vector<int>> &myfbdataset, dd total, int &max, vector<int> &supporder)
{
    map<int, ll> supcount;
    // Finding the support count for each item in the tranasaction dataset
    for (int i = 1; i <= mydataset.size(); i++)
    {
        for (auto itr = mydataset[i].begin(); itr != mydataset[i].end(); itr++)
        {
            //If item is not present in map,add the item into map and assign count 1 else increment the count
            if (supcount.count(*itr) == 0)  
                supcount[*itr] = 1;
            else
                supcount[*itr]++;
        }
    }
    pair<int, ll> flag;
    vector<pair<int, ll>> temp;
    for (auto itr = supcount.begin(); itr != supcount.end(); itr++)
    {
        dd check = itr->second / total;
        if (check >= minsupport)    //Check if item's support count is greater than minimum support 
        {
            flag.first = itr->first;
            flag.second = itr->second;
            temp.push_back(flag);
        }
    }
    sort(temp.begin(), temp.end(), compare); // Sorting the items based on the suppport count
    for (int j = 0; j < temp.size(); j++)
    {
        supporder.push_back(temp[j].first); //Storing the sorted items without support count
    }
    temp.clear();
    ll index = 1;
    for (int i = 1; i <= mydataset.size(); i++)
    {
        temp.clear();
        for (auto itr = mydataset[i].begin(); itr != mydataset[i].end(); itr++)
        {
            dd check = (supcount[*itr] / total);
            if (check >= minsupport)
            {
                flag.first = *itr;
                flag.second = supcount[*itr];
                temp.push_back(flag);
                if (max < *itr)
                    max = *itr;
            }
        }
        if (temp.size())
        {
            // Getting the ordered item set for each transaction
            sort(temp.begin(), temp.end(), compare);    
            for (int j = 0; j < temp.size(); j++)
            {
                myfbdataset[index].push_back(temp[j].first); //Store the modified orderd itemset in myfbdataset
            }
            index++;
        }
    }
}

void collect_all(node *item, map<int, pair<ll, set<pair<int, node *>>>> &countt, ll initial)
{
    if (item == NULL)
        return;
    pair<ll, set<pair<int, node *>>> p;
    pair<int, node *> tt;
    while (item->parent != NULL)
    {

        if (countt.count(item->parent->item) == 0)
        {
            p.first = initial;
            p.second.clear();
            tt.first = initial;
            tt.second = item->parent;
            p.second.insert(tt);
            countt[item->parent->item] = p;
        }
        else
        {
            countt[item->parent->item].first += initial;
            tt.first = initial;
            tt.second = item->parent;
            countt[item->parent->item].second.insert(tt);
        }
        item = item->parent;
    }
}

void fbfrequentsets(vector<vector<int>> &fbfreq, vector<int> curr, map<int, pair<ll, set<pair<int, node *>>>> count, dd total)
{
    for (auto itr = count.begin(); itr != count.end(); itr++)
    {
        dd check = (itr->second).first / total;
        if (check >= minsupport)
        {
            curr.push_back(itr->first);
            fbfreq.push_back(curr);
            map<int, pair<ll, set<pair<int, node *>>>> cc;
            for (auto it = (itr->second).second.begin(); it != (itr->second).second.end(); it++)
                collect_all((*it).second, cc, (*it).first);
            fbfrequentsets(fbfreq, curr, cc, total);
            curr.pop_back();
        }
    }
}
void display(map<ll, set<int>> mydataset)
{
   for (int i = 1; i <= mydataset.size(); i++)
    {
        for (auto itr = mydataset[i].begin(); itr != mydataset[i].end(); itr++)
        {
            cout<<*itr<<endl;
        }
    }
}

int main()
{
    map<ll, set<int>> mydataset;
    takeinputdataset(mydataset);
    dd avgwidth = 1;
    ll totalintems = 0;
    //display(mydataset);
    
    dd total_transactions = mydataset.size() * 1.0;
    ll totaltrans = total_transactions;
    cout << "Total number of transactions : " << totaltrans << "\n";
    cout << "Fp-Growth Algorithm\n";
    auto start = high_resolution_clock::now();
    map<ll, vector<int>> myfbdataset;
    vector<int> supporder;

    int max_nodevalue = 0;
    takeinputfpdataset(mydataset, myfbdataset, total_transactions, max_nodevalue, supporder);
    vector<node *> fptree(max_nodevalue + 1, NULL);
    vector<node *> position[max_nodevalue + 1];

            for (int i = 1; i <= myfbdataset.size(); i++)
            {
                if (myfbdataset[i].size())
                    insertnode(fptree[myfbdataset[i][0]], NULL, myfbdataset[i], 0, position);
            }
            map<int, vector<vector<int>>> fbfrequent;
            vector<int> curr;
            map<int, pair<ll, set<pair<int, node *>>>> count;
            for (int i = supporder.size() - 1; i >= 0; i--)
            {
                curr.clear();
                curr.push_back(supporder[i]);
                fbfrequent[supporder[i]].push_back(curr);
                count.clear();
                for (int j = 0; j < position[supporder[i]].size(); j++)
                {
                    collect_all(position[supporder[i]][j], count, position[supporder[i]][j]->count);
                }

                fbfrequentsets(fbfrequent[supporder[i]], curr, count, total_transactions);
                for (int j = 0; j < position[supporder[i]].size(); j++)
                {
                    node *tempp = position[supporder[i]][j];
                    if (tempp->parent)
                    {
                        int index = tempp->parent->childposition[tempp->item];
                        tempp->parent->child[index] = NULL;
                    }
                }
            }
            ll countt = 0;
            int size = 0;
            for (auto itr = fbfrequent.begin(); itr != fbfrequent.end(); itr++)
            {
                for (int j = 0; j < itr->second.size(); j++)
                {
                    countt++;
                    if (size < itr->second[j].size())
                        size = itr->second[j].size();
                }
            }
            cout << "All frequent item sets for the minsupport -> " << minsupport << ": " << countt << "\n";
            cout << "Size of maximum frequent item set : " << size << "\n";
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);

            cout << "Time taken by Fp-Growth: "
                 << duration.count() << " microseconds" << endl;
           
            freopen("output.txt", "w", stdout);
            for (auto itr = fbfrequent.begin(); itr != fbfrequent.end(); itr++)
            {
                for (int j = 0; j < itr->second.size(); j++)
                {
                    cout << "{ ";
                    for (int k = 0; k < itr->second[j].size(); k++)
                        cout << itr->second[j][k] << " ";
                    cout << "}";
                }
                cout << "\n";
            }
    return 0;
}
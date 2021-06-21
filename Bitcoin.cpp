#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

//struct to define each transaction
struct transaction
{
    string tx_id;
    int fee;
    int weight;
    string parents;
};

//Function to recursively calculate total fee and total weight of a particular tree of transactions
void calc_total_fee(string tx_id, double pps1, int *tw, int *tf, unordered_map <string, transaction*> &m)
{
    istringstream iss(m[tx_id]->parents);
    string temp;
  
    while( getline(iss, temp, ';'))
    {
        calc_total_fee(temp, pps1, tw, tf, m );
    }
    if(1.0* (m[tx_id]->fee)/m[tx_id]->weight < pps1)
        {
            *tf+=m[tx_id]->fee;
            *tw+=m[tx_id]->weight;
        }
}

//Function to recursively add the parents of a transaction into the list before the transaction itself
void trackparent(int *remainingweight, int *maxfees, string tx_id , vector<string> &finallist, unordered_map <string, transaction*> &m, unordered_map <string, int> &checklist)
{
    istringstream iss(m[tx_id]->parents);
    string temp;
    
    while( getline(iss, temp, ';'))
    {
        trackparent(remainingweight, maxfees, temp, finallist, m, checklist);
    }

    if(*remainingweight>=m[tx_id]->weight && checklist.find(tx_id)==checklist.end())
    {
        checklist[tx_id];
        finallist.push_back(tx_id);
        *remainingweight-=m[tx_id]->weight;
        *maxfees+=m[tx_id]->fee;
    }  
}

int main()
{
vector<transaction> mempool;

//Read from mempool.csv
ifstream fin("mempool.csv");
bool fla=false;
string temptx, tempfee, tempweight, tempparents, line;

while( getline(fin, line))
{
    istringstream iss(line);
    getline(iss, temptx, ',');
    getline(iss, tempfee, ',');
    getline(iss, tempweight, ',');
    getline(iss, tempparents, ',');

    if(fla)
    {    
        transaction tran;
        tran.tx_id = temptx;
        tran.fee = stoi(tempfee);
        tran.weight = stoi(tempweight);
        tran.parents = tempparents;
        mempool.push_back(tran);
    }
    fla=true;
}
//Reading complete and transactions stored in mempool vector

int i;
unordered_map <string, transaction*> m; // Hash map with tx_id as key so that searching takes O(1)

for(i=0;i<mempool.size();i++)
{
    m.insert({mempool[i].tx_id, &mempool[i]});
}

vector<pair<double,transaction*>> greedysort(mempool.size()); // vector to store fee per weight of transactions

for(i=0;i<mempool.size();i++) // calculating fee per weight of transactions
{
    int tw=mempool[i].weight,tf=mempool[i].fee;
    double pps1,pps2=0;
    pps1=1.0*mempool[i].fee/mempool[i].weight;
    
    calc_total_fee(mempool[i].tx_id, pps1, &tw, &tf, m);
    pps2=1.0*tf/(1.0*tw);
    greedysort[i]=make_pair(pps2,&mempool[i]);
}

sort(greedysort.begin(),greedysort.end()); // sorting transactions according to fee per weight

int remainingweight=4000000;
int maxfees=0;
unordered_map <string, int> checklist; // maintaining a check list to ensure that the same transaction does not enter the finallist twice
vector<string> finallist; // final list transactions to be included in the blobk

for(i=mempool.size()-1;i>=0;i--)  // Making the final list
{
    trackparent(&remainingweight, &maxfees, greedysort[i].second->tx_id, finallist, m, checklist);
}

cout<<"Mempool size = "<<mempool.size()<<"\n";
cout<<"Block size = "<<finallist.size()<<endl;
cout<<"Maximum fees = "<<maxfees<<endl;

ofstream myfile("block.txt");
for(i=0;i<finallist.size();i++)
{
    myfile << finallist[i] << "\n";
}

return 0;
}
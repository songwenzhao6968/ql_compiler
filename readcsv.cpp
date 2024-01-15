#include<bits/stdc++.h>
using namespace std;

int main()
{
    ifstream inFile("SLR1_table.csv", ios::in);
    if (!inFile)
    {
        cout << "OPEN FILE FAIL！" << endl;
        return -1;
    }
    int i = 0;
    int width = 33;
    string line;
    string field;
    vector<vector<string> > slr(100);
    while (getline(inFile, line))
    {
        string field;
        istringstream sin(line);
        for(int j=0;j<width;j++)
        {
            getline(sin, field, ',');
            cout<< field<<" ";
            slr[i].push_back(field);
        }
        i++;
    }
    inFile.close();
    cout << "Read " << i << " lines" << endl;
    cout << "---Load SLR1 FINISHED---" << endl;
    return 0;
}
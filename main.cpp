#include <iostream>
using namespace std;
#include <vector>
#include <string>
#include <fstream>
#include <set>
#include <sstream>
#include <algorithm>
using namespace std;
struct implicant
{
    set<int> mins;
    string binary;
    bool marked;

    bool operator==(implicant b)
    {
        return (this->mins == b.mins && this->binary == b.binary);
    }

    implicant()
    {
        mins = {0};
        binary = "";
        marked = false;
    }
    implicant(string b, int m)
    {
        binary = b;
        mins.insert(m);
        marked == false;
    }

    implicant(string bits, implicant &t1, implicant &t2)
    {
        mins.insert(t1.mins.begin(), t1.mins.end());
        mins.insert(t2.mins.begin(), t2.mins.end());
        marked = false;
        binary = bits;
    }
};

class QuineMclausky
{
public:
    QuineMclausky()
    {
    }
    ifstream infile;

    // vector<implicant> readtxt()
    vector<implicant> readtxt()
    {
        vector<implicant> result; // optional return
        string line;

        if (!infile.is_open())
        {
            cout << "Error: file is not open.\n";
            return result;
        }

        if (getline(infile, line))
        {
            numvar = stoi(line);
        }
        else
        {
            numvar = 0; // zero variables
            // return result;
        }

        if (getline(infile, line))
        {
            string integer = "";
            for (char a : line)
            {
                if (isdigit(a)) // example: mmmmgmhdm 100,mnjfsd50
                {
                    integer += a;
                }
                else if (!integer.empty()) // you arrived at a char after a bunch of digits, so it should check if you have any ints or not to flush them into the vector with stoi() of course
                {
                    minterms.push_back(stoi(integer));
                    integer = ""; // to make room for next ints
                }
            }

            if (!integer.empty()) // since when we finish the line there won't be any char to flush the final int, so this ensures it.
            {
                minterms.push_back(stoi(integer));
                integer = "";
            }
        }

        if (getline(infile, line))
        {
            string integer = "";

            for (char a : line) // same thing for the dontcares
            {
                if (isdigit(a))
                {
                    integer += a;
                }
                else if (!integer.empty())
                {
                    dontcares.push_back(stoi(integer));
                    integer = "";
                }
            }

            if (!integer.empty())
            {
                dontcares.push_back(stoi(integer));
                integer = "";
            }
        }

        return result;
    }

    bool isGreyCode(implicant a, implicant b)
    {
        int change = 0;
        for (int i = 0; i < a.binary.length(); i++)
        {
            if (a.binary[i] != b.binary[i])
            {
                change++;
            }
        }

        return change == 1;
    }
    string IntegerToBinary(int m,int NumberOfVariables) {

        string binary="";
        while (m>0) {
            int rem = m%2;
            if (rem==0) {
                binary += '0';
            }else
                binary += '1';

            m = m/2;
        }
        reverse(binary.begin(),binary.end());
        if (binary.length()<NumberOfVariables) {
            while (binary.length() < NumberOfVariables) {
                binary.insert(0, "0");
            }
        }

        return binary;
    }


    implicant merge(implicant a, implicant b)
    {
        string bits = a.binary;
        for (int i = 0; i < a.binary.length(); i++)
        {
            if (a.binary[i] != b.binary[i])
            {
                bits[i] = '_';
            }
        }
        return implicant(bits, a, b);
    }

    string IntegerToBinary(int m, int NumberOfVariables)
    {

        string binary = "";
        while (m > 0)
        {
            int rem = m % 2;
            if (rem == 0)
            {
                binary += '0';
            }
            else
                binary += '1';

            m = m / 2;
        }
        reverse(binary.begin(), binary.end());
        if (binary.length() < NumberOfVariables)
        {
            while (binary.length() < NumberOfVariables)
            {
                binary.insert(0, "0");
            }
        }

        return binary;
    }

    bool allIsNotMarked(vector<implicant> c)
    {
        for (int i = 0; i < c.size(); i++)
        {
            if (c[i].marked == true)
                return false;
        }
        return true;
    }

    // bool noGreyCodeFound(vector<implicant> c)
    // {
    //     for (int i = 0; i < c.size(); i++)
    //     {
    //         if (c[i].marked == true)
    //             return false;
    //     }
    //     return true;
    // }

    void matching()
    {
        readtxt();

        // uses readtxt() and merge()
        // needs to create the implicants int he first column first.

        columns.clear();
        columns.push_back(vector<implicant>()); // to make columns[0] available

        for (int i = 0; i < minterms.size(); i++)
        {
            implicant pi(IntegerToBinary(minterms[i], numvar), minterms[i]);
            columns[0].push_back(pi);
        }

        for (int i = 0; i < dontcares.size(); i++)
        {
            implicant pi(IntegerToBinary(dontcares[i], numvar), dontcares[i]);
            columns[0].push_back(pi);
        }

        // sorting (I believe may not be needed?)
        for (int i = 0; i < columns[0].size() - 1; i++) // simple selection sort alg.
        {
            int minIndex = i;
            for (int j = i + 1; j < columns[0].size(); j++)
            {
                int firstA = *(columns[0][minIndex].mins.begin());
                int firstB = *(columns[0][j].mins.begin());

                if (firstB < firstA)
                {
                    minIndex = j;
                }
            }
            if (minIndex != i)
            {
                swap(columns[0][i], columns[0][minIndex]);
            }
        }
        // now columns[0] is sorted

        int k = 0;

        bool GreyCodeFound = false;
        while (!GreyCodeFound && k < columns.size())
        {
            GreyCodeFound = true;
            for (int i = 0; i < columns[k].size() - 1; i++)
            {
                for (int j = i + 1; j < columns[k].size(); j++)
                {
                    if (isGreyCode(columns[k][i], columns[k][j]))
                    {
                        if (columns.size() <= k + 1) // to make sure that there is an available column to push_back into
                            columns.push_back(vector<implicant>());

                        columns[k + 1].push_back(merge(columns[k][i], columns[k][j]));
                        columns[k][i].marked = true;
                        columns[k][j].marked = true;
                        GreyCodeFound = false;
                    }
                }
            }
            k++;
            // cout << k << " "; // for testing
        }

        for (int i = columns.size() - 1; i >= 0; i--)
        {
            for (int j = 0; j < columns[i].size(); j++)
            {
                if (columns[i][j].marked == false)
                {
                    matched.push_back(columns[i][j]);
                }
            }
        }

        cout << "matching() is done!\n"; // for testing

        if (matched.empty())
            cout << "matched is empty!\n";
        else
            cout << "matched have things in it!\n";
        return;
    }

    void printAllFinalPIs()
    {
        cout << "\nAll the possible Prime Implicants are:\n\n";

        for (int i = 0; i < matched.size(); i++)
        {
            cout << "PI: (";
            for (int a : matched[i].mins)
            {
                cout << a << ", ";
            }
            cout << ") --> " << matched[i].binary << endl;
        }
    }

    void PITable()
    {
        matching();
        // uses matching to get the PI vector and minterms vector members modified and would use the printing fucntions later as final outputs

        // should create a vector<implicant> done/finalisedPIs,
        // to make printOutputExp() use it as the final out put and maybe printVerilogModule() also
    }

    void printOutputExp()
    {
    }

    void printVerliogModule()
    {
    }

    void printMembers() // for testing
    {
        cout << numvar << endl;
        for (int i = 0; i < minterms.size(); i++)
        {
            cout << minterms[i] << " ";
        }
        cout << endl;
        for (int i = 0; i < dontcares.size(); i++)
        {
            cout << dontcares[i] << " ";
        }
        cout << endl;
    }

private:
    int numvar;
    vector<int> minterms;
    vector<int> dontcares;

    vector<vector<implicant>> columns;
    vector<implicant> matched;
};

int main()
{
    QuineMclausky app;
    app.infile.open("C:\\Users\\Mohammad Dawood\\Desktop\\Digital Design I\\Project 1\\DD1-Project-1\\TestFiles\\Test4.txt");
    if (app.infile.is_open())
    {
        cout << "processing\n";
        app.readtxt();
        app.printMembers();
        cout << endl;

        app.matching();
        app.printAllFinalPIs();
    }
    else
    {
        perror("Error opening file");
    }
    return 0;
}
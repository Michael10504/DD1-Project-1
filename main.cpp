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
    implicant(string b,int m) {
        binary = b;
        mins.insert(m);
    }

    implicant( string bits, implicant &t1, implicant &t2 ) {
        mins.insert(t1.mins.begin(), t1.mins.end());
        mins.insert(t2.mins.begin(), t2.mins.end());
        marked=true;
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
        int change=0;
        for(int i=0;i<a.binary.length();i++) {
            if (a.binary[i] != b.binary[i]) {
                change++;
            }

        }

        return change==1;
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
        string bits=a.binary;
        for(int i=0;i<a.binary.length();i++) {
            if (a.binary[i] != b.binary[i]) {
                bits[i] = '-';
            }
        }
        return implicant(bits,a,b);
    }

    vector<implicant> matching()
    {
        readtxt();
        vector<implicant> matched;
        // uses readtxt() and merge()

        return matched;
    }

    void printAllPIs()
    {
    }

    void PITable()
    {
        vector<implicant> matched = matching();
        // uses matching to get the PI vector and minterms vector member and would use the printing fucntions later as final outputs

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
};

int main()
{
    QuineMclausky app;
    app.infile.open("TestFiles/Test2.txt");
    if (app.infile.is_open())
    {
        cout << "processing\n";
        app.readtxt();
        //app.printMembers();
    }
    else
    {
        perror("Error opening file");
    }
    return 0;
}
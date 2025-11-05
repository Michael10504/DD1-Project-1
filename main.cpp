#include <iostream>
using namespace std;
#include <vector>
#include <string>
#include <fstream>
#include <set>

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

    vector<implicant> readtxt()
    {
        // should assign the intvar and assign it to the number of minterms;
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
        vector<implicant> matched;
        // uses readtxt() and merge()

        
        return matched;
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

    void printAllPIs()
    {
    }

    void PITable()
    { 
        vector<implicant> matched = matching();
        // uses matching to get the PI vector and minterms vector member and would use the printing fucntions later as final outputs

    //should create a vector<implicant> done/finalisedPIs, 
    //to make printOutputExp() use it as the final out put and maybe printVerilogModule() also
    }



    void printOutputExp()
    {
    }

    void printVerliogModule()
    {
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
        cout << "processing";
    else
    {
        perror("Error opening file");
    }
    return 0;
}
#include <iostream>
using namespace std;
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <set>
#include <sstream>
#include <algorithm>
using namespace std;
#include <iomanip>
#include <cmath>

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
        marked = false;
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

    void readtxt()
    {
        string line;

        if (!infile.is_open())
        {
            cout << "Error: file is not open.\n";
            return;
        }

        if (getline(infile, line))
        {
            numvar = stoi(line);
        }
        else
        {
            numvar = 0; // no variables
            return;
        }

        if (getline(infile, line))
        {
            int curr = 0;
            int next = 1;

            while (next < line.size())
            {
                if (line[next] == ' ')
                {
                    // skips
                }
                if ((line[curr] == 'M' || line[curr] == 'm') && isdigit(line[next])) // m1 -> m 1
                {
                    line.insert(line.begin() + next, ' ');
                    next++;
                    curr++;
                }
                else if (isdigit(line[curr]) && line[next] == ',') // 1, -> 1 ,
                {
                    line.insert(line.begin() + next, ' ');
                    next++;
                    curr++;
                }
                else if (line[curr] == ',' && isalpha(line[next])) //,m -> , m
                {
                    line.insert(line.begin() + next, ' ');
                    next++;
                    curr++;
                }

                curr++;
                next++;
            }
        }

        stringstream ss(line);
        string token;

        if (line[0] == 'm')
        {
            while (ss >> token)
            {
                if (token[0] == ',')
                {
                    continue;
                }
                else if (isdigit(token[0]))
                {
                    minterms.push_back(stoi(token));
                }
            }
        }
        else if (line[0] == 'M')
        {
            vector<int> Maxterms;
            while (ss >> token)
            {
                if (token[0] == ',')
                {
                    continue;
                }
                else if (isdigit(token[0]))
                {
                    Maxterms.push_back(stoi(token));
                }
            }

            vector<int> temp(pow(2, numvar), 0);

            for (int a : Maxterms)
            {
                temp[a] = 1;
            }

            for (int i = 0; i < temp.size(); i++)
            {
                if (temp[i] == 0)
                {
                    minterms.push_back(i);
                }
            }
        }

        if (getline(infile, line))
        {
            int curr = 0;
            int next = 1;

            while (next < line.size())
            {
                if (line[next] == ' ')
                {
                    // skips
                }
                if (line[curr] == 'd' && isdigit(line[next])) // m1 -> m 1
                {
                    line.insert(line.begin() + next, ' ');
                    next++;
                    curr++;
                }
                else if (isdigit(line[curr]) && line[next] == ',') // 1, -> 1 ,
                {
                    line.insert(line.begin() + next, ' ');
                    next++;
                    curr++;
                }
                else if (line[curr] == ',' && isalpha(line[next])) //,m -> , m
                {
                    line.insert(line.begin() + next, ' ');
                    next++;
                    curr++;
                }

                curr++;
                next++;
            }
        }

        ss.clear();
        ss.str(line);
        if (line[0] == 'd')
        {
            while (ss >> token)
            {
                if (token[0] == ',')
                {
                    continue;
                }
                if (isdigit(token[0]))
                {
                    dontcares.push_back(stoi(token));
                }
            }
        }
        return;
    }

    bool
    isGreyCode(implicant a, implicant b)
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

    implicant merge(implicant a, implicant b)
    {
        string bits = a.binary;
        for (int i = 0; i < a.binary.length(); i++)
        {
            if (a.binary[i] != b.binary[i])
            {
                bits[i] = '-';
            }
        }
        return implicant(bits, a, b);
    }

    void sortC(vector<implicant> &C)
    {
        for (int i = 0; i < C.size() - 1; i++) // simple selection sort alg.
        {
            int minIndex = i;
            for (int j = i + 1; j < C.size(); j++)
            {
                int firstA = *(C[minIndex].mins.begin());
                int firstB = *(C[j].mins.begin());

                if (firstB < firstA)
                {
                    minIndex = j;
                }
            }
            if (minIndex != i)
            {
                swap(C[i], C[minIndex]);
            }
        }
        return;
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

    void matching()
    {

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

        // sorting
        sortC(columns[0]);
        // now columns[0] is sorted

        int k = 0;

        bool GreyCodeFound = false;
        while (!GreyCodeFound && k < columns.size())
        {
            GreyCodeFound = true;
            sortC(columns[k]);

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
                    bool isUnique = true; // checking for uniqueness
                    for (int k = 0; k < matched.size(); k++)
                    {
                        if (matched[k] == columns[i][j])
                        {
                            isUnique = false;
                            break;
                        }
                    }
                    if (isUnique)
                    {
                        matched.push_back(columns[i][j]);
                        // cout << i << " " << j << endl;
                    }
                }
            }
        }
    }

    void printAllPossiblePIs()
    {
        cout << "\n\nAll the possible Prime Implicants are:\n";

        for (int i = 0; i < matched.size(); i++)
        {

            cout << "PI" << setw(2) << i + 1 << ":" << " (";
            bool first = true;
            for (int a : matched[i].mins)
            {
                if (!first)
                    cout << ", ";
                cout << a;
                first = false;
            }

            cout << ")" << setw(2) << " --> " << matched[i].binary << endl;
        }

        cout << "\n---------------------------------------------------------------";
    }

    void PITable()
    {

        // uses matching to get the PI vector and minterms vector members modified
        // and would use the printing fucntions later as final outputs (cancelled)
        // should create a vector<implicant> done/finalisedPIs,

        matching(); // returns all prime implicants

        finalPIs.clear(); // to clear the final solution vector

        if (minterms.empty())
        {
            cout << "No minterms to cover." << endl;
            // will print nothing, should be handled in the print functions.
            printOutputExp();

            return;
        }

        if (matched.empty() && !minterms.empty())
        {
            cout << "Error: Minterms exist but there are no Prime Implicants" << endl;
            return;
        }

        // We now form the chart

        // convert minterm values into column indices
        map<int, int> minterm_to_column;
        for (int i = 0; i < minterms.size(); i++)
        {
            minterm_to_column[minterms[i]] = i;
        }

        // chart[row][col] : chart[PI_index][minterm_index]
        vector<vector<bool>> chart(matched.size(), vector<bool>(minterms.size(), false)); // we set all entries in the chart to false

        // Now fill the the chart
        for (int i = 0; i < matched.size(); i++)
        { // For each PI (row)
            for (int minterm_in_pi : matched[i].mins)
            { // Check if this minterm is one of our original minterms
                if (minterm_to_column.count(minterm_in_pi))
                {
                    int col_index = minterm_to_column[minterm_in_pi];
                    chart[i][col_index] = true; // As if we mark 'X' in the chart
                }
            }
        }
        // We now find essential prime implicants, columns that have only one 'X'

        vector<bool> minterm_covered(minterms.size(), false); // to check if the minterm is covered or not;
        vector<bool> pi_used(matched.size(), false);          // to check if the prime implicant was used before;

        bool found_essential;
        do
        {
            found_essential = false; // Assume none are found in this loop at the beginning

            // Iterate through each column (minterm)
            for (int j = 0; j < minterms.size(); j++)
            {
                if (minterm_covered[j])
                    continue;

                int cover_count = 0;
                int last_pi_index = -1;
                // Iterate through each ROW (PI) for this column
                for (int i = 0; i < matched.size(); i++)
                {
                    if (chart[i][j])
                    {
                        cover_count++;
                        last_pi_index = i;
                    }
                }
                // If count is 1, it's an essential PI
                if (cover_count == 1)
                {
                    // Check if we have already used this PI
                    if (pi_used[last_pi_index])
                        continue;

                    found_essential = true;
                    int pi_index = last_pi_index;

                    // we add this PI to our final solution
                    finalPIs.push_back(matched[pi_index]);
                    pi_used[pi_index] = true;

                    // mark all minterms covered by this PI as covered
                    for (int k = 0; k < minterms.size(); k++)
                    {
                        if (chart[pi_index][k])
                        {
                            minterm_covered[k] = true;
                        }
                    }
                }
            }
        } while (found_essential);

        // We now need to cover Remaining Minterms
        // Had to search the web for the algorithm, it's called Set Cover Approach using Greedy

        bool all_covered = false;
        while (!all_covered)
        {
            // Check if all minterms are covered
            all_covered = true;
            for (bool covered : minterm_covered)
            {
                if (!covered)
                {
                    all_covered = false;
                    break;
                }
            }
            if (all_covered)
                break; // exit the loop because all minterms are covered

            // Find the remaining PI that covers the most uncovered minterms
            int best_pi_index = -1;
            int max_new_covered = 0;
            for (int i = 0; i < matched.size(); i++)
            {
                if (pi_used[i])
                    continue; // skip the PI if it is already used

                int current_new_covered = 0;
                // Count how many new minterms the PI covers;
                for (int j = 0; j < minterms.size(); j++)
                {
                    if (!minterm_covered[j] && chart[i][j])
                    {
                        current_new_covered++;
                    }
                }
                if (current_new_covered > max_new_covered)
                {
                    max_new_covered = current_new_covered;
                    best_pi_index = i;
                }
            }
            if (best_pi_index == -1) // This shouldn't happen if matching() is correct, but will leave it for debugging
            {
                cout << "error: Could not cover all minterms." << endl;
                break;
            }

            // Add the best PI to our solution
            finalPIs.push_back(matched[best_pi_index]);
            pi_used[best_pi_index] = true;

            // Mark all minterms that the PI we added covers as 'covered'
            for (int k = 0; k < minterms.size(); k++)
            {
                if (chart[best_pi_index][k])
                {
                    minterm_covered[k] = true;
                }
            }
        }

        return;
    }

    void printAllFinalPIs()
    {
        cout << "\n\nThe solution Prime Implicants are:\n";

        for (int i = 0; i < finalPIs.size(); i++)
        {

            cout << "PI" << setw(2) << i + 1 << ":" << " (";
            bool first = true;
            for (int a : finalPIs[i].mins)
            {
                if (!first)
                    cout << ", ";
                cout << a;
                first = false;
            }

            cout << ")" << setw(2) << " --> " << finalPIs[i].binary << endl;
        }

        cout << "\n---------------------------------------------------------------";
    }

    void printOutputExp()
    {
        cout << "\n\nThe Final Expression:\n";
        cout << "f(";
        bool first = true;
        for (int i = 0; i < numvar; i++)
        {
            if (!first)
                cout << ", ";
            cout << char(i + 65);
            first = false;
        }
        cout << ") = ";

        first = true;
        for (implicant a : finalPIs)
        {
            if (!first)
                cout << " + ";
            // bool firstVar = true; // nested flag method
            for (int i = 0; i < a.binary.size(); i++)
            {
                if (a.binary[i] == '0' || a.binary[i] == '1') // only in these cases we should print a corresponding variable
                {
                    // if (!firstVar)
                    //     cout << '.';

                    if (a.binary[i] == '0')
                        cout << char(i + 65) << '\'';

                    if (a.binary[i] == '1')
                        cout << char(i + 65);

                    //                firstVar = false;
                }
            }
            first = false;
        }
        cout << endl;
        cout << "\n---------------------------------------------------------------";
        return;
    }

    void printVerilogModule(const string &moduleName = "Function",vector<string> Output={})
    {
        cout << "\n\nVerilog Module:\n\n";
        int var=Output[0].size();
        if (var == 0)
            cout << "could not print verilog module\n";

        cout << moduleName << "( input [" << var-1 << ":0] vars, output out)\n";
        for (int i = 0; i <var; i++){
            cout << "    wire not_var_" << i << ";" << endl;
            cout << "    not inv_" << i << " (not_var_" << i << ", vars[" << i << "]);" << endl;
        }
        cout << endl;
        for (int i = 0; i <Output.size(); i++) {
            cout << "wire term" << i << ";" << endl;
        }
        for (int i = 0; i <Output.size(); i++) {
            cout << "and and_out" << i << "(term" << i;
            for (int j=0;i<var;j++) {
                char curr=Output[i][j];
                if (curr==1) {
                  cout  <<",vars[" << j<<"]";
                }else if (curr==0) {
                    cout << ",not_var_" << j;
                }
            }
            cout << ");" <<endl;
        }
        cout << "or final (out";
        for (int i = 0; i <Output.size(); i++) {
            cout << ",term" << i;
        }
        cout << ");"<<endl;
        cout << "\nendmodule\n";
        cout << "\n---------------------------------------------------------------";
    }

    void printMembers() // for testing
    {
        cout << "\n\nGivens:\n";
        cout << "Number of Variables: " << numvar << endl;
        cout << "Minterms Involved: ";
        if (minterms.empty())
            cout << "---";
        else
            for (int i = 0; i < minterms.size(); i++)
            {
                cout << minterms[i] << " ";
            }
        cout << endl;

        cout << "Don't Cares Involved: ";
        if (dontcares.empty())
            cout << "---";
        else
            for (int i = 0; i < dontcares.size(); i++)
            {
                cout << dontcares[i] << " ";
            }
        cout << endl;
        cout << "\n---------------------------------------------------------------";
    }

    void runQM() // literally runs everthing/all the functions
    {
        cout << "\n---------------------------------------------------------------";
        readtxt();
        PITable();

        printMembers();
        printAllPossiblePIs();
        printAllFinalPIs();

        printOutputExp();

        return;
    }

private:
    int numvar;
    vector<int> minterms;
    vector<int> dontcares;

    vector<vector<implicant>> columns;
    vector<implicant> matched;
    vector<implicant> finalPIs; // to include the final set of Prime implicants that form the solution
};

int main()
{

    QuineMclausky app;
    app.infile.open("C:\\Users\\Mohammad Dawood\\Desktop\\Digital Design I\\Project 1\\DD1-Project-1\\TestFiles\\Test1.txt");

    if (app.infile.is_open())
    {
        cout << "---------------------------------------------------------------\n";
        cout << "File opened successfully. Processing...";
        app.runQM();
    }
    else
    {
        perror("Error opening file");
    }
    return 0;
}

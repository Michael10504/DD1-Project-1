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

        cout << "matching() is done!\n"; // for testing

        if (matched.empty())
            cout << "matched is empty!\n";
        else
            cout << "matched have things in it!\n";
        return;
    }

    void printAllFinalPIs()
    {
        cout << "\nAll the possible Prime Implicants are:\n";

        for (int i = 0; i < matched.size(); i++)
        {
            cout << "PI" << i + 1 << ": (";
            for (int a : matched[i].mins)
            {
                cout << a << " ";
            }
            cout << ") --> " << matched[i].binary << endl;
        }
    }

    void PITable()
    {
        matching(); // returns all prime implicants

        finalPIs.clear(); // to clear the final solution vector

        if (minterms.empty())
        {
            cout << "No minterms to cover." << endl;
            // will print nothing, should be handled in the print functions.
            printOutputExp();
            printVerilogModule();
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

        // uses matching to get the PI vector and minterms vector members modified and would use the printing fucntions later as final outputs

        // should create a vector<implicant> done/finalisedPIs,
        // to make printOutputExp() use it as the final out put and maybe printVerilogModule() also

        printOutputExp();
        printVerilogModule(); // should print both the output expressions and verilog module
    }

    void printOutputExp()
    {
    }

    void printVerilogModule()
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
    vector<implicant> finalPIs; // to include the final set of Prime implicants that form the solution
};

int main()
{
    QuineMclausky app;
    app.infile.open("C:\\Users\\Mohammad Dawood\\Desktop\\Digital Design I\\Project 1\\DD1-Project-1\\TestFiles\\Test4.txt");
    if (app.infile.is_open())
    {
        // cout << "processing\n";
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
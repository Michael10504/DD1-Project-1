#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <set>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

struct implicant
{
    set<int> mins;
    string binary;
    bool marked;
    int id; // will be used in the recursive algorithm search

    bool operator==(implicant b)
    {
        return (this->mins == b.mins && this->binary == b.binary);
    }

    bool operator<(const implicant &b) const
    {
        if (id != b.id)
            return id < b.id;
        if (binary != b.binary)
            return binary < b.binary;
        return mins < b.mins;
    }

    implicant()
    {
        mins = {0};
        binary = "";
        marked = false;
        id = -1; // Added default id
    }

    implicant(string b, int m)
    {
        binary = b;
        mins.insert(m);
        marked = false;
        id = -1;
    }

    implicant(string bits, implicant &t1, implicant &t2)
    {
        mins.insert(t1.mins.begin(), t1.mins.end());
        mins.insert(t2.mins.begin(), t2.mins.end());
        marked = false;
        binary = bits;
        id = -1; // Added default id
    }
};

class QuineMclausky
{
public:
    QuineMclausky() {}

    void readtxt()
    {
        // cout << "\nStarted Reading\n";
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
                if ((line[curr] == 'd' || line[curr] == 'D') && isdigit(line[next])) // d1 -> d 1
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
                else if (line[curr] == ',' && isalpha(line[next])) //,d -> , d
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

        for (int a : minterms)
        {
            if (a > pow(2, numvar) - 1)
            {
                terms_are_ok = false;
                return;
            }
        }

        for (int a : dontcares)
        {
            if (a > pow(2, numvar) - 1)
            {
                terms_are_ok = false;
                return;
            }
        }

        return;

        // cout << "\nEnded Reading\n";
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
            {
                binary += '1';
            }
            m = m / 2;
        }
        reverse(binary.begin(), binary.end());

        while (binary.length() < NumberOfVariables)
        {
            binary.insert(0, "0");
        }

        // Handle m=0 case
        if (NumberOfVariables > 0 && binary.empty())
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
        // simple selection sort alg.
        for (int i = 0; i < C.size() - 1; i++)
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
        // cout << "\nStarted Matching\n";
        //  uses readtxt() and merge()
        //  needs to create the implicants in the first column first.

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
                        {
                            columns.push_back(vector<implicant>());
                        }
                        columns[k + 1].push_back(merge(columns[k][i], columns[k][j]));
                        columns[k][i].marked = true;
                        columns[k][j].marked = true;
                        GreyCodeFound = false;
                    }
                }
            }
            k++;
        }

        for (int i = columns.size() - 1; i >= 0; i--)
        {
            for (int j = 0; j < columns[i].size(); j++)
            {
                if (columns[i][j].marked == false)
                {
                    bool isUnique = true; // checking for uniqueness
                    for (int l = 0; l < matched.size(); l++)
                    {
                        if (matched[l] == columns[i][j])
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

        for (int i = 0; i < matched.size(); ++i)
        {
            matched[i].id = i;
        } // we assign ids to the minterms
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
        // cout << "\nEnded Matching\n";
        cout << "\n---------------------------------------------------------------";
    }

    void PITable()
    {
        // cout << "\nStarted PITable\n";
        //  uses matching to get the PI vector and minterms vector members modified
        //  and would use the printing fucntions later as final outputs (cancelled)
        //  should create a vector<implicant> done/finalisedPIs,
        matching(); // returns all prime implicants

        finalEPIs.clear(); // to clear the Essential Prime Implicants vector
        finalSol.clear();

        if (minterms.empty())
        {
            cout << "No minterms to cover." << endl;
            // will print nothing, should be handled in the print functions.
            // printOutputExp();
            cout << "Error: No minterms to find their prime implicants." << endl;
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
                    finalEPIs.push_back(matched[pi_index]);
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
        // We will use Recursive search algorithm to find all possible combinations of minterms, had to search the web for the code.

        // Find remaining minterms and PIs.
        set<int> remaining_minterms;
        for (int i = 0; i < minterms.size(); i++)
        {
            if (!minterm_covered[i])
            {
                remaining_minterms.insert(minterms[i]);
            }
        }

        if (remaining_minterms.empty())
        {
            // This means that essential prime implicants covered everything
            finalSol.push_back(finalEPIs);
            return;
        }

        vector<int> remaining_pi_indices;
        for (int i = 0; i < matched.size(); i++)
        {
            if (!pi_used[i])
            {
                remaining_pi_indices.push_back(i);
            }
        }

        // we map every PI index to set of remaining minterms it covers
        // we also map remaining minterms to set of PI indices that cover it.
        map<int, set<int>> pi_to_minterms;
        map<int, set<int>> minterm_to_pis;

        for (int pi_index : remaining_pi_indices)
        {
            for (int minterm : matched[pi_index].mins)
            {
                if (remaining_minterms.count(minterm))
                {
                    pi_to_minterms[pi_index].insert(minterm);
                    minterm_to_pis[minterm].insert(pi_index);
                }
            }
        }

        // Remove PIs that don't cover any remaining minterms
        vector<int> final_remaining_pis;
        for (int pi_index : remaining_pi_indices)
        {
            if (!pi_to_minterms[pi_index].empty())
            {
                final_remaining_pis.push_back(pi_index);
            }
        }

        // We now solve the reduced chart
        vector<vector<int>> sub_solutions_indices = solveRecursive(pi_to_minterms, minterm_to_pis);

        // Combine EPIs with each sub-solution (Prime implicants)
        for (const auto &index_set : sub_solutions_indices)
        {
            vector<implicant> full_solution = finalEPIs;
            for (int pi_index : index_set)
            {
                full_solution.push_back(matched[pi_index]);
            }
            finalSol.push_back(full_solution);
        }

        if (sub_solutions_indices.empty() && !remaining_minterms.empty())
        {
            cout << "Error: Could not find any solution for remaining minterms.";
        }

        // cout << "\nEnded PITable\n";
        return;
    }

    vector<vector<int>> solveRecursive(map<int, set<int>> pi_to_minterms, map<int, set<int>> minterm_to_pis)
    {
        // Apply dominance rules repeatedly until the chart stabilizes
        while (applyDominance(pi_to_minterms, minterm_to_pis))
            ;

        // Base Case 1: All minterms are covered
        if (minterm_to_pis.empty())
        {
            return {{}};
        }

        // Base Case 2: Minterms remain but no PIs to cover them
        if (pi_to_minterms.empty())
        {
            return {};
        }

        int best_minterm = -1;
        int min_pis = 1000000;

        for (const auto &pair : minterm_to_pis)
        {
            if (pair.second.size() < min_pis)
            {
                min_pis = pair.second.size();
                best_minterm = pair.first;
            }
        }

        set<int> covering_pis = minterm_to_pis[best_minterm];
        vector<vector<int>> all_solutions;

        for (int pi_index : covering_pis)
        {
            // Create copies of the charts to modify for the recursive call
            map<int, set<int>> next_pi_map = pi_to_minterms;
            map<int, set<int>> next_minterm_map = minterm_to_pis;

            // Get all minterms covered by this chosen PI
            set<int> minterms_covered_by_pi = next_pi_map[pi_index];

            // Remove this PI and all minterms it covers
            next_pi_map.erase(pi_index);
            for (int m : minterms_covered_by_pi)
            {
                next_minterm_map.erase(m);
            }

            // Remove this PI from all other minterm-to-PI lists
            for (auto &pair : next_minterm_map)
            {
                pair.second.erase(pi_index);
            }

            // Recurse
            vector<vector<int>> sub_solutions = solveRecursive(next_pi_map, next_minterm_map);

            // Add our chosen PI to all returned solutions
            for (auto &sol : sub_solutions)
            {
                sol.push_back(pi_index);
                all_solutions.push_back(sol);
            }
        }
        return findMinimalSolutions(all_solutions);
    }

    bool applyDominance(map<int, set<int>> &pi_to_minterms, map<int, set<int>> &minterm_to_pis)
    {
        bool changed = false;

        // We first apply column dominance
        // If minterm x is covered by a superset of PIs that cover minterm y, then x is dominant. We only need to cover y and x will automatically be covered.
        // So, we can remove column x from the chart.
        vector<int> minterms_to_remove;
        vector<int> minterms_list;

        for (auto const &[minterm, pis] : minterm_to_pis)
            minterms_list.push_back(minterm);

        for (int i = 0; i < minterms_list.size(); ++i)
        {
            for (int j = 0; j < minterms_list.size(); ++j)
            {
                if (i == j)
                    continue;

                int m_x = minterms_list[i];
                int m_y = minterms_list[j];

                if (minterm_to_pis.find(m_x) == minterm_to_pis.end() || minterm_to_pis.find(m_y) == minterm_to_pis.end())
                    continue;

                set<int> pis_x = minterm_to_pis[m_x];
                set<int> pis_y = minterm_to_pis[m_y];

                // Check if pis_x is a superset of pis_y
                if (std::includes(pis_x.begin(), pis_x.end(), pis_y.begin(), pis_y.end()))
                {
                    // x is dominant over y. Remove x.
                    if (find(minterms_to_remove.begin(), minterms_to_remove.end(), m_x) == minterms_to_remove.end())
                    {
                        minterms_to_remove.push_back(m_x);
                        changed = true;
                    }
                }
            }
        }

        for (int m : minterms_to_remove)
        {
            minterm_to_pis.erase(m);
            // Also remove this minterm from the pi_to_minterms map
            for (auto &pair : pi_to_minterms)
            {
                pair.second.erase(m);
            }
        }

        if (changed)
            return true; // Re-run after one change

        // we now apply row dominance
        // If PI x covers a superset of minterms that PI y covers, the x is dominant and always a better or equal choice of y
        // So, we can remove row y from the chart.
        vector<int> pis_to_remove;
        vector<int> pis_list;

        for (auto const &[pi, minterms] : pi_to_minterms)
            pis_list.push_back(pi);

        for (int i = 0; i < pis_list.size(); ++i)
        {
            for (int j = 0; j < pis_list.size(); ++j)
            {
                if (i == j)
                    continue;

                int pi_x = pis_list[i];
                int pi_y = pis_list[j];

                if (pi_to_minterms.find(pi_x) == pi_to_minterms.end() || pi_to_minterms.find(pi_y) == pi_to_minterms.end())
                    continue;

                set<int> mins_x = pi_to_minterms[pi_x];
                set<int> mins_y = pi_to_minterms[pi_y];

                // Check if mins_x is a superset of mins_b
                if (std::includes(mins_x.begin(), mins_x.end(), mins_y.begin(), mins_y.end()))
                {
                    // x is dominant over y. Remove y.
                    if (find(pis_to_remove.begin(), pis_to_remove.end(), pi_y) == pis_to_remove.end())
                    {
                        pis_to_remove.push_back(pi_y);
                        changed = true;
                    }
                }
            }
        }

        for (int pi : pis_to_remove)
        {
            pi_to_minterms.erase(pi);
            // Also remove this PI from the minterm_to_pis map
            for (auto &pair : minterm_to_pis)
            {
                pair.second.erase(pi);
            }
        }

        return changed;
    }

    // Return solutions with minimum size
    vector<vector<int>> findMinimalSolutions(vector<vector<int>> &all_solutions)
    {
        if (all_solutions.empty())
            return {};

        int min_size = 1000000;
        for (const auto &sol : all_solutions)
        {
            if (sol.size() < min_size)
            {
                min_size = sol.size();
            }
        }

        vector<vector<int>> minimal_solutions;
        for (const auto &sol : all_solutions)
        {
            if (sol.size() == min_size)
            {
                // Check for uniqueness before adding
                bool duplicate = false;
                for (const auto &min_sol : minimal_solutions)
                {
                    if (set<int>(sol.begin(), sol.end()) == set<int>(min_sol.begin(), min_sol.end()))
                    {
                        duplicate = true;
                        break;
                    }
                }
                if (!duplicate)
                {
                    minimal_solutions.push_back(sol);
                }
            }
        }
        return minimal_solutions;
    }

    void printAllFinalPIs()
    {

        for (int i = 0; i < finalSol.size(); i++) // Iterate over all solutions
        {
            if (finalSol.size() > 1)
            {
                cout << "Solution " << i + 1 << ":\n";
            }

            for (int j = 0; j < finalSol[i].size(); j++) // Iterate over PIs in that solution
            {
                cout << "  PI (";
                bool first = true;
                for (int a : finalSol[i][j].mins)
                {
                    if (!first)
                        cout << ", ";
                    cout << a;
                    first = false;
                }
                cout << ")" << " --> " << finalSol[i][j].binary << endl;
            }
        }
        cout << "\n---------------------------------------------------------------";
    }

    void printOutputExp(int itsIndex, vector<implicant> aSolution)
    {

        if (finalSol.empty() && minterms.empty())
        {
            cout << "f = 0" << endl; // Handle no minterms case
            cout << "\n---------------------------------------------------------------";
            return;
        }

        if (finalSol.empty() && !minterms.empty())
        {
            cout << "No solution found." << endl;
            cout << "\n---------------------------------------------------------------";
            return;
        }

        // for (int i = 0; i < finalSol.size(); i++)
        // {
        cout << "f" << itsIndex + 1 << "(";
        bool first = true;
        for (int j = 0; j < numvar; j++)
        {
            if (!first)
                cout << ", ";
            cout << char(j + 65);
            first = false;
        }
        cout << ") = ";

        first = true;
        for (implicant a : aSolution)
        {
            if (!first)
                cout << " + ";

            bool term_has_vars = false;
            for (int j = 0; j < a.binary.size(); j++)
            {
                if (a.binary[j] == '0' || a.binary[j] == '1')
                {
                    term_has_vars = true;
                    if (a.binary[j] == '0')
                        cout << char(j + 65) << '\'';
                    if (a.binary[j] == '1')
                        cout << char(j + 65);
                }
            }
            if (!term_has_vars) // Handle case like "---" which is '1'
            {
                cout << "1";
            }
            first = false;
        }
        cout << endl;
        //}

        return;
    }

    void printVerilogModule(const string &moduleName = "Function", vector<implicant> aSolution = {})
    {

        if (finalSol.empty())
        {
            cout << "No solutions to generate a Verilog module for." << endl;

            return;
        }

        // Use the first solution
        // vector<implicant> firstSolution = finalSol[0];
        vector<string> binaryOutputs;
        for (const auto &imp : aSolution)
        {
            binaryOutputs.push_back(imp.binary);
        }

        if (binaryOutputs.empty())
        {
            // Handle f=0 case
            cout << "module " << moduleName << "( output out );\n";
            cout << "  assign out = 1'b0;\n";
            cout << "endmodule\n";

            return;
        }

        // Check for f=1 case
        bool f_is_one = false;
        for (const string &term : binaryOutputs)
        {
            bool all_dashes = true;
            for (char c : term)
            {
                if (c != '-')
                {
                    all_dashes = false;
                    break;
                }
            }
            if (all_dashes)
            {
                f_is_one = true;
                break;
            }
        }

        if (f_is_one)
        {
            cout << "module " << moduleName << "( output out );\n";
            cout << "  assign out = 1'b1;\n";
            cout << "endmodule\n";

            return;
        }

        int var = numvar;
        if (var == 0)
        {
            cout << "could not print verilog module (0 variables)\n";
            return;
        }

        cout << "module " << moduleName << "( input [" << var - 1 << ":0] vars, output out );\n";

        // Create inverters
        for (int i = 0; i < var; i++)
        {
            cout << "  wire not_var_" << i << ";" << endl;
            cout << "  not inv_" << i << " (not_var_" << i << ", vars[" << i << "]);" << endl;
        }
        cout << endl;

        // Create wires for each AND term
        for (int i = 0; i < binaryOutputs.size(); i++)
        {
            cout << "  wire term" << i << ";" << endl;
        }
        cout << endl;

        // Create AND gates
        for (int i = 0; i < binaryOutputs.size(); i++)
        {
            cout << "  and and_out" << i << "(term" << i;
            bool has_inputs = false;
            for (int j = 0; j < var; j++)
            {
                char curr = binaryOutputs[i][j];
                if (curr == '1')
                {
                    cout << ", vars[" << j << "]";
                    has_inputs = true;
                }
                else if (curr == '0')
                {
                    cout << ", not_var_" << j;
                    has_inputs = true;
                }
            }
            if (!has_inputs)
            {
                // This should not happen if f=1 is caught, but as a fallback.
                cout << ", 1'b1";
            }
            cout << ");" << endl;
        }
        cout << endl;

        // Create final OR gate
        cout << "  or final_or (out";
        for (int i = 0; i < binaryOutputs.size(); i++)
        {
            cout << ", term" << i;
        }
        cout << ");" << endl;

        cout << "\nendmodule\n";
    }

    void printMembers()
    {
        cout << "\n\nGivens:\n";
        cout << "Number of Variables: " << numvar << endl;
        cout << "Variables: ";
        for (int i = 0; i < numvar; i++)
        {
            char var = 'A' + i;
            cout << var;
            if (i != numvar - 1) // not the last
                cout << ", ";
        }
        cout << endl;

        if (!Maxterms.empty())
        {
            cout << "Maxterms Involved: ";
            for (int i = 0; i < Maxterms.size(); i++)
            {
                cout << Maxterms[i] << " ";
                if (i != Maxterms.size() - 1) // not the last
                    cout << ", ";
            }

            cout << endl;
        }

        cout << "Minterms Involved: ";
        if (minterms.empty())
        {
            cout << "---";
        }
        else
        {
            for (int i = 0; i < minterms.size(); i++)
            {
                cout << minterms[i] << " ";
                if (i != minterms.size() - 1) // not the last
                    cout << ", ";
            }
        }
        cout << endl;

        cout << "Don't Cares Involved: ";
        if (dontcares.empty())
        {
            cout << "---";
        }
        else
        {
            for (int i = 0; i < dontcares.size(); i++)
            {
                cout << dontcares[i] << " ";
                cout << minterms[i] << " ";
                if (i != dontcares.size() - 1) // not the last
                    cout << ", ";
            }
        }
        cout << endl;
        cout << "\n---------------------------------------------------------------";
    }

    void runQM() // literally runs everthing/all the functions
    {
        // setting all memebrs before starting each time
        terms_are_ok = true;

        minterms.clear();
        Maxterms.clear();
        dontcares.clear();

        columns.clear();
        matched.clear();
        finalEPIs.clear();
        finalSol.clear();

        cout << "\n---------------------------------------------------------------";
        readtxt();
        if (terms_are_ok)
        {
            PITable(); // This now finds all solutions

            printMembers();
            printAllPossiblePIs();

            if (finalSol.size() > 1)
                cout << "\n\nAll Possible Solutions:\n";
            else
                cout << "\n\nThe Only Solution:\n";

            printAllFinalPIs(); // This will print all solutions

            if (finalSol.size() > 1)
                cout << "\n\nThe Final Expressions:\n";
            else
                cout << "\n\nThe Final Expression:\n";

            int i = 0;
            for (const auto a : finalSol)
            {
                printOutputExp(i, a); // This will print all expressions
                i++;
            }

            cout << "\n---------------------------------------------------------------";

            if (finalSol.size() > 1)
                cout << "\n\nVerilog Modules:\n";
            else
                cout << "\n\nVerilog Module:\n";

            i = 1;
            for (const auto a : finalSol)
            {
                if (finalSol.size() > 1)
                {
                    cout << "\nModule " + to_string(i) + ":\n";
                }
                printVerilogModule("Function" + to_string(i), a); // This will print verilog for the *first* solution
                i++;
            }
            cout << "\n---------------------------------------------------------------";
        }
        else
        {
            cout << "\nError: Minterms/Don't Cares exceed the possible boundaries.";
            cout << "\n---------------------------------------------------------------";
        }

        infile.close();
        return;
    }

    ifstream infile;

private:
    int numvar;
    bool terms_are_ok;

    vector<int> minterms;
    vector<int> Maxterms; // in case there were
    vector<int> dontcares;

    vector<vector<implicant>> columns;
    vector<implicant> matched;          // All PIs
    vector<implicant> finalEPIs;        // Just EPIs
    vector<vector<implicant>> finalSol; // List of all minimal solutions
};

int main()
{

    QuineMclausky app;
    string folder;
    vector<fs::path> txtFiles;
    fs::path p(folder);
    do
    {
        cout << "Enter folder path: ";
        getline(cin, folder);

        fs::path p(folder);

        if (!fs::exists(p) || !fs::is_directory(p))
        {
            cout << "\n---------------------------------------------------------------\n";
            cout << "Error: invalid folder. Try Agian.\n";
            cout << "---------------------------------------------------------------\n";
        }
        else
        {

            // Collect all txt files
            for (const auto &entry : fs::directory_iterator(p))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".txt")
                {
                    txtFiles.push_back(entry.path());
                }
            }

            if (txtFiles.empty())
            {
                cout << "\n---------------------------------------------------------------\n";
                cout << "No .txt files found in folder. Try adding some files!\n";
                cout << "---------------------------------------------------------------\n";
            }
        }
    } while ((!fs::exists(p) || !fs::is_directory(p)) && txtFiles.empty());

    int choice;
    cout << "---------------------------------------------------------------\n";
    cout << "Found " << txtFiles.size() << " text files!\n";
    do
    {
        choice = 0;
        cout << "\nChoices:\n";
        for (int i = 0; i < txtFiles.size(); ++i)
        {
            cout << i + 1 << ". " << txtFiles[i].filename().string() << "\n";
        }

        cout << endl
             << to_string(txtFiles.size() + 1) + ". Exit\n";

        cout << "---------------------------------------------------------------\n";
        cout << "Choose: ";
        cin >> choice;

        if (choice < 1 || choice > (txtFiles.size() + 1))
        {
            cout << "\n---------------------------------------------------------------\n";
            cout << "Invalid selection. Try Agian!\n";
            cout << "-----------------------------------------------------------------\n\n";
        }
        else
        {
            if (choice > 0 || choice < (txtFiles.size()))
            {
                fs::path chosen = txtFiles[choice - 1];
                cout << "You selected: " << chosen << "\n";
                app.infile.open(chosen);

                if (app.infile.is_open())
                {
                    cout << "---------------------------------------------------------------\n";
                    cout << "File opened successfully. Processing...";
                    app.runQM();

                    app.infile.close();
                }
                else
                {
                    perror("Error opening file");
                }
            }
        }
    } while (choice != (txtFiles.size() + 1));

    return 0;
}

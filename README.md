==================================
  Quine-McCluskey Logic Minimizer
==================================

This is a C++ console application that automates the process of Boolean logic minimization using the Quine-McCluskey (QM) algorithm. It takes a text file specifying minterms (or maxterms) and don't-cares, and outputs all possible minimal Sum-of-Products (SOP) expressions and their corresponding Verilog modules.

The entire program is implemented in a single .cpp file and driven by an interactive terminal menu.

🚀 FEATURES
----------
- Tabulation Method: Finds all Prime Implicants (PIs) from the given terms.
- Covering Algorithm: Identifies all Essential Prime Implicants (EPIs) and uses a recursive search to find all possible minimal solutions.
- Comprehensive Output:
    - 🧾 Displays all found Prime Implicants.
    - 💡 Lists all unique minimal solutions (combinations of PIs).
    - ✍️ Prints the final, human-readable Boolean expressions for each solution.
    - 🤖 Generates a complete Verilog module for each solution.
- Flexible Input:
    - Accepts both minterms (e.g., m1, m3) and maxterms (e.g., M1, M3).
    - Correctly processes don't-care terms.
- Interactive Menu: Allows the user to select input files from a directory and run the algorithm repeatedly without restarting.


🔧 HOW TO BUILD AND RUN
--------------------

Prerequisites:
- A C++ compiler (e.g., g++, Clang, MSVC).

Compilation:
Since the program is a single .cpp file (e.g., main.cpp), you can compile it directly from your terminal:

    # Using g++
    g++ -o qm_solver main.cpp -std=c++17

    # Using Clang
    clang++ -o qm_solver main.cpp -std=c++17

Execution:
1. Run the compiled executable:
    ./qm_solver

2. The program will first ask you to provide the *absolute path* to the folder containing your .txt input files.
3. A menu will appear listing all .txt files found in that directory.
4. Select a file by entering its corresponding number.
5. The program will process the file and display the full analysis.
6. Once the output is complete, the menu will reappear, allowing you to select another file or quit.

Note on File Handling: The list of available files is generated when the program starts. You cannot add or remove test files while the program is running (you must restart it). However, you *can* edit the *contents* of an existing file, and the program will read the updated data the next time you select it from the menu.


📁 INPUT FILE FORMAT
-----------------

The program expects .txt files formatted as follows:

- Line 1: The number of Boolean variables.
- Line 2: The minterms (prefixed with 'm') or maxterms (prefixed with 'M'), separated by commas.
- Line 3: The don't-care terms (prefixed with 'd'), separated by commas.

Example File:
This example specifies a function of 3 variables, with minterms (1, 3, 6, 7) and don't-cares (0, 5).

    3
    m1,m3,m6,m7
    d0,d5


⚙️ PROGRAM DESIGN AND DATA STRUCTURES
----------------------------------

The application is built around a central `QM` class and is divided into two fundamental phases.

Algorithms:
1. Prime Implicant Generation: The `matching()` function implements the tabular method. It iteratively compares and merges terms to find all Prime Implicants (PIs) of the function.
2. Covering Algorithm: The `PITable()` and `solveRecursive()` functions implement the second phase. They build a prime implicant chart, find all Essential Prime Implicants (EPIs), and then use a recursive backtracking search to find all minimal combinations of PIs to cover the remaining minterms.

Core Data Structures:

  struct implicant
  This struct represents a single term (or implicant) during the tabulation process.
  - set<int> mins: Stores the set of original minterm numbers that this implicant covers.
  - string binary: The binary representation of the implicant (e.g., 0-10).
  - bool marked: A flag used during tabulation to mark if an implicant was combined.
  - int id: A unique ID used to manage the PI in the recursive solving phase.

  class QM
  This class encapsulates the entire algorithm and its data.
  - int numvar: Stores the number of Boolean variables.
  - vector<int> minterms: A vector holding the input minterms to be covered.
  - vector<int> Maxterms: A vector for input maxterms (if provided).
  - vector<int> dontcares: A vector for the "don't care" input terms.
  - vector<vector<implicant>> columns: A 2D vector representing the implicant grouping columns.
  - vector<implicant> matched: Stores all Prime Implicants (PIs) after tabulation.
  - vector<implicant> finalEPIs: Stores the identified Essential Prime Implicants (EPIs).
  - vector<vector<implicant>> finalSol: A 2D vector storing all possible minimal solutions. Each inner vector represents one complete solution.
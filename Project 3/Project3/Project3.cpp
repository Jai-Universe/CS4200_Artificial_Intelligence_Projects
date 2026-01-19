// CS 4200 Project 3
// Using tic-tac-toe code provided as base skeleton

#include <iostream>
#include <string>
#include <chrono>
using Clock = std::chrono::steady_clock; //setting up Clock

Clock::time_point searchStart; // setting searchStart as the variable capturing a specific instant in time
const int TIME_LIMIT = 5000; // constant used to set how long the computer has to make a move

using namespace std;

// Function prototypes
void makemove();
int minSolver(int depth, int alpha, int beta);
int maxSolver(int depth, int alpha, int beta);
int evaluate();
bool setup();
int check4winner();
bool timeUp();
void checkGameOver();
void getamove();
void printboard();

// Initializing variables
char b[8][8];
int maxdepth = 5;
bool playerFirst;

int main()
{
    bool first = setup();
    printboard();
    for (;;) //infinite loop
    {
        if (first) { // player goes first
            getamove();
            checkGameOver();
            makemove();
            checkGameOver();
        }
        else { // computer goes first
            makemove();
            checkGameOver();
            getamove();
            checkGameOver();
        }
    }
}

void printboard()
{
    cout << endl;
    cout << "  1 2 3 4 5 6 7 8" << endl; // Column headers
    string LETTERS = "ABCDEFGH"; 
    // Loop to print row headers and board
    for (int i = 0; i < 8; i++) {
        cout << LETTERS[i] << " ";
        for (int j = 0; j < 8; j++) {
            cout << char(b[i][j]) << " ";
        }
        cout << endl;
    }
}

// Create the board and get the player order
bool setup() {
    for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) { b[i][j] = '-'; }

    cout << "Enter 1 if you want to go first and enter 2 if you want the computer to go first: ";
    int playerOrder = -1;
    // Loop that keeps asking for proper input
    while (playerOrder != 0) {
        cin >> playerOrder;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // remove the newline character from the buffer
        if (playerOrder == 1) {
            playerFirst = true;
            playerOrder = 0;
        }
        else if (playerOrder == 2) {
            playerFirst = false;
            playerOrder = 0;
        }
        else {
            cout << "Invalid input, enter 1 if you want to go first and enter 2 if you want the computer to go first: ";
            playerOrder = -1;
        }
    }
    return playerFirst;
}

void getamove()
{
    string input;
    string letterSearch = "abcdefgh";
    bool validMove = false;
    cout << "Enter your move by entering the square you want to occupy in chess form (e.g., b5):  ";
    // Loop to ensure proper move input
    while (!validMove) {
        getline(cin, input);
        if (input.length() != 2) {
            cout << "Please enter exactly 2 characters (e.g., c2)." << endl;
            continue;
        }


        char rowChar = tolower(input[0]);  // Converting to lowercase in case user entered capital letter
        char colChar = input[1];
        int colIndex = colChar - '0' - 1;

        size_t rowIndex = letterSearch.find(rowChar);
        if (rowIndex == string::npos) { // find search return no position meaning it is not in the list of letters 
            cout << "Please enter a valid square to occupy. That is not a valid row.  ";
            continue;
        }
        if (colIndex < 0 || colIndex > 7) {
            cout << "Please enter a valid square to occupy, that column does not exist.  ";
            continue;
        }
        if (b[rowIndex][colIndex] != '-') {
            cout << "That square is already occupied, please choose an empty square." << endl;
            continue;
        }
        
        b[rowIndex][colIndex] = 'O'; // Move accepted, that cell is marked as occupied by the player
        validMove = true;
    }
}

// Evaluates how good non-terminal states are by using a linear weighted function based on how many X's or O's are in a row
// Positive outputs for good states for X, negative outputs for good states for O
int evaluate() 
{
    int score = 0;

    // Horizontal  checking
    for (int r = 0; r < 8; r++) { 
        for (int c = 0; c < 5; c++) { // Limit the column to a range of 5 so it doesn't try to check outside the board

            int xCount = 0;
            int oCount = 0;

            for (int window = 0; window < 4; window++) {
                // Horizontal window

                if (b[r][c+window] == 'X') {
                    xCount += 1;
                }
                else if (b[r][c+window] == 'O') {
                    oCount += 1;
                }
            }

            // Blocked window
            if (xCount > 0 && oCount > 0)
                continue; // break out of checking across these four columns because it's blocked by the opposing player

            if (xCount == 3) score += 500;
            if (xCount == 2) score += 100;
            if (xCount == 1) score += 50;

            if (oCount == 3) score -= 500;
            if (oCount == 2) score -= 100;
            if (oCount == 1) score -= 100;
        }
    }

    // Vertical checking
    for (int c = 0; c < 8; c++) { 
        for (int r = 0; r < 5; r++) { // Limit the  row to a range of 5 so it doesn't try to check outside the board

            int xCount = 0;
            int oCount = 0;

            for (int window = 0; window < 4; window++) { 
                // Vertical window
                if (b[r + window][c] == 'X') {
                    xCount += 1;
                }
                else if (b[r + window][c] == 'O') {
                    oCount += 1;
                }
            }

            // Blocked window because there is the opposite marker present
            if (xCount > 0 && oCount > 0)
                continue; // break out of checking across these four rows because it's blocked by the opposing player

            // Weights
            if (xCount == 3) score += 500;
            if (xCount == 2) score += 100;
            if (xCount == 1) score += 50;

            if (oCount == 3) score -= 500;
            if (oCount == 2) score -= 100;
            if (oCount == 1) score -= 50;
        }
    }
    return score;
}

void makemove()
{
    int best = -20000, depth = maxdepth, score;
    int mi = -1, mj = -1;
    searchStart = Clock::now(); // resets the start of the timer at the start of turn
    bool outOfTime = false;

    for (int i = 0; i < 8 && !outOfTime; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (b[i][j] == '-') {
   
                if (timeUp()) {
                    outOfTime = true;
                    break;
                }
                b[i][j] = 'X'; // make move on board
                score = minSolver(depth - 1, -20000, 20000); //start alpha very low so any real value is chosen as the actual alpha, vice versa for beta
                if (score > best) { 
                    mi = i; 
                    mj = j; 
                    best = score; 
                }
                b[i][j] = '-'; // undo move
            }
        }
    }
    string letterSearch = "abcdefgh";
    char rowChar = letterSearch[mi]; // matching the index to the proper row letter
    int colIndex = mj + 1; // +1 since the board doesn't start at 0

    cout << "My move is " << rowChar << colIndex << endl;
    b[mi][mj] = 'X'; // Making move on board
}

// MIN function minimizes score which is in favor of the player
int minSolver(int depth, int alpha, int beta) { //alpha is the best case value for MAX, beta is the best case value for MIN
    int best = 20000, score;
    if (check4winner() != 0) return (check4winner());
    if (depth == 0) return (evaluate());

    if (timeUp()) return evaluate(); // checking that it's still within time

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++)
        {
            if (b[i][j] == '-')
            {
                b[i][j] = 'O'; // make move on board
                score = maxSolver(depth - 1, alpha, beta);
                
                b[i][j] = '-'; // undo move

                best = min(best, score); // Best for MIN is the lowest score
                beta = min(beta, best); // Beta becomes whatever that's been seen that is the lowest

                if (alpha >= beta) { // this means that the MIN function had a better option prior
                    return best; // stop looking, prune
                }
            }
        }
    }
    return(best);
}

// MAX function which maximizes score which is better for the computer
int maxSolver(int depth, int alpha, int beta) { //alpha is the best case value for MAX, beta is the best case value for MIN
    int best = -20000, score; // Start with a very low score so that any actual move score improves it
    if (check4winner() != 0) return (check4winner()); // make sure the board isn't in a winning state already
    if (depth == 0) return (evaluate()); // return evaluation of non-terminal state since it's not a winning board
    
    if (timeUp()) return evaluate(); // checking that it's still within time

    // Going down future board states 
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (b[i][j] == '-')
            {
                b[i][j] = 'X'; // make move on board
                score = minSolver(depth - 1, alpha, beta);
                best = max(best, score);
                alpha = max(alpha, best);
                b[i][j] = '-'; // undo move
                
                if (alpha >= beta) { // this means that the MIN function had a better option prior
                    return best; // stop looking, prune
                }
            }
        }
    }
    return(best);
}

// Function to keep track of the time
bool timeUp() {
    auto now = Clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - searchStart).count();
    return elapsed >= TIME_LIMIT;
}


int check4winner()
{
    // Row win check
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 5; c++) { // Limit the column to range of 5 so it doesn't try to check outside the board
            if (b[r][c] == 'X' &&
                b[r][c + 1] == 'X' &&
                b[r][c + 2] == 'X' &&
                b[r][c + 3] == 'X') {
                return 5000;  // computer wins
            }
            if (b[r][c] == 'O' &&
                b[r][c + 1] == 'O' &&
                b[r][c + 2] == 'O' &&
                b[r][c + 3] == 'O') {
                return -5000; // player wins
            }
        }
    }

    // Column win check
    for (int c = 0; c < 8; c++) {
        for (int r = 0; r < 5; r++) { // Limit the row range to 5 so it doesn't try to check outside the board
            if (b[r][c] == 'X' &&
                b[r + 1][c] == 'X' &&
                b[r + 2][c] == 'X' &&
                b[r + 3][c] == 'X') {
                return 5000;  // computer wins
            }
            if (b[r][c] == 'O' &&
                b[r + 1][c] == 'O' &&
                b[r + 2][c] == 'O' &&
                b[r + 3][c] == 'O') {
                return -5000; // player wins
            }
        }
    }

    for (int i = 0; i < 8; i++) {// easy check, if the board still has playable spaces, continue the game
        for (int j = 0; j < 8; j++)
        {
            if (b[i][j] == '-') return 0;
        }
    }
    return 0; // draw
}

void checkGameOver()
{
    printboard();
    if (check4winner() == -5000) { cout << "You win" << endl; exit(0); }
    if (check4winner() == 5000) { cout << "I win" << endl; exit(0); }
    if (check4winner() == 1) { cout << "It's a draw" << endl; exit(0); }
}

// CS 4200
// A* heuristic search
// Project1.cpp : Main file

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
using namespace std;

// Declarations
vector<int> availableNumbers = { 0,1,2,3,4,5,6,7,8 };
vector<int> goal = {0,1,2,3,4,5,6,7,8 };
vector<int> inputNumbers;
int nodesExpanded = 0;
int nodesTotal = 0;

// Structures

struct Node {
    vector<int> boardState; //Associated board state
    int gCost;  // path cost so far
    int hCost;  // heuristic cost
    Node* parent; // Parent node so solution path can be tracked
};

// Check solvability
bool isSolvable(vector<int>& puzzle) {
    int numInverses = 0;
    for (int i = 0; i < 9; i++) {
        if (puzzle[i] == 0) { // Skip the blank tile
            continue;
        }
        for (int j = i + 1; j < 9; j++)
            if (puzzle[j] != 0 && puzzle[j] < puzzle[i])
                numInverses ++;
    }
    return numInverses % 2 == 0;
}

// Create puzzle from user input
void createPuzzle() {
    inputNumbers.clear();
    vector<int> tempAvailable = availableNumbers;

    cout << "\nEnter 9 unique numbers from 0-8 (0 represents blank) to create your puzzle:\n";
    for (int i = 0; i < 9; i++) {
        int num;
        while (!tempAvailable.empty()) { //Loop for the user to enter numbers for the puzzle
            cout << "\nAvailable: ";
            for (int availableNumber : tempAvailable) {
                cout << availableNumber << " ";
            }
            cout << "\nEnter number " << (i + 1) << ": "; 
            cin >> num;
        
            // Places the location of the number being searched for in the iterator object if found
            auto it = find(tempAvailable.begin(), tempAvailable.end(), num);
            if (it != tempAvailable.end()) { // If the number the user is looking to add is actually in the available numbers list
                tempAvailable.erase(it); 
                inputNumbers.push_back(num); // Adds the number into the user's start puzzle state
            }
            else {
                cout << num << " is an invalid number, try again\n";
            }
        }
    }

    if (!isSolvable(inputNumbers)) {
        cout << "Puzzle not solvable! Try again.\n";
        createPuzzle(); // Sends the user back to enter numbers for the puzzle again
    }
}

// Heuristics, the h(n) part of f(n) = g(n) + h(n)
int calculateH1Cost(const vector<int>& board) {
    int h = 0;
    for (int i = 0; i < 9; i++) {
        if (board[i] != 0 && board[i] != goal[i]) { // Checking if the tile is not where it's supposed to be
            h++;
        }
    }
    return h;
}

int calculateH2Cost(const vector<int>& board) {
    int h = 0;

    for (int i = 0; i < 9; i++) { // For loop to calculate distance needed for each tile
        int tileValue = board[i];
        if (tileValue == 0) continue; // ignore blank tile

        // Find where the tile is supposed to be
        int goalIndex = find(goal.begin(), goal.end(), tileValue) - goal.begin(); //returns an iterator for the position and subtracting goal.begin() makes it an int

        int currentRow = i / 3;
        int currentCol = i % 3;
        int goalRow = goalIndex / 3;
        int goalCol = goalIndex % 3;

        h += abs(currentRow - goalRow) + abs(currentCol - goalCol);
    }

    return h;
}

// Hash function for vector<int> which just returns an int of all the numbers put together
int hashBoard(const vector<int>& board) {
    int h = 0;
    for (int x : board) h = h * 10 + x;
    return h;
}

// Check if goal state achieved
bool isGoal(const vector<int>& board) {
    return board == goal;
}

// Returns a vector of the states of the neighboring tiles
vector<vector<int>> getNeighboringTiles(vector<int>& board) {
    vector<vector<int>> neighbors;
    int blankTilePosition = find(board.begin(), board.end(), 0) - board.begin(); // index of blank

    // Can move left if its index is not divisible by 3
    if (blankTilePosition % 3 != 0) {
        vector<int> leftMove = board;
        swap(leftMove[blankTilePosition], leftMove[blankTilePosition - 1]);
        neighbors.push_back(leftMove);
    }

    // Can move right if its index % 3 is not 2
    if (blankTilePosition % 3 != 2) {
        vector<int> rightMove = board;
        swap(rightMove[blankTilePosition], rightMove[blankTilePosition + 1]);
        neighbors.push_back(rightMove);
    }

    // Can move up if its index is not 0, 1, 2
    if (blankTilePosition > 2) {
        vector<int> upMove = board;
        swap(upMove[blankTilePosition], upMove[blankTilePosition - 3]);
        neighbors.push_back(upMove);
    }

    // Can move down if its index is not 6, 7, 8
    if (blankTilePosition < 6) {
        vector<int> downMove = board;
        swap(downMove[blankTilePosition], downMove[blankTilePosition + 3]);
        neighbors.push_back(downMove);
    }

    return neighbors;
}


// Print solution path
void printSolutionPath(Node* goalNode) {
    vector<Node*> path;
    while (goalNode) { // While goalNode is a valid node and not nullptr
        path.push_back(goalNode); // Add the node to the solution path
        goalNode = goalNode->parent; // Step back by setting the node to be the parent node
    }
    reverse(path.begin(), path.end()); // Reverse the path so it is easier to print out in the correct order
    cout << "Solution path (" << path.size() - 1 << " moves):\n";

    for (int step = 0; step < path.size(); step++) {
        cout << "Step " << step << ":\n";
        Node* state = path[step];
        for (int i = 0; i < 9; i++) {
            cout << state->boardState[i] << " ";
            if (i % 3 == 2) cout << "\n"; // Adds a line break so it is printed in the 3x3 form
        }
        cout << "----------\n";
    }
    cout << "Nodes expanded: " << nodesExpanded;
    nodesTotal += nodesExpanded;
}

// A* Search
void solvePuzzle(vector<int>& startBoard, int hMethod) {

    // Priority queue to keep track of the frontier, using a pair so that the cost value associated with the Node state can be used for sorting
    priority_queue<pair<int, Node*>, vector<pair<int, Node*>>,greater<pair<int, Node*>>> frontier; 

    // Explored states that keeps track of the <hash, cost>
    unordered_map<int, int> explored;

    Node* start = new Node;            // Create a new Node object for the root Node
    start->boardState = startBoard;    
    start->gCost = 0;                  // At depth 0 the g cost is 0
    if (hMethod == 1) {
        start->hCost = calculateH1Cost(startBoard);  
    }
    else {
        start->hCost = calculateH2Cost(startBoard);  
    }

    start->parent = nullptr;           // Root node has no parent

    int startFCost = start->gCost + start->hCost;
    frontier.push({ startFCost, start }); // Push to frontier as a node to be expanded
    explored[hashBoard(startBoard)] = 0; // Cost of this original board state is 0

    while (!frontier.empty()) { // While there are still nodes to expand in the frontier
        Node* current = frontier.top().second; // Obtain the vector<int> state from the top of the frontier queue
        frontier.pop();

        nodesExpanded++;
        int currentHash = hashBoard(current->boardState);

        if (isGoal(current->boardState)) { // If the current state is the goal state, exit out the loop
            cout << "\nPuzzle solved!\n";
            printSolutionPath(current);
            return;
        }

        if (explored[currentHash] < current->gCost) { // If there already exists a board state that matches the board state currently being looked out with a lower cost, break out the loop (graph search)
            continue;
        }

        for (vector<int> neighborConfig : getNeighboringTiles(current->boardState)) {
            int gCost = current->gCost + 1;
            int hCost;
            if (hMethod == 1) {
                hCost = calculateH1Cost(neighborConfig);
            }
            else if (hMethod == 2) {
                hCost = calculateH2Cost(neighborConfig);
            }
            else {
                hCost = 0; 
            }

            int hash = hashBoard(neighborConfig);

            if (explored.count(hash) && explored[hash] <= gCost) { // If the neighbor's hash has already been explored and had a lower cost, break out the loop
                continue;
            }

            Node* nextNode = new Node;
            nextNode->boardState = neighborConfig;
            nextNode->gCost = gCost;
            nextNode->hCost = hCost;
            nextNode->parent = current;


            int fCost = gCost + hCost;
            frontier.push({ fCost, nextNode }); // It is a viable node and should be added to the frontier
            explored[hash] = gCost; // Keeping track of what nodes have been explored and the respective cost
        }
    }

    cout << "No solution found!\n";
    cout << "Total nodes expanded: " << nodesExpanded << "\n";
}

// Main
int main() {
    cout << "Press Enter to create your puzzle...";
    cin.ignore(); // Wait for enter

    
    createPuzzle();

    cout << "\nYour inputted puzzle:\n";
    for (int i = 0; i < 9; i++) {
        cout << inputNumbers[i] << " ";
        if (i % 3 == 2) cout << "\n";
    }

    cout << "Enter 1 for Heuristic 1 and 2 for Heuristic 2: ";
    int hInput;
    cin >> hInput;
    cout << "Solving with h" << hInput << "...\n";
    solvePuzzle(inputNumbers, hInput);
    

    /* Code I messed around with to make the table
    // Example puzzles from files provided
    vector<vector<int>> puzzles4 = {
        { 1,2,5,3,4,8,6,7,0},
        { 1,2, 5,3,0,4,6,7,8},
        { 1,4,2,6,3,5,0,7,8 },
        { 1,4,0,3,5,2,6,7,8},
        { 1,4,2,3,7,5,6,8,0},
        {0,3,2,4,1,5,6,7,8},
        {3,2,0,4,1,5,6,7,8},
        {3,1,0,4,5,2,6,7,8},
        {3,2,5,4,0,8,6,1,7},
        {1,2,3,4,0,6,7,8,5},
    };
    vector<vector<int>> puzzles8 = {
        { 1,2,5,4,0,8,3,6,7},
        {2,3,5,1,0,4,6,7,8 },
        { 1,4,2,6,0,3,7,8,5 },
        { 1,5,4,6,3,2,0,7,8},
        { 1,2,0,3,4,8,6,5,7},
        {4,7,2,1,0,5,3,6,8},
        {1,7,4,3,0,2,6,8,5},
        {0,4,2,1,3,5,6,7,8},
        {3,2,5,4,0,8,6,1,7},
        {4,3,1,5,0,2,6,7,8},
    };
    vector<vector<int>> puzzles12 = {
        { 0,2,5,1,6,8,4,3,7},
        { 0,3,5,2,7,4,1,6,8 },
        { 4,2,0,1,6,3,7,8,5 },
        { 1,5,4,6,0,3,7,8,2 },
        { 1,2,8,3,0,7,6,4,5},
        {4,7,2,1,5,8,0,3,6},
        {1,7,4,6,0,2,8,3,5},
        {1,4,2,6,3,5,7,8,0},
        {3,5,8,4,0,2,6,1,7},
        {5,4,1,6,3,2,0,7,8},
        {1,5,4,7,6,2,0,3,8}
    };
    vector<vector<int>> puzzles16 = {
        { 2,6,5,1,8,7,4,3,0 },
        { 3,7,5,2,4,8,1,6,0 },
        { 4,2,3,1,8,6,7,5,0 },
        { 1,5,4,6,0,8,7,2,3 },
        { 1,8,7,3,0,2,6,4,5},
        {4,7,2,5,8,6,1,3,0},
        {1,7,4,8,6,2,3,5,0},
        {1,3,4,6,0,2,7,8,5},
        {5,4,1,7,6,3,8,2,0},
        {3,5,8,6,0,2,1,4,7},
        {5,4,1,6,0,3,7,8,2},
    };
    vector<vector<int>> puzzles20 = {
        { 2,6,5,8,0,7,1,4,3 },
        { 3,7,5,1,2,8,0,4,6 },
        { 0,4,3,1,2,8,7,5,6 },
        { 4,7,2,8,0,6,5,1,3 },
        { 8,7,0,1,3,2,6,4,5},
        {0,1,4,8,7,6,3,5,2},
        {1,3,4,6,0,8,7,5,2},
        {5,8,0,3,6,2,1,4,7},
        {5,4,1,7,6,3,8,2,0},
        {2,8,3,1,6, 4,7,0,5},
        {8,6,7,2,5,4,3,0,1},
        {7,2,4,5,0,6,8,3,1}
    };

    for (int i= 0; i <10; i++) {
        solvePuzzle(puzzles4[i], 2);
        double NA = nodesTotal / 10.0;
        cout << "Nodes total average is " << NA;
    }
    */
}

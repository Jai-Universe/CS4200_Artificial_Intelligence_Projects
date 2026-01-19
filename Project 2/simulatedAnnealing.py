#CS 4200 Artifical Intelligence
import random
import numpy as np
import time
import math

# class
class Problem:
    def __init__(self):
        self.board = np.zeros((8, 8), dtype=int)  #creates a 8x8 array of 0's as integers
        self.generateProblem()
        self.startTime = time.perf_counter()

    def generateProblem(self):
        for i in range(8):
            self.board[random.randint(0, 7)][i] = 1 # randomly places a 1 in each col
            

# other functions

def countAttacking(state):
    queens = []
    attacks = 0

    for row in range(8):
        for col in range(8):
            if state[row][col] == 1:
                queens.append((row, col)) # array of location pairs

    for i in range(len(queens)):
        currentRow, currentCol = queens[i]
        for j in range(i + 1, len(queens)):
            neighborRow, neighborCol = queens[j]

            # checking for same column
            if currentCol == neighborCol:
                attacks += 1
            # checking for same row
            if currentRow == neighborRow:
                attacks += 1
            # checking for diagonal
            elif abs(currentRow - neighborRow) == abs(currentCol - neighborCol):
                attacks += 1

    return attacks

def getSuccessors(currentState):
    successors = []
    queens = [(r, c) for r in range(8) for c in range(8) if currentState[r][c] == 1] # searching for all queens

    for r, c in queens:
        for newRow in range(8):
            for newCol in range(8):
                if (newRow, newCol) != (r, c) and currentState[newRow][newCol] == 0:
                    newState = currentState.copy()
                    newState[r][c] = 0      # remove the queen from current spot
                    newState[newRow][newCol] = 1  # place queen in new spot
                    successors.append(newState) # this is a new succesor option so it is added to the list

    return successors

# value of temperature T as a function of time
def schedule(startTime, T0=1.0, alpha=0.1): # alpha is the cooling rate (how fast temperature decreases with time)
    elapsed = time.perf_counter() - startTime
    return (T0 - alpha * elapsed) 



def simulatedAnnealing():
    problem = Problem() # create Problem class which will create the puzzle and its own start time
    T = 1 # starting temperature
    currentState = problem.board

    print("Starting board:\n")
    print(currentState)
    currentH = countAttacking(currentState) # want to continue to find best successors until attacking = 0
    steps = 0

    while True:
        elapsed = time.perf_counter() - problem.startTime

        T = schedule(problem.startTime)
        if T <= 0:
            return currentState, steps, elapsed

        next_state = random.choice(getSuccessors(currentState))

        ΔE = countAttacking(next_state) - countAttacking(currentState)
        if currentH == 0:
            return currentState, steps, elapsed
        
        elif ΔE < 0 or random.random() < math.exp(-ΔE / T): # if it is a better case OR not that bad of a move depending on temperature
            currentState = next_state
            currentH = countAttacking(currentState)  # update after switching state

        steps += 1


# main code
print("Welcome to the 8-Queen Puzzle Solver")

RUNS = 100
solved = 0
totalSteps = 0
totalTime = 0

for i in range(RUNS): # running the program for given number of runs
    print("\nRun " + str(i) + "\n")
    final_state, steps, elapsedTime = simulatedAnnealing() 
    print("\nFinal board:\n")
    print(final_state)
    h = countAttacking(final_state)

    if h == 0:
        solved += 1

    totalSteps += steps
    totalTime += elapsedTime

print("\nTotal runs:", RUNS)
print("Solved:", solved)
print("Stuck in local minima:", RUNS - solved)
print("Success rate:", solved / RUNS)
print("Average steps:", totalSteps / RUNS)
print("Average run time for puzzle:", totalTime/RUNS)
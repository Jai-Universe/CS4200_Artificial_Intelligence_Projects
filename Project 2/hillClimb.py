#CS 4200 Artifical Intelligence
#no sideways moves so if the next best succesor is the same quality as the current one, stop
import random
import numpy as np
import time

class Problem:
    def __init__(self):
        self.board = np.zeros((8, 8), dtype=int)  #creates a 8x8 array of 0's as integers
        self.generateProblem()

    def generateProblem(self):
        for i in range(8):
            self.board[random.randint(0, 7)][i] = 1 # randomly places a 1 in each col
            

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

            # checking for same row
            if currentRow == neighborRow:
                attacks += 1
            # checking for diagonal
            elif abs(currentRow - neighborRow) == abs(currentCol - neighborCol):
                attacks += 1

    return attacks


def getSuccessors(currentState):
    successors = []

    for col in range(8):
        row = np.argmax(currentState[:, col]) # search all rows for the given column for the max value which is where the queen is (since the queen is at 1)

        for new_row in range(8):
            if new_row != row:
                newState = currentState.copy()
                newState[row][col] = 0 # remove the queen from the original state
                newState[new_row][col] = 1 # insert queen in new row
                successors.append(newState)

    return successors


def hillClimb():
    problem = Problem()
    currentState = problem.board
    print("Starting board:\n")
    print(currentState)
    currentH = countAttacking(currentState) # want to continue to find best successors until attacking = 0
    steps = 0

    while True:
        successors = getSuccessors(currentState)

        bestState = currentState # starting with assuming the best state is the current state
        bestH = currentH

        for s in successors:
            h = countAttacking(s)
            if h < bestH:
                bestState = s # set the new best state to the state with the lower h
                bestH = h

        if bestH >= currentH: # found the best state possible
            break

        currentState = bestState
        currentH = bestH
        steps += 1

    print("\nFinal state:\n")
    print(currentState)
    return bestH, steps


# main code
print('Welcome to the 8-Queen Puzzle Solver')
RUNS = 100
solved = 0
totalSteps = 0
totalTime = 0

startTime = time.perf_counter()
for i in range(RUNS):
    print("\nRun " + str(i) + "\n")
    finalH, steps = hillClimb()

    if finalH == 0:
        solved += 1     

    totalSteps += steps
    totalTime += time.perf_counter() - startTime

print("Total runs:", RUNS)
print("Solved:", solved)
print("Stuck in local minima:", RUNS - solved)
print("Success rate:", solved / RUNS)
print("Average steps:", totalSteps / RUNS)
print("Average run time for puzzle:", totalTime/RUNS)
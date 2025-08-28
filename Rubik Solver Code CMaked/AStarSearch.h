#pragma once
#include "PuzzleState.h"
#include <vector>

using namespace std;

class AStarSearch
{
public:
    vector<PuzzleState> open;
    vector<PuzzleState> closed;
    vector<PuzzleState> clopen; //temp variable

    bool AddSuccesor(PuzzleState& node);
    void Algorithm(PuzzleState& stateRC);

    // Orienting Edges
    bool GetSuccessors_EdgeOrient(PuzzleState nodeParent);
    void EdgeOrient(PuzzleState& stateRC);

    // F2L
    bool GetSuccessors_CornerOrient(PuzzleState nodeParent);
    void CornerOrient(PuzzleState& stateRC);

    bool GetSuccessors_EdgeUDPosition(PuzzleState nodeParent);
    void EdgeUDPosition(PuzzleState& stateRC);

    // Solve the rest of Rubik
    bool GetSuccessors(PuzzleState nodeParent);
    void SolveRest(PuzzleState &stateRC);

};

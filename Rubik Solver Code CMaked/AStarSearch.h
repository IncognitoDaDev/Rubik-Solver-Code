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
    void FirstPhase();
    void SecondPhase();
    void GetSuccessors_1stPhase(PuzzleState nodeParent);
    void GetSuccessors_2ndPhase(PuzzleState nodeParent);
    void Algorithm();
};

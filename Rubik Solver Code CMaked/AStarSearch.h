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
    bool GetSuccessors(PuzzleState nodeParent);
    void Algorithm();
    bool CA_GetSuccessors(PuzzleState nodeParent);
    int ColorAlgo();
};

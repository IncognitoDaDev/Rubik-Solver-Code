#pragma once
#include "PuzzleState.h"
#include <vector>

using namespace std;

class AStarSearch
{
public:
    void returnMovesInChar(vector<int> moves, char* a);
    vector<int> GetSuccessors_G1(vector<int> path, int state);
    vector<int> GetSuccessors_Solve(vector<int> path);
    void Algorithm(PuzzleState& nodeStart);
    PuzzleState SearchSolution(PuzzleState startNode);
    bool DFS_EdgeOrient(int G, vector<int>& paths, int threshold, int& nextThreshold, RubikCube RC);
    bool DFS_CornerOrient(int G, vector<int>& path, int threshold, int& nextThreshold, RubikCube RC);
    bool DFS_Solve(int G, vector<int>& paths, int threshold, int& nextThreshold, RubikCube RC);
};

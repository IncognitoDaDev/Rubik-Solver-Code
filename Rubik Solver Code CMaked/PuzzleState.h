#pragma once
#include "RubikCube.h"
#include <vector>
#include <string>

using namespace std;

class PuzzleState
{
public:
	float	f = 0, // sum of cumulative cost of predecessors and self and heuristic
			g = 0, // cost of this node + its predecessors
			h = 0; // heuristic estimate of distance to goal

	vector<PuzzleState> neighboorNodes;

	std::vector<int> moves;
	int previousMove = -1;

	RubikCube RC;

	int HashPiece(int typeOfPiece, int colors);
	int PieceID(RubikCube &nodeGoal, int typeOfPiece, int *coords);
	void HashRC(RubikCube &nodeGoal, int (&hash)[3][3][3]);
	void DehashRC(int(&hash)[3][3][3], int(&rc)[6][3][3]);
	void DesiredCoords(int hash[3][3][3], int pieceID, int& zyx);
	bool IsSameState(PuzzleState& rhs);
	std::string returnSolution();

	int GoalDistanceEstimate_G1(RubikCube node, int stage);
	int GoalDistanceEstimate_Solve(RubikCube node);

	bool IsGoal(RubikCube &RC);
	void SumCost(PuzzleState& node, int G);
};


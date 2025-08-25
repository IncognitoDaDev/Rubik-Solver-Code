#pragma once
#include "RubikCube.h"
#include <vector>
#include <string>

class PuzzleState
{
public:
	float	f = 0, // sum of cumulative cost of predecessors and self and heuristic
			g = 0, // cost of this node + its predecessors
			h = 0; // heuristic estimate of distance to goal

	std::vector<int> moves;
	int previousMove = -1;

	RubikCube RC;

	int HashPiece(int typeOfPiece, int colors, int orientations);
	int PieceID(RubikCube &nodeGoal, int typeOfPiece, int *coords);
	void HashRC(RubikCube &nodeGoal, int (&hash)[3][3][3]);
	void DehashRC(int(&hash)[3][3][3], int(&rc)[6][3][3]);
	void DesiredCoords(int hash[3][3][3], int pieceID, int& zyx);
	void DebugHash(int(&hash)[3][3][3]);
	int distCenter(int face, int pieceFace);
	bool IsSameState(PuzzleState& rhs);
	void copyMoves(PuzzleState& rhs);
	std::string returnSolution();

	void copyHash(int(&hash)[3][3][3], int rhs[3][3][3]);

	float GoalDistanceEstimate(PuzzleState& nodeGoal);
	bool IsGoal(PuzzleState& nodeGoal);
	void SumCost(PuzzleState& nodeGoal, PuzzleState nodeParent);
	bool CA_IsGoal(PuzzleState& nodeGoal);
	void CA_SumCost(PuzzleState& nodeGoal, PuzzleState nodeParent);
	float CA_GoalDistEst(PuzzleState& nodeGoal);
};


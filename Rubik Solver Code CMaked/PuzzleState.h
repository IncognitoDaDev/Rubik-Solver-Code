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

	RubikCube RC;

	int HashPiece(int typeOfPiece, int colors, int orientations);
	int PieceID(RubikCube &nodeGoal, int typeOfPiece, int *coords);
	void HashRC(RubikCube &nodeGoal, int (&hash)[3][3][3]);
	void DesiredCoords(int hash[3][3][3], int pieceID, int& zyx);
	void DebugHash(int(&hash)[3][3][3]);
	int distCenter(int face, int pieceFace);
	bool IsSameState(PuzzleState& rhs);
	void copyMoves(PuzzleState& rhs);
	std::string returnSolution();

	float GoalDistanceEstimate_1stPhase(PuzzleState& nodeGoal);
	bool IsGoal_1stPhase(PuzzleState& nodeGoal);
	void SumCost_1stPhase(PuzzleState& nodeGoal, PuzzleState nodeParent);

	float GoalDistanceEstimate_2ndPhase(PuzzleState& nodeGoal);
	bool IsGoal_2ndPhase(PuzzleState& nodeGoal);
	void SumCost_2ndPhase(PuzzleState& nodeGoal, PuzzleState nodeParent);
};


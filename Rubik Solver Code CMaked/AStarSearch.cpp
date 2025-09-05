#define _CRT_SECURE_NO_WARNINGS

#include "AStarSearch.h"
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

class Timer
{
private:
	// Type aliases to make accessing nested type easier
	using Clock = std::chrono::steady_clock;
	using Second = std::chrono::duration<double, std::ratio<1> >;

	std::chrono::time_point<Clock> m_beg{ Clock::now() };

public:

	void reset()
	{
		m_beg = Clock::now();
	}

	double elapsed() const
	{
		return std::chrono::duration_cast<Second>(Clock::now() - m_beg).count();
	}

	Timer() = default;
};

void AStarSearch::returnMovesInChar(vector<int> moves, char *a)
{
	strcpy(a, "");

	for(int i = 0; i < moves.size(); i++)
		switch (moves[i])
		{
		case 0: strcat(a, "U "); break;
		case 1: strcat(a, "D "); break;
		case 2: strcat(a, "F "); break;
		case 3: strcat(a, "B "); break;
		case 4: strcat(a, "R "); break;
		case 5: strcat(a, "L "); break;
		case 6: strcat(a, "U' "); break;
		case 7: strcat(a, "D' "); break;
		case 8: strcat(a, "F' "); break;
		case 9: strcat(a, "B' "); break;
		case 10: strcat(a, "R' "); break;
		case 11: strcat(a, "L' "); break;
		case 12: strcat(a, "U2 "); break;
		case 13: strcat(a, "D2 "); break;
		case 14: strcat(a, "F2 "); break;
		case 15: strcat(a, "B2 "); break;
		case 16: strcat(a, "R2 "); break;
		case 17: strcat(a, "L2 "); break;
		}
}

vector<int> AStarSearch::GetSuccessors_G1(vector<int> path, int state)
{
	vector<int> PossibleMoves;

	switch (state)
	{
	case 0:
	{
		for (int i = 0; i < 12; i++)
		{
			if (path.size() > 0 &&
				(path[path.size() - 1] < 6 && path[path.size() - 1] + 6 == i
				|| path[path.size() - 1] > 5 && path[path.size() - 1] - 6 == i)) continue;

			PossibleMoves.push_back(i);
		}
		break;
	}

	case 1:
	{
		for (int i = 0; i < 12; i++)
		{
			if (i == 2 || i == 8 || i == 3 || i == 9 || i == 12 || i == 13) continue;

			if (path.size() > 0 &&
				(path[path.size() - 1] < 6 && path[path.size() - 1] + 6 == i
					|| path[path.size() - 1] > 5 && path[path.size() - 1] - 6 == i
				|| path[path.size() - 1] == i && i > 11)) continue;

			PossibleMoves.push_back(i);
		}
		break;
	}
	}

	return PossibleMoves;
}

vector<int> AStarSearch::GetSuccessors_Solve(vector<int> path)
{
	vector<int> PossibleMoves;

	for (int i = 0; i < 18; i++)
	{
		if (i < 1 || i == 6 || i == 7)
		{
			if (path.size() > 0 &&
				(path[path.size() - 1] < 6 && path[path.size() - 1] + 6 == i
					|| path[path.size() - 1] > 5 && path[path.size() - 1] - 6 == i)) continue;
		}
		else
		{
			if (path.size() > 0 && (path[path.size() - 1] == i)) continue;
		}

		PossibleMoves.push_back(i);
	}

	return PossibleMoves;
}

void AStarSearch::Algorithm(PuzzleState& startNode)
{
	startNode.RC.copyRC(startNode.RC.rc, SearchSolution(startNode).RC.rc);
}

PuzzleState startCube;

PuzzleState AStarSearch::SearchSolution(PuzzleState startNode)
{
	Timer t;
	startCube = startNode;
	vector<int> path;
	char a[256] = "";

	// PART 1 -----------------

	int threshold = startCube.GoalDistanceEstimate_G1(startCube.RC, 0) + 1;
	int nextThreshold;

	while (true) // Orienting The Edges
	{
		nextThreshold = 0;

		bool found = DFS_EdgeOrient(0, path, threshold, nextThreshold, startCube.RC);

		if (found) break;

		threshold = nextThreshold;

		cout <<"Threshold has been updated: " << threshold <<" / " << t.elapsed()<<endl;
		t.reset();
	}

	returnMovesInChar(path, a);
	startCube.RC.queueMove(startCube.RC, a);
	startCube.RC.ReadRubik(startCube.RC.dir, startCube.RC.rc);
	cout << endl << "Part 1 Solution: " << a << endl;

	// PART 2 -----------------

	path.clear();
	threshold = startCube.GoalDistanceEstimate_Solve(startCube.RC) + 1;

	while (true) // Solving G1 state
	{
		nextThreshold = 0;

		bool found = DFS_Solve(0, path, threshold, nextThreshold, startCube.RC);

		if (found) break;

		threshold = nextThreshold;

		cout << endl << "Threshold has been updated: " << threshold << " / " << t.elapsed();
		t.reset();
	}

	returnMovesInChar(path, a);
	startCube.RC.queueMove(startCube.RC, a);
	startCube.RC.ReadRubik(startCube.RC.dir, startCube.RC.rc);
	cout << endl << "Part 2 Solution: " << a << endl;

	return startCube;
}

bool AStarSearch::DFS_EdgeOrient(int G, vector<int>& path, int threshold, int &nextThreshold, RubikCube RC)
{	
	int min = -1;

	int	g = G,
		h = startCube.GoalDistanceEstimate_G1(RC, 0),
		f = h + g;

	if (f > threshold)
	{
		nextThreshold = f;
		return false;
	}

	if (h == 0) return true;

	for (int nextMove : GetSuccessors_G1(path, 0))
	{
		char a[4];

		path.push_back(nextMove);
		returnMovesInChar({ nextMove }, a);
		RC.queueMove(RC, a);

		bool result = DFS_EdgeOrient(g + 5, path, threshold, nextThreshold, RC);
		if (result == true) return result;

		if (min > nextThreshold || min == -1) min = nextThreshold;

		returnMovesInChar({ nextMove + (nextMove > 5 ? -6 : 6) }, a);
		RC.queueMove(RC, a);
		path.pop_back();
	}

	nextThreshold = min;
	return false;
}

bool AStarSearch::DFS_CornerOrient(int G, vector<int>& path, int threshold, int& nextThreshold, RubikCube RC)
{
	int min = -1;

	int	g = G + 10,
		h = startCube.GoalDistanceEstimate_G1(RC, 1),
		f = h + g;

	if (f > threshold)
	{
		nextThreshold = f;
		return false;
	}

	if (h == 0) return true;

	for (int nextMove : GetSuccessors_G1(path, 1))
	{
		char a[4];

		path.push_back(nextMove);
		returnMovesInChar({ nextMove }, a);
		RC.queueMove(RC, a);

		bool result = DFS_CornerOrient(g, path, threshold, nextThreshold, RC);
		if (result == true) return result;

		if (min > nextThreshold || min == -1) min = nextThreshold;

		if (nextMove > 11) returnMovesInChar({ nextMove }, a);
		else returnMovesInChar({ nextMove + (nextMove > 5 ? -6 : 6) }, a);
		RC.queueMove(RC, a);
		path.pop_back();
	}

	nextThreshold = min;
	return false;
}

bool AStarSearch::DFS_Solve(int G, vector<int>& path, int threshold, int& nextThreshold, RubikCube RC)
{
	int min = -1;

	int	g = G,
		h = startCube.GoalDistanceEstimate_Solve(RC),
		f = h + g;

	if (f > threshold)
	{
		nextThreshold = f;
		return false;
	}

	if (h == 0) return true;

	for (int nextMove : GetSuccessors_Solve(path))
	{
		char a[4];

		path.push_back(nextMove);
		returnMovesInChar({ nextMove }, a);
		RC.queueMove(RC, a);

		bool result = DFS_Solve(g + 10, path, threshold, nextThreshold, RC);
		if (result == true) return result;

		if (min > nextThreshold || min == -1) min = nextThreshold;

		if (nextMove > 11) returnMovesInChar({ nextMove }, a);
		else  returnMovesInChar({ nextMove + (nextMove > 5 ? -6 : 6) }, a);
		RC.queueMove(RC, a);
		path.pop_back();
	}

	nextThreshold = min;
	return false;
}
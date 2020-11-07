#pragma once
#include "Sudoku.h"

#define PERFORMANCE_MODE
#undef PERFORMANCE_MODE

#define POSSIBILTY_ARRAY_SIZE (ROW * ROW * NUMBER_COUNT)

enum ESolveResult
{
	E_UNSOLVABLE = 0,
	E_SOLVED_HEURISTIC,
	E_SOLVED_BRUTE_FORCE
};

class SudokuSolver
{
private:
	ESolveResult _solve(Sudoku* s);
	bool applyTechniques;
	bool stepByStep;
	int foundCellCount;
	void InitPossibilityArray(Sudoku * s, int * possibilityArr);
	int PassOverSudoku(Sudoku * s);
	int ApplyNakedSingle( int* arr, Sudoku* s);
	int ApplyHiddenSingle(int * possibilityArr, Sudoku * s);
	bool ApplyNakedPair(int * possibilityArr, Sudoku * s);
	bool ApplyHiddenPair(int * possibilityArr, Sudoku * s);
	void Update(int x, int y, int val, int* arr, Sudoku* s, std::string msg);
	int RemovePossibilitiesAtSameBox(int x, int y, int * possibilities, int val);
	int RemoveAllPossibilities(int x, int y, int * possibilities);
	int RemovePossibilitiesAtSameColumn(int y, int * possibilities, int val);
	int RemovePossibilitiesAtSameRow(int x, int * possibilities, int val);

	inline bool IsEqualTuple(int x1, int y1, int x2, int y2)
	{
		return (x1 == x2 && y1 == y2) || (x1 == y2 && x2 == y1);
	}

	void PrintStepInfo(std::string msg, Sudoku * s);
public:
	void ApplySudokuTechniques(bool val) { applyTechniques = val; }
	void SolveStepByStep(bool val) { stepByStep = val; }
	int GetSolvedCellCount() { return foundCellCount; }
	SudokuSolver();
	ESolveResult Solve(Sudoku* s);
	~SudokuSolver();
};


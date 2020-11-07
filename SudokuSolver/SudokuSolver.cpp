#include "SudokuSolver.h"
#include <iostream>
#include <assert.h>
#include <conio.h>
#include <sstream>

SudokuSolver::SudokuSolver()
{
	applyTechniques = false;
	stepByStep = false;
	foundCellCount = 0;
}

ESolveResult SudokuSolver::Solve(Sudoku * s)
{
	if (applyTechniques)
	{
		foundCellCount = PassOverSudoku(s);

		int row, col;
		if (!s->getFirstUnassignedPos(row, col))
		{
			PrintStepInfo("Sudoku completely solved with applied techniques.", s);
			return E_SOLVED_HEURISTIC;
		}
		else
		{
			PrintStepInfo("Applied techniques couldn't completely solve sudoku, starting to recursive brute force algorithm", s);
		}
	}
	return _solve(s);
}


ESolveResult SudokuSolver::_solve(Sudoku* s)
{
	int row, col;
	if (!s->getFirstUnassignedPos(row, col))
	{
		return E_SOLVED_BRUTE_FORCE;
	}

	for (int num = 1; num <= ROW; num++)
	{
		if (s->isSafe(row, col, num))
		{
			std::stringstream ss;
			ss << "Trying " << num << " for cell ( " << row+1 << ", " << col+1 << ")... ";
			s->set(row, col, num, false, ss.str() );
			PrintStepInfo("Cell filled", s);
			if (_solve(s))
			{
				return E_SOLVED_BRUTE_FORCE;
			}

			ss.str("");
			ss << "Trial failed for cell ( " << row+1 << ", " << col+1 << ")  with number " << num << ", reverting to UNASSIGNED ";
			s->set(row, col, UNASSIGNED, false, ss.str());
			PrintStepInfo("Cell cleared", s);
		}
	}
	return E_UNSOLVABLE;
}

void SudokuSolver::PrintStepInfo(std::string msg, Sudoku * s)
{
#ifndef PERFORMANCE_MODE
	if (stepByStep)
	{
		system("cls");
		std::cout << "********************************************************************" << std::endl;
		std::cout << "STEP INFO: " << msg << std::endl;
		s->print();
		std::cout << "( Press anykey to next step; 'q' to last step)" << std::endl;
		std::cout << "********************************************************************" << std::endl;
		char c = _getch();
		if (c == 'q' || c == 'Q')
		{
			stepByStep = false;
		}
	}
#endif
}


int SudokuSolver::PassOverSudoku(Sudoku * s)
{
	int* possibilityArr = new int[POSSIBILTY_ARRAY_SIZE];
	bool hasNewClue = false;
	int row, col;
	int totalFoundCellCount = 0;
	// Init possibility array
	InitPossibilityArray(s, possibilityArr);
	do
	{
		PrintStepInfo("Turn starting", s);
		
		hasNewClue = false;


		int foundCellCount = ApplyNakedSingle(possibilityArr, s);
		totalFoundCellCount += foundCellCount;

		hasNewClue |= ( foundCellCount != 0);

		foundCellCount = ApplyHiddenSingle(possibilityArr, s);
		totalFoundCellCount += foundCellCount;

		hasNewClue |= (foundCellCount != 0);

		hasNewClue |= ApplyNakedPair(possibilityArr, s);

		hasNewClue |= ApplyHiddenPair(possibilityArr, s); 

		PrintStepInfo("Turn finished", s);

		if (!s->getFirstUnassignedPos(row, col))// Sudoku finished, no need for further investigation
		{
			break;
		}
	} while (hasNewClue);

	delete[] possibilityArr;

	return totalFoundCellCount;
}

void SudokuSolver::InitPossibilityArray(Sudoku * s, int * possibilityArr)
{
	memset(possibilityArr, 0, POSSIBILTY_ARRAY_SIZE * sizeof(int));
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < ROW; j++)
		{
			if (s->get(i, j) == UNASSIGNED)
			{
				int counter = 0;
				int startIndex = (i * ROW + j) * NUMBER_COUNT;
				for (int num = 1; num <= ROW; num++)
				{
					if (s->isSafe(i, j, num))
					{
						possibilityArr[startIndex + counter] = num;
						counter++;
					}
				}
			}
		}
	}
}

int SudokuSolver::ApplyNakedSingle(int * arr, Sudoku * s)
{
	int newClueCount = 0;
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < ROW; j++)
		{
			int startIndex = (i * ROW + j) * NUMBER_COUNT;
			if( arr[startIndex] != 0 && arr[startIndex+1] == 0) // If there is only 1 possibility
			{
				newClueCount++;
				Update(i, j, arr[startIndex], arr, s, "Cell has only 1 choice" );
			}
		}
	}
	return newClueCount;
}

int SudokuSolver::ApplyHiddenSingle(int * possibilityArr, Sudoku * s)
{
	// For every row, column and box; we will search for numbers from 1 to 9, and check if any number has only one place.
	// For example: 2-9-5-(1/3/8)-(3/8)-7-(3/6/8)-(3/6)-(3/6) at this row it is obvious that 4th number is 1. 
	
	int newClueCount = 0;
	// Search in rows
	for (int i = 0; i < ROW; i++)
	{
		int numberPossibilities[NUMBER_COUNT][2] = { 0 };// 2d array keeps possibility count and the last index of possibility. If possibility count is 1, we will use the index.
		for (int j = 0; j < ROW; j++)
		{
			int startIndex = (i * ROW + j) * NUMBER_COUNT;
			for (int k = 0; k < NUMBER_COUNT; k++)
			{
				if (possibilityArr[startIndex + k] == 0)
				{
					break;
				}
				numberPossibilities[possibilityArr[startIndex + k] - 1][0]++;
				numberPossibilities[possibilityArr[startIndex + k] - 1][1] = j;
			}
		}

		for (int n = 0; n < NUMBER_COUNT; n++)
		{
			if (numberPossibilities[n][0] == 1)
			{
				newClueCount++;
				Update(i, numberPossibilities[n][1], n + 1, possibilityArr, s, "Number has only 1 possible place for this row");
			}
		}
	}

	// Search in columns
	for (int j = 0; j < ROW; j++)
	{
		int numberPossibilities[NUMBER_COUNT][2] = { 0 };// 2d array keeps possibility count and the last index of possibility. If possibility count is 1, we will use the index.
		for (int i = 0; i < ROW; i++)
		{
			int startIndex = (i * ROW + j) * NUMBER_COUNT;
			for (int k = 0; k < NUMBER_COUNT; k++)
			{
				if (possibilityArr[startIndex + k] == 0)
				{
					break;
				}
				numberPossibilities[possibilityArr[startIndex + k] - 1][0]++;
				numberPossibilities[possibilityArr[startIndex + k] - 1][1] = i;
			}
		}
		for (int n = 0; n < NUMBER_COUNT; n++)
		{
			if (numberPossibilities[n][0] == 1)
			{
				newClueCount++;
				Update(numberPossibilities[n][1], j, n + 1, possibilityArr, s, "Number has only 1 possible place for this column");
			}
		}
	}
	
	// Search in boxes
	for (int k = 0; k < 3; k++)
	{
		for (int n = 0; n < 3; n++)
		{
			int numberPossibilities[NUMBER_COUNT][3] = { 0 };
			for (int i = 0; i < 3; i++)
			{
				int x = (3 * k + i);
				for (int j = 0; j < 3; j++)
				{
					int y = (3 * n) + j;

					int pos = ((x * ROW) + y);
					int startIndex = pos * NUMBER_COUNT;
					for (int m = 0; m < NUMBER_COUNT; m++)
					{
						if (possibilityArr[startIndex + m] == 0)
						{
							break;
						}
						numberPossibilities[possibilityArr[startIndex + m] - 1][0]++;
						numberPossibilities[possibilityArr[startIndex + m] - 1][1] = x;
						numberPossibilities[possibilityArr[startIndex + m] - 1][2] = y;
					}

				}
			}
			for (int p = 0; p < NUMBER_COUNT; p++)
			{
				if (numberPossibilities[p][0] == 1)
				{
					newClueCount++;
					Update(numberPossibilities[p][1], numberPossibilities[p][2], p + 1, possibilityArr, s, "Number has only 1 possible place in box" );
				}
			}
		}
	}
	return newClueCount;
}

bool SudokuSolver::ApplyHiddenPair(int * possibilityArr, Sudoku * s)
{
	bool hasNewClue = false;
	// Search in rows
	for (int i = 0; i < ROW; i++)
	{
		int numberPossibilities[NUMBER_COUNT][3] = { 0 };// 2d array keeps possibility count and the last index of possibility. If possibility count is 1, we will use the index.
		for (int j = 0; j < ROW; j++)
		{
			int startIndex = (i * ROW + j) * NUMBER_COUNT;
			for (int k = 0; k < NUMBER_COUNT; k++)
			{
				int number = possibilityArr[startIndex + k];
				if ( number == 0 )
				{
					break;
				}
				numberPossibilities[ number - 1 ][0]++; // Increase count
				if (numberPossibilities[number - 1][1] == -1) // Check if first cell index assigned before, if not directly assign
				{
					numberPossibilities[number - 1][1] = j;
				}
				else if(numberPossibilities[number - 1][2] == -1)// Check if second cell index assigned before, if not directly assign
				{
					numberPossibilities[number - 1][2] = j;
				}
				else // If both cells assigned, swap with first and second, and assign new index as second cell index.
				{
					numberPossibilities[number - 1][1] = numberPossibilities[number - 1][2];
					numberPossibilities[number - 1][2] = j;
				}
			}
		}

		for (int n = 0; n < NUMBER_COUNT; n++)
		{
			if (numberPossibilities[n][0] == 2)
			{
				for (int m = n + 1; m < NUMBER_COUNT; m++)
				{
					if (numberPossibilities[m][0] == 2)
					{
						if (IsEqualTuple(numberPossibilities[n][1], numberPossibilities[n][2], numberPossibilities[m][1], numberPossibilities[m][2]))
						{// If both x's and y's are equal, this 2 numbers share same 2 cells. We can remove this numbers from possibilities array for this row
							int count1 = RemoveAllPossibilities(i, numberPossibilities[n][1], possibilityArr);
							int pos = (i * ROW + numberPossibilities[n][1]) * NUMBER_COUNT;
							possibilityArr[pos] = n+1;
							possibilityArr[pos + 1] = m+1;

							int count2 = RemoveAllPossibilities(i, numberPossibilities[n][2], possibilityArr);
							pos = (i * ROW + numberPossibilities[n][2]) * NUMBER_COUNT;
							possibilityArr[pos] = n + 1;
							possibilityArr[pos + 1] = m + 1;
							
							if (count1 > 2 || count2 > 2)
							{
								hasNewClue = true;
#ifndef PERFORMANCE_MODE
								std::stringstream ss;
								ss << m + 1 << " and " << n + 1 << " removed from possibilities at row " << i + 1
									<< " Because these 2 numbers can go only " << numberPossibilities[n][1] + 1 << " & " << numberPossibilities[n][2] + 1;
								PrintStepInfo(ss.str(), s);
#endif

							}
						}
					}
				}
			}
		}
	}

	// Search in columns
	for (int j = 0; j < ROW; j++)
	{
		int numberPossibilities[NUMBER_COUNT][3] = { 0 };// 2d array keeps possibility count and the last index of possibility. If possibility count is 1, we will use the index.
		for (int i = 0; i < ROW; i++)
		{
			int startIndex = (i * ROW + j) * NUMBER_COUNT;
			for (int k = 0; k < NUMBER_COUNT; k++)
			{
				int number = possibilityArr[startIndex + k];
				if (number == 0)
				{
					break;
				}
				numberPossibilities[number - 1][0]++; // Increase count
				if (numberPossibilities[number - 1][1] == -1) // Check if first cell index assigned before, if not directly assign
				{
					numberPossibilities[number - 1][1] = i;
				}
				else if (numberPossibilities[number - 1][2] == -1)// Check if second cell index assigned before, if not directly assign
				{
					numberPossibilities[number - 1][2] = i;
				}
				else // If both cells assigned, swap with first and second, and assign new index as second cell index.
				{
					numberPossibilities[number - 1][1] = numberPossibilities[number - 1][2];
					numberPossibilities[number - 1][2] = i;
				}
			}
		}
		for (int n = 0; n < NUMBER_COUNT; n++)
		{
			if (numberPossibilities[n][0] == 2)
			{
				for (int m = n + 1; m < NUMBER_COUNT; m++)
				{
					if (numberPossibilities[m][0] == 2)
					{
						if (IsEqualTuple(numberPossibilities[n][1], numberPossibilities[n][2], numberPossibilities[m][1], numberPossibilities[m][2]))
						{// If both x's and y's are equal, this 2 numbers share same 2 cells. We can remove this numbers from possibilities array for this column
							int count1 = RemoveAllPossibilities(numberPossibilities[n][1], j,  possibilityArr);
							int pos = (numberPossibilities[n][1] * ROW + j) * NUMBER_COUNT;
							possibilityArr[pos] = n + 1;
							possibilityArr[pos + 1] = m + 1;

							int count2 = RemoveAllPossibilities(numberPossibilities[n][2], j, possibilityArr);
							pos = (numberPossibilities[n][2] * ROW + j) * NUMBER_COUNT;
							possibilityArr[pos] = n + 1;
							possibilityArr[pos + 1] = m + 1;
							if (count1 > 2 || count2 > 2)
							{
								hasNewClue = true;
#ifndef PERFORMANCE_MODE
								std::stringstream ss;
								ss << m + 1 << " and " << n + 1 << " removed from possibilities at col " << j + 1
									<< " Because these 2 numbers can go only " << numberPossibilities[n][1] + 1 << " & " << numberPossibilities[n][2] + 1;
								PrintStepInfo(ss.str(), s);
#endif
							}
						}
					}
				}
			}
		}
	}

	// Search in boxes
	for (int k = 0; k < 3; k++)
	{
		for (int n = 0; n < 3; n++)
		{
			int numberPossibilities[NUMBER_COUNT][5] = { 0 };
			for (int i = 0; i < 3; i++)
			{
				int x = (3 * k + i);
				for (int j = 0; j < 3; j++)
				{
					int y = (3 * n) + j;

					int pos = ((x * ROW) + y);
					int startIndex = pos * NUMBER_COUNT;
					for (int m = 0; m < NUMBER_COUNT; m++)
					{
						int number = possibilityArr[startIndex + k];
						if (number == 0)
						{
							break;
						}
						numberPossibilities[number - 1][0]++; // Increase count
						if (numberPossibilities[number - 1][1] == -1) // Check if first cell index assigned before, if not directly assign
						{
							numberPossibilities[number - 1][1] = x;
							numberPossibilities[number - 1][2] = y;
						}
						else if (numberPossibilities[number - 1][3] == -1)// Check if second cell index assigned before, if not directly assign
						{
							numberPossibilities[number - 1][3] = x;
							numberPossibilities[number - 1][4] = y;
						}
						else // If both cells assigned, swap with first and second, and assign new index as second cell index.
						{
							numberPossibilities[number - 1][1] = numberPossibilities[number - 1][3];
							numberPossibilities[number - 1][3] = x;
							numberPossibilities[number - 1][2] = numberPossibilities[number - 1][4];
							numberPossibilities[number - 1][4] = y;
						}
					}
				}
			}
			for (int l = 0; l < NUMBER_COUNT; l++)
			{
				if (numberPossibilities[l][0] == 2)
				{
					for (int m = l + 1; m < NUMBER_COUNT; m++)
					{
						if (numberPossibilities[m][0] == 2)
						{
							int x1, x2, y1, y2;
							x1 = numberPossibilities[l][1];
							y1 = numberPossibilities[l][2];

							x2 = numberPossibilities[l][3];
							y2 = numberPossibilities[l][4];

							if ( IsEqualTuple(x1, x2, numberPossibilities[m][1], numberPossibilities[m][3]) 
								&& IsEqualTuple(y1, y2, numberPossibilities[m][2], numberPossibilities[m][4]) )
								// If both x's and y's are equal, this 2 numbers share same 2 cells. We can remove this numbers from possibilities array for this box
							{
								int count1 = RemoveAllPossibilities( x1, y1, possibilityArr);
								int count2 = RemoveAllPossibilities( x2, y2, possibilityArr);
								int pos = (x1 * ROW + y1) * NUMBER_COUNT;
								possibilityArr[pos] = l + 1;
								possibilityArr[pos + 1] = m + 1;
										

								pos = (x2 * ROW + y2) * NUMBER_COUNT;
								possibilityArr[pos] = l + 1;
								possibilityArr[pos + 1] = m + 1;

								if (count1 > 2 || count2 > 2)
								{
									hasNewClue = true;
#ifndef PERFORMANCE_MODE
									std::stringstream ss;
									ss << m + 1 << " and " << n + 1 << " removed from possibilities at box "
										<< " Because these 2 numbers can go only ( " << numberPossibilities[n][1] + 1 << ", " << numberPossibilities[n][2] + 1
										<< " ) and ( " << numberPossibilities[n][3] + 1 << ", " << numberPossibilities[n][4] + 1 << " ) ";
									PrintStepInfo(ss.str(), s);
#endif
								}
							}
						}
					}
				}
			}
		}
	}
	return hasNewClue;
}

bool SudokuSolver::ApplyNakedPair(int * possibilities, Sudoku * s)
{
	bool hasNewClue = false;
	for (int i = 0; i < ROW; i++)
	{
		int doubleIndices[9];
		int counter = 0;
		memset(doubleIndices, -1, sizeof(int) * 9);
		for (int j = 0; j < ROW; j++)
		{
			int startIndex = (i * ROW + j) * NUMBER_COUNT;
			if (possibilities[startIndex] != 0 && possibilities[startIndex + 1] != 0 && possibilities[startIndex + 2] == 0) // If there is only 1 possibility
			{
				doubleIndices[counter++] = j;
			}
		}

		for (int k = 0; k < counter - 1; k++)
		{
			for (int l = k + 1; l < counter; l++)
			{
				int firstIndex = (i*ROW + doubleIndices[k]) * NUMBER_COUNT;
				int secondIndex = (i*ROW + doubleIndices[l]) * NUMBER_COUNT;

				if(  IsEqualTuple(	possibilities[firstIndex], possibilities[firstIndex + 1], 
									possibilities[secondIndex], possibilities[secondIndex + 1]) )
				{
					int val1 = possibilities[firstIndex];
					int val2 = possibilities[firstIndex+1];

					int n = RemovePossibilitiesAtSameRow(i, possibilities, val1);
					int m = RemovePossibilitiesAtSameRow(i, possibilities, val2);

					// RemovePossibilitiesAtSameRow completely also removes possibilities from these cells, so we need to add again.
					possibilities[firstIndex] = possibilities[secondIndex] = val1;
					possibilities[firstIndex+1] = possibilities[secondIndex+1] = val2;
					if ( n > 2 || m > 2 ) // If these cells are not last 2 cells and we removed at least 1 more possibility, we have new clue
					{
#ifndef PERFORMANCE_MODE
						std::stringstream ss;
						ss << "New clue: " << val1 << " and " << val2 << " can go only " << doubleIndices[k] + 1 << " & " << doubleIndices[l] + 1 << " at row " << i + 1;
						PrintStepInfo(ss.str(), s);
#endif
						hasNewClue = true;
					}
				}
			}
		}
	}

	for (int j = 0; j < ROW; j++)
	{
		int doubleIndices[9];
		int counter = 0;
		memset(doubleIndices, -1, sizeof(int) * 9);
		for (int i = 0; i < ROW; i++)
		{
			int startIndex = (i * ROW + j) * NUMBER_COUNT;
			if (possibilities[startIndex] != 0 && possibilities[startIndex + 1] != 0 && possibilities[startIndex + 2] == 0) // If there is only 1 possibility
			{
				doubleIndices[counter++] = i;
			}
		}

		for (int k = 0; k < counter - 1; k++)
		{
			for (int l = k + 1; l < counter; l++)
			{
				int firstIndex = (doubleIndices[k] * ROW + j) * NUMBER_COUNT;
				int secondIndex = (doubleIndices[l] * ROW + j) * NUMBER_COUNT;

				if ( IsEqualTuple(	possibilities[firstIndex], possibilities[firstIndex + 1],
										possibilities[secondIndex], possibilities[secondIndex + 1]))
				{
					int val1 = possibilities[firstIndex];
					int val2 = possibilities[firstIndex+1];
					int m = RemovePossibilitiesAtSameColumn(j, possibilities, val1);
					int n = RemovePossibilitiesAtSameColumn(j, possibilities, val2);
					
					// RemovePossibilitiesAtSameRow completely also removes possibilities from these cells, so we need to add again.
					possibilities[firstIndex] = possibilities[secondIndex] = val1;
					possibilities[firstIndex + 1] = possibilities[secondIndex + 1] = val2;
					if (n > 2 || m > 2) // If these cells are not last 2 cells and we removed at least 1 more possibility, we have new clue
					{
#ifndef PERFORMANCE_MODE
						std::stringstream ss;
						ss << "New clue: " << val1 << " and " << val2 << " can go only " << doubleIndices[k] + 1 << " & " << doubleIndices[l] + 1 << " at column " << j + 1;
						PrintStepInfo(ss.str(), s);
#endif
						hasNewClue = true;
					}
				}
			}
		}
	}

	for (int boxX = 0; boxX < 3; boxX++)
	{
		for (int boxY = 0; boxY < 3; boxY++)
		{

			int doubleIndices[9*2];
			int counter = 0;
			memset(doubleIndices, -1, sizeof(int) * 9 * 2);

			for (int i = 0; i < 3; i++)
			{
				int x = (3 * boxX + i);
				for (int j = 0; j < 3; j++)
				{
					int y = (3 * boxY) + j;

					int pos = ((x * ROW) + y);
					int startIndex = pos * NUMBER_COUNT;
					if (possibilities[startIndex] != 0 && possibilities[startIndex + 1] != 0 && possibilities[startIndex + 2] == 0) // If there is only 1 possibility
					{
						doubleIndices[counter++] = x;
						doubleIndices[counter++] = y;
					}

				}
			}

			for (int k = 0; k < counter - 2; k+=2)
			{
				for (int l = k + 2; l < counter; l+=2)
				{
					int firstIndex = (doubleIndices[k] * ROW + doubleIndices[k+1]) * NUMBER_COUNT;
					int secondIndex = (doubleIndices[l] * ROW + doubleIndices[l+1]) * NUMBER_COUNT;

					if ( IsEqualTuple(	possibilities[firstIndex], possibilities[firstIndex + 1],
										possibilities[secondIndex], possibilities[secondIndex + 1]))
					{
						int val1 = possibilities[firstIndex];
						int val2 = possibilities[firstIndex + 1];

						int m = RemovePossibilitiesAtSameBox(doubleIndices[k], doubleIndices[k + 1], possibilities, val1);
						int n = RemovePossibilitiesAtSameBox(doubleIndices[l], doubleIndices[l+1], possibilities, val2);

						// RemovePossibilitiesAtSameRow completely also removes possibilities from these cells, so we need to add again.
						possibilities[firstIndex] = possibilities[secondIndex] = val1;
						possibilities[firstIndex + 1] = possibilities[secondIndex + 1] = val2;
						if (n > 2 || m > 2) // If these cells are not last 2 cells and we removed at least 1 more possibility, we have new clue
						{
#ifndef PERFORMANCE_MODE
							std::stringstream ss;
							ss << "New clue: " << val1 << " and " << val2 << " can go only ( "
									<< doubleIndices[k] + 1 << ", " << doubleIndices[k + 1] + 1 << " ) & ( "
									<< doubleIndices[l] + 1 << ", " << doubleIndices[l + 1] + 1 << " ) at  box " << std::endl;
							PrintStepInfo(ss.str(), s);
#endif
							hasNewClue = true;
						}
					}
				}
			}
		}
	}
	return hasNewClue;
}

void SudokuSolver::Update(int x, int y, int val, int* possibilities, Sudoku* s, std::string msg) 
{
	// Set the value, also clear corresponding area of possibility array
	int startIndex = (x * ROW + y) * NUMBER_COUNT;
	s->set(x, y, val, true, msg);

	memset(&possibilities[startIndex], 0, sizeof(int) * NUMBER_COUNT);// This box is set, so no need for possibility array
	
	// Remove possibilities at same row
	RemovePossibilitiesAtSameRow(x, possibilities, val);
	
	// Remove possibilities at same column
	RemovePossibilitiesAtSameColumn(y, possibilities, val);

	// Remove possibilities at same box
	RemovePossibilitiesAtSameBox(x, y, possibilities, val);

	PrintStepInfo("A new cell solved.", s);

}

int SudokuSolver::RemovePossibilitiesAtSameBox(int x, int y, int * possibilities, int val)
{
	int counter = 0;
	int boxStartRow = x - x % 3;
	int boxStartCol = y - y % 3;
	for (int row = 0; row < ROW / 3; row++)
	{
		int rowPos = (row + boxStartRow);
		for (int col = 0; col < ROW / 3; col++)
		{
			int colPos = col + boxStartCol;
			int startIndex = (rowPos * ROW + colPos) * NUMBER_COUNT;
			int foundIndex = -1;
			int j;
			for (j = 0; j < NUMBER_COUNT; j++)
			{
				if (possibilities[startIndex + j] == 0)// End of possibilities
				{
					break;
				}
				if (possibilities[startIndex + j] == val)// Set index for swap with last possibility
				{
					foundIndex = startIndex + j;
					counter++;
				}
			}
			if (foundIndex != -1)// Swap with last element and set last element to zero .
			{
				possibilities[foundIndex] = possibilities[startIndex + j - 1];
				possibilities[startIndex + j - 1] = 0;
			}
		}
	}
	return counter;
}

int SudokuSolver::RemoveAllPossibilities(int x, int y, int * possibilities)
{
	int pos = (x * ROW + y)* NUMBER_COUNT;
	int counter = 0;
	for (int i = 0; i < NUMBER_COUNT; i++)
	{
		if (possibilities[pos + i] != 0)
		{
			counter++;
			possibilities[pos + i] = 0;
		}
	}
	return counter;
}

int SudokuSolver::RemovePossibilitiesAtSameColumn(int y, int * possibilities, int val)
{
	int counter = 0;
	for (int i = 0; i < ROW; i++)
	{
		int foundIndex = -1;
		int startIndex = (i*ROW + y) * NUMBER_COUNT;
		int j;
		for (j = 0; j < NUMBER_COUNT; j++)
		{
			if (possibilities[startIndex + j] == 0)// End of possibilities
			{
				break;
			}
			if (possibilities[startIndex + j] == val)// Set index for swap with last possibility
			{
				foundIndex = startIndex + j;
				counter++;
			}
		}
		if (foundIndex != -1)// Swap with last element and set last element to zero .
		{
			possibilities[foundIndex] = possibilities[startIndex + j - 1];
			possibilities[startIndex + j - 1] = 0;
		}
	}
	return counter;
}

int SudokuSolver::RemovePossibilitiesAtSameRow(int x, int * possibilities, int val)
{
	int counter = 0;
	for (int i = 0; i < ROW; i++)
	{
		int foundIndex = -1;
		int startIndex = (x*ROW + i) * NUMBER_COUNT;
		int j;
		for (j = 0; j < NUMBER_COUNT; j++)
		{
			if (possibilities[startIndex + j] == 0)// End of possibilities
			{
				break;
			}
			if (possibilities[startIndex + j] == val)// Set index for swap with last possibility
			{
				foundIndex = startIndex + j;
				counter++;
			}
		}
		if (foundIndex != -1)// Swap with last element and set last element to zero .
		{
			possibilities[foundIndex] = possibilities[startIndex + j - 1];
			possibilities[startIndex + j - 1] = 0;
		}
	}
	return counter;
}

SudokuSolver::~SudokuSolver()
{
}

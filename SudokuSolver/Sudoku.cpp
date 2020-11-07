#include "Sudoku.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool Sudoku::usedInRow(int row, int val)
{
	for (int col = 0; col < ROW; col++)
		if (data[ row * ROW + col] == val)
			return true;
	return false;
}

bool Sudoku::usedInColumn(int col, int val)
{
	for (int row = 0; row < ROW; row++)
		if (data[ row * ROW + col] == val)
			return true;
	return false;
}

bool Sudoku::usedInBox(int x, int y, int val)
{
	int boxStartRow = x - x % 3;
	int boxStartCol = y - y % 3;
	for (int row = 0; row < ROW/3; row++)
		for (int col = 0; col < ROW/3; col++)
			if (data[ (row + boxStartRow) * ROW  + col + boxStartCol ] == val)
				return true;
	return false;
}

Sudoku::Sudoku()
{
	isCertain = false;
	lastX = lastY = -1;
	lastMessage = "";
	memset(data, 0, ROW * ROW * sizeof(int));
}

Sudoku::~Sudoku()
{
}

void Sudoku::setAll(int data[ROW * ROW])
{
	memcpy(this->data, data, ROW * ROW * sizeof(int));
}

void Sudoku::set(int x, int y, int val, bool certain, std::string msg)
{
	data[x*ROW + y] = val;

	lastX = x;
	lastY = y;
	isCertain = certain;
	
	lastMessage = msg;
	
}

int Sudoku::get(int x, int y)
{
	return data[x*ROW + y];
}

void Sudoku::print()
{
	std::cout << std::endl;
	if (lastX >= 0)
	{
		std::cout << "\tLast Cell: ( " << lastX + 1 << "," << lastY + 1 << " ) =  " << data[lastX*ROW + lastY] << std::endl;
		std::cout << "\tExplanation: " << lastMessage << std::endl << std::endl;
	}
	
	std::cout << "\t  | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |" << std::endl;
	std::cout << "\t  -------------------------------------" << std::endl;
	for (int i = 0; i < ROW; i++)
	{
		std::cout << "\t"<< i+1 <<" | ";
		for (int j = 0; j < ROW; j++)
		{
			bool isLastSetBox = false;
			bool foundWithHeuristic = false;

			isLastSetBox = (i == lastX && j == lastY);
			foundWithHeuristic  = isCertain;

			std::cout << data[i*ROW + j] << (isLastSetBox ? (foundWithHeuristic ? "*" : "?") : " ") << ((j+1)%3 == 0 ? "|" : " ") <<" ";
		}
		std::cout << std::endl;
		if( (i+1) % 3 == 0)
			std::cout << "\t-------------------------------------" << std::endl;
		else
			std::cout << "\t                                     " << std::endl;
	}
}

bool Sudoku::getFirstUnassignedPos(int &x, int &y)
{
	for (x = 0; x < ROW; x++)
		for (y = 0; y < ROW; y++)
			if (data[x * ROW + y] == UNASSIGNED)
				return true;
	return false;
}

int Sudoku::getFoundCellCount()
{
	int counter = 0;
	for (int x = 0; x < ROW; x++)
		for (int y = 0; y < ROW; y++)
			if (data[x * ROW + y] != UNASSIGNED)
				counter++;
	return counter;
}

bool Sudoku::init(std::string input)
{
	lastX = lastY = -1;
	lastMessage = "";
	isCertain = false;
	memset(data, 0, ROW * ROW * sizeof(int));

	std::ifstream in(input);
	if (!in.good())
	{
		return false;
	}
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < ROW; j++)
		{
			in >> data[i*ROW + j];
		}
	}
	in.close();
	return true;
}

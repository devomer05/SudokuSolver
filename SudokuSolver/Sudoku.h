#pragma once

#include <string>

#define ROW 9
#define NUMBER_COUNT 9
#define UNASSIGNED 0

class Sudoku
{
private:
	int data[ ROW * ROW ];
	bool usedInRow(int row, int val);
	bool usedInColumn(int col, int val);
	bool usedInBox(int x, int y, int val);
	
	int lastX;
	int lastY;
	std::string lastMessage;
	bool isCertain;

public:
	Sudoku();
	~Sudoku();
	void setAll(int data[ROW * ROW]);
	void set(int x, int y, int val, bool certain = false, std::string msg = "");
	int get(int x, int y);
	void print();
	bool getFirstUnassignedPos(int& row, int &col);
	int GetAssignedCellCount();
	bool init(std::string input);
	bool isSafe(int x, int y, int val)
	{
		return !usedInRow( x,  val) && !usedInBox( x, y, val) && !usedInColumn( y, val);
	}
};


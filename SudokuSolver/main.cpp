#include <iostream>
#include <ctime>
#include <Windows.h>
#include <vector>
#include <comdef.h>
#include <fstream>
#include <sstream>
#include "Sudoku.h"
#include "SudokuSolver.h"


std::wstring s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

void GetFileNames( std::string pathQuery, std::vector<std::string> &vs)
{
	std::string directory = pathQuery;
	pathQuery.append("\\*.*");

	std::wstring wPathQuery = s2ws(pathQuery);
	bool x = true;
	int i = 0;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile( wPathQuery.c_str(), &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			_bstr_t b(FindFileData.cFileName);
			const char* c = b;
			if (strcmp(c, ".") == 0 || strcmp(c, "..") == 0)
				continue;

			std::string fullName = directory;
			fullName.append("\\");
			fullName.append(c);

			vs.push_back(fullName);
		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
}

ESolveResult SolveSudokuAtFile(bool applyTechniques, bool stepByStep, std::string fileName)
{
	ESolveResult solved = E_UNSOLVABLE;

	SudokuSolver ss;

	ss.ApplySudokuTechniques(applyTechniques);
	ss.SolveStepByStep(stepByStep);

	Sudoku s;
	if (s.init(fileName))
	{
		std::cout << "Sudoku has initialized with success: " << fileName << " " << s.GetAssignedCellCount() << " hints;\t";
#ifndef PERFORMANCE_MODE
		s.print();
#endif
		std::clock_t begin, end;
		double elapsed_secs;
		begin = clock();
		solved = ss.Solve(&s) ;
		end = clock();
		elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

		if (solved == E_SOLVED_BRUTE_FORCE)
		{
			std::cout << "Sudoku solved (in " << elapsed_secs << " secs ) by using brute force";
			if (applyTechniques)
			{
				std::cout << " ( " << ss.GetSolvedCellCount() << " cells found by techniques )";
			}
			std::cout << std::endl;
			
		}
		else if (solved == E_SOLVED_HEURISTIC)
		{
			std::cout << "Sudoku solved (in " << elapsed_secs << " secs ) by using sudoku techniques" << std::endl;
		}
		else
		{
			std::cout << "Could not solve sudoku" << fileName << std::endl;
		}
#ifndef PERFORMANCE_MODE
		s.print();
#endif
	}
	else
	{
		std::cout << "Could not initialize sudoku file " << fileName << std::endl;
	}
	return solved;
}

void PerformanceTestFile(std::string fileName, int repeat)
{
	ESolveResult solved = E_UNSOLVABLE;

	std::clock_t begin, end;
	double elapsed_secs;

	std::cout << "********************************************************************" << std::endl ;
	Sudoku s;
	if (s.init(fileName))
	{
		std::cout << "Starting to solve " << fileName << ";\t Repeat count: " << repeat << std::endl << std::endl;
		SudokuSolver ss;
		ss.SolveStepByStep(false);

		Sudoku copy;

		ss.ApplySudokuTechniques(false);
		begin = clock();
		for (int i = 0; i < repeat; i++)
		{
			copy = s;
			solved = ss.Solve(&copy);
		}

		end = clock();
		elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

		if (solved == E_SOLVED_BRUTE_FORCE)
		{
			std::cout << "BRUTE FORCE:\t" << elapsed_secs << " seconds for " << repeat << " repetition. ( " << (elapsed_secs / repeat) << " avg )" << std::endl;
		}
		else
		{
			std::cout << "BRUTE FORCE:\tCould not solve sudoku " << fileName << std::endl;
		}

		ss.ApplySudokuTechniques(true);
		begin = clock();
		for (int i = 0; i < repeat; i++)
		{
			copy = s;
			solved = ss.Solve(&copy);
		}
		end = clock();
		elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

		if (solved == E_SOLVED_HEURISTIC)
		{
			std::cout << "HEURISTIC:\t" << elapsed_secs << " seconds for " << repeat << " repetition. ( " << (elapsed_secs / repeat) << " avg ) ( FULLY COMPLETED BY TECHNIQUES)" << std::endl;
		}
		else if (solved == E_SOLVED_BRUTE_FORCE)
		{
			std::cout << "HEURISTIC:\t" << elapsed_secs << " seconds for " << repeat << " repetition. ( " << (elapsed_secs / repeat) << " avg ) ( BRUTE FORCE AFTER TECHNIQUES )" << std::endl;
		}
		else
		{
			std::cout << "HEURISTIC:\tCould not solve sudoku " << fileName << std::endl;
		}
	}
	else
	{
		std::cout << "Could not initialize sudoku file " << fileName << std::endl;
	}
	std::cout << "********************************************************************" << std::endl << std::endl;
}

void SuccessTest( std::string filePath)
{
	std::vector<std::string> vs;
	GetFileNames(filePath, vs);

	int counterBrute = 0, counterHeuristic = 0, counterUnsolved = 0;
	for (int i = 0; i < vs.size(); i++)
	{
		ESolveResult e = SolveSudokuAtFile(true, false, vs[i]);
		if (e == E_SOLVED_BRUTE_FORCE)
			counterBrute++;
		else if (e == E_SOLVED_HEURISTIC)
			counterHeuristic++;
		else
			counterUnsolved++;
	}
	std::cout << " Solved(B) " << counterBrute << std::endl << " Solved(H) " << counterHeuristic << std::endl << " Unsolved " << counterUnsolved << std::endl << " Total: " << vs.size() << std::endl;

}


void PerformanceTest(std::string path, int repetition)
{
	std::cout << "Performance tests starting" << std::endl;
	std::vector<std::string> vs;
	GetFileNames(path, vs);
	for(int i = 0; i < vs.size(); i++)
		PerformanceTestFile( vs[i], repetition);
	std::cout << "All performance tests finished" << std::endl;
}

void PerformanceMenu()
{
	while (true)
	{
		system("cls");
		bool quit = false;
		std::cout << "*********************************************************" << std::endl;
		std::cout << "\t\t\t\t\tPERFORMANCE MODE" << std::endl;
		std::cout << "\t\t1- Performance Test Folder" << std::endl;
		std::cout << "\t\t2- Performance Test File" << std::endl;
		std::cout << "\t\t3- Success Test Folder" << std::endl;
		std::cout << "\t\t4- Success Test File" << std::endl << std::endl;
		std::cout << "\t\tq- Exit" << std::endl;
		char c;
		std::cin >> c;
		switch (c)
		{
		case '1':
		{
			std::string path;
			int repetition;
			std::cout << "Enter path of Sudoku files you want to run performance test and repetition" << std::endl;
			std::cin >> path >> repetition;
			system("cls");
			PerformanceTest(path, repetition);
			system("pause");
			break;
		}
		case '2':
		{
			std::string path;
			int repetition;
			std::cout << "Enter full path of Sudoku file you want to run performance test and repetition" << std::endl;
			std::cin >> path >> repetition;
			system("cls");
			PerformanceTestFile(path, repetition);
			system("pause");
			break;
		}
		case '3':
		{
			std::string path;
			std::cout << "Enter path of Sudoku files you want to run success test" << std::endl;
			std::cin >> path;
			system("cls");
			SuccessTest(path);
			system("pause");
			break;
		}
		case '4':
		{
			std::string fileName;
			std::cout << "Enter sudoku file you want to run success test" << std::endl;
			std::cin >> fileName;
			system("cls");
			SolveSudokuAtFile(true, false, fileName);
			system("pause");
			break;
		}
		case 'q':
		case 'Q': quit = true; break;
		}
		if (quit)
		{
			break;
		}
	}
}

int main(int argc, char* argsv[])
{
#ifdef PERFORMANCE_MODE
	PerformanceMenu();
#else

	bool applyTechniques = false;
	bool stepByStep = false;

	std::string fileName = "";
	if (argc > 1)
	{
		fileName = argsv[1];
		char c;
		std::cout << "Do you want to apply sudoku techniques? (y/n) ";
		std::cin >> c;
		applyTechniques = (c == 'y' || c == 'Y');

		std::cout << "Do you want to solve step by step ? (y/n) ";
		std::cin >> c;
		stepByStep = (c == 'y' || c == 'Y');

		SolveSudokuAtFile(applyTechniques, stepByStep, fileName);
	}
	else
	{
		while (true)
		{
			std::cout << "Enter path of sudoku file (\"exit\" to leave ): ";
			std::cin >> fileName;
			if (fileName == "exit")
				break;
			char c;
			std::cout << "Do you want to apply sudoku techniques? (y/n) ";
			std::cin >> c;
			applyTechniques = (c == 'y' || c == 'Y');

			std::cout << "Do you want to solve step by step ? (y/n) ";
			std::cin >> c;
			stepByStep = (c == 'y' || c == 'Y');

			SolveSudokuAtFile(applyTechniques, stepByStep, fileName);
		}
	}
#endif
	system("pause");
	return 0;
}
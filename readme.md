# Sudoku Solver
Sudoku Solver is a windows console application that tries to solve sudoku puzzles by using sudoku solution techniques.

## Introduction
The commonly known recursive brute force solution to a puzzle may take long times if given hints are not enough. But if we can crop possibilities; thisduration can be reduced. 

## Solution
To reduce possibilities at each cell, A possibility table used. This is an array of 9 possibilities per cell. After initializing this possibility array; the sudoku solution techniques; naked single, hidden single, naked pair and hidden pair; processed on it. This process keeps searching until these techniques can not find any new clue:

- naked single, 
    >Cells that has only 1 possibility due to all other numbers show up at same row, column or box.
	
- hidden single, 
	>If a number shows up at only one cell as a possibility, then this cell must be this number.
Ex: If Row is  2-9-5-(1/3/8)-(3/8)-7-(3/6/8)-(3/6)-(3/6);  then 4. cell is "1" .
-  naked pair,
	>2 Cells have same 2 possibilities, then it is obvious that these 2 numbers will go these 2 cells. So we can remove these numbers from possibility list of other cells.
	Ex: If row is  2-9-5-(1/3/8)-(4/8)-7-(4/6/8)-(3/6)-(3/6) last 2 cells are obviously 3 and 6, so we can remove 3 and 6 from other cells :  2-9-5-(1/8)-(4/8)-7-(4/8)-(3/6)-(3/6).

- hidden pair
    > If  2numbers show up at only 2 cells as a possibility, then this cells must be this numbers. 
Ex: If Row is  2-9-5-(1/3/8)-(4/3/8)-7-(1/4/6)-(4/6)-(1/4/6);   3&8 only can go 3. and 4. cells. 
Then these cells must be 3 or 8. we can remove other possibilities here:  2-9-5-(3/8)-(3/8)-7-(1/4/6)-(4/6)-(1/4/6))

After techniques applied, if Sudoku puzzle could not be solved completely, the recursive brute force algorithm takes charge. Due to new cells and hints
found, brute force takes less time compared to fully recursive brute force solution.
### Usage

You can run exe with an argument which is a path of sudoku file as
```sh
.\SudokuSolver.exe "sudoku.txt"
Do you want to apply sudoku techniques? (y/n) y
Do you want to solve step by step ? (y/n) n
Sudoku has initialized with success: sudoku.txt 25 hints;
          | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
          -------------------------------------
        1 | 5   8   6 | 0   7   0 | 0   0   0 |
        2 | 0   0   0 | 9   0   1 | 6   0   0 |
        3 | 0   0   0 | 6   0   0 | 0   0   0 |
        -------------------------------------
        4 | 0   0   7 | 0   0   0 | 0   0   0 |
        5 | 9   0   2 | 0   1   0 | 3   0   5 |
        6 | 0   0   5 | 0   9   0 | 0   0   0 |
        -------------------------------------
        7 | 0   9   0 | 0   4   0 | 0   0   8 |
        8 | 0   0   3 | 5   0   0 | 0   6   0 |
        9 | 0   0   0 | 0   2   0 | 4   7   0 |
        -------------------------------------
Sudoku solved (in 0 secs ) by using sudoku techniques

        Last Cell: ( 1,6 ) =  2
        Explanation: Cell has only 1 choice

          | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
          -------------------------------------
        1 | 5   8   6 | 4   7   2*| 9   1   3 |
        2 | 3   2   4 | 9   5   1 | 6   8   7 |
        3 | 1   7   9 | 6   3   8 | 2   5   4 |
        -------------------------------------
        4 | 4   3   7 | 2   6   5 | 8   9   1 |
        5 | 9   6   2 | 8   1   7 | 3   4   5 |
        6 | 8   1   5 | 3   9   4 | 7   2   6 |
        -------------------------------------
        7 | 2   9   1 | 7   4   6 | 5   3   8 |
        8 | 7   4   3 | 5   8   9 | 1   6   2 |
        9 | 6   5   8 | 1   2   3 | 4   7   9 |
        -------------------------------------
```		
Or you can just run the exe and give sudoku file path manually. In this usage you can solve more than one sudoku puzzles,  consecutively; until you enter "exit".
At both usage; you will be asked if you want to use Sudoku techniques ( or just run recursive brute force algorithm ) and if you want to see each step of solution of given sudoku, that means everytime a new cell is solved or possibility list of a cell changed, sudoku will be printed and an explanation will be seen.


### Performance Mode
There is a performance mode of SudokuSolver. To enable performance mode, comment out "#undef  PERFORMANCE_MODE" at 5. line of  SudokuSolver.h header file.
You can analyze performance of sudoku solution techniques or calculate success rate of applied techniques.
When you choose "Peformance Test File", a file path will be asked. After you enter path of sudoku and repetition number, the sudoku puzzle will be solved "repetition" times by recursive brute force algorithm; then same puzzle will be solved by sudoku solution techniques ( again repetition times ) and will give you results. 

If you choose "Success Test File", program will try to solve sudoku puzzle with sudoku techniques, and it will print if sudoku solved completely, or it will print  cell count found by these techniques.

If you select "Performance Test Folder" or "Success Test Folder"; the processes above will be applied all files in given path.

For performance mode, I supplied a "PerformanceTestData" folder which have folders of sudokus, grouped by level ( these levels are determined by brute force algorithm solve duration)
Also, for success test, I supplied a zip file which contains 5 x 10.000 sudoku puzzles grouped by initial non-empty cell count. ( Due to file count makes hard to add git,  I zipped all folders in a zip file.)
If you want to run Success Test Folder, you can unzip any of these 5 folders to a path and give this path to Sudoku Solver.

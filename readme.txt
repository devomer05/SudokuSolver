	Sudoku Solver is a c++ project that tries to solve sudoku puzzles by using some sudoku solution techniques. 

- Introduction
	
	The commonly known recursive brute force solution to a puzzle may take long times if given hints are not enough. But if we can crop possibilities; this
duration can be reduced. At first I started the project by easiest technique which is "first put all certain numbers" then start to brute force solution.
After a few brain storm and ask myself "how i solve a sudoku", my code started to grow. Also I researched about these techniques and finalized the project.

- Solution
	
	To reduce possibilities at each cell, A possibility table used. This is an array of 9 possibilities per cell. After initializing this possibility array;
the sudoku solution techniques; naked single, hidden single, naked pair and hidden pair; processed on it. This process continued until these techniques can 
not find any new clue:

	while new clue found;
		-apply naked single, 
				( Cells that has only 1 possibility due to all other numbers show up at same row, column or box)
		-apply hidden single, 
				(  If a number shows up at only one cell as a possibility, then this cell must be this number. Ex: If Row is  2-9-5-(1/3/8)-(3/8)-7-(3/6/8)-(3/6)-(3/6);  then 4. cell is "1" )
		-apply  naked pair,
				( 2 Cells have same 2 possibilities, then it is obvious that these 2 numbers will go these 2 cells. So we can remove these numbers from possibility list of other cells)
				( Ex: If row is  2-9-5-(1/3/8)-(4/8)-7-(4/6/8)-(3/6)-(3/6) last 2 cells are obviously 3 and 6, so we can remove 3 and 6 from other cells :  2-9-5-(1/8)-(4/8)-7-(4/8)-(3/6)-(3/6)  )
		-apply hidden pair
				(  If  2numbers show up at only 2 cells as a possibility, then this cells must be this numbers.  Ex: If Row is  2-9-5-(1/3/8)-(4/3/8)-7-(1/4/6)-(4/6)-(1/4/6),  )
				( 3&8 only can go 3. and 4. cells. Then these cells must be 3 or 8. we can remove other possibilities here:  2-9-5-(3/8)-(3/8)-7-(1/4/6)-(4/6)-(1/4/6))

	After techniques applied, if Sudoku puzzle could not be solved completely, the recursive brute force algorithm takes charge. Due to new cells and hints
found, brute force takes less time compared to fully recursive brute force solution.

- Usage
	
	You can run exe with an argument which is a path of sudoku file as

			.\SudokuSolver.exe ".\sudokuFile.txt"
			
	Or you can just run the exe and give sudoku file path manually. In this usage you can solve more than one sudoku puzzles,  consecutively; until you enter "exit".

			.\SudokuSolver.exe
	
	At both usage; you will be asked if you want to use Sudoku techniques ( or just run recursive brute force algorithm ) and if you want to see each step of solution
of given sudoku, that means everytime a new cell is solved or possibility list of a cell changed, sudoku will be printed and an explanation will be seen.

	Also there is a performance mode of SudokuSolver.  You can analyze performance of sudoku solution techniques or calculate success rate of applied techniques.
When you choose "Peformance Test File", a file path will be asked. After you enter path of sudoku and repetition number, the sudoku puzzle will be solved "repetition"
times by recursive brute force algorithm; then same puzzle will be solved by sudoku solution techniques ( again repetition times ) and will give you results. 

	If you choose "Success Test File", program will try to solve sudoku puzzle with sudoku techniques, and it will print if sudoku solved completely, or it will print  cell
count found by these techniques.

	If you select "Performance Test Folder" or "Success Test Folder"; the processes above will be applied all files in given path.

	For performance mode, I supplied a "PerformanceTestData" folder which have folders of sudokus, grouped by level ( these levels are determined by brute force algorithm solve duration)
Also, for success test, I supplied a zip file which contains 5 x 10.000 sudoku puzzles grouped by initial non-empty cell count. ( Due to file count makes hard to add git,  I zipped all folders in a zip file.)
If you want to run Success Test Folder, you can unzip any of these 5 folders to a path and give this path to Sudoku Solver.
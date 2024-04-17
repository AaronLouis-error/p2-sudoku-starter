// Sudoku puzzle verifier and solver

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int **globalGrid; 
int globalPsize;



struct gridInfo {
    int** grid;  
    int psize;
    bool isComplete; // Pointer to a boolean indicating completeness
    bool isValid;    // Pointer to a boolean indicating validity     // Row number
};

//todo: create a struct that can store the &grid, bool isComplete, bool isVaild, row num
// & pass that as a parameter in here.
void* row(int rowNum){

  int array[globalPsize]; 
  for (int i = 0; i < globalPsize; i++){
    array[i] = -1;
  }

  bool isValid = true;
  int current;
  //printf("thread: %d\n", rowNum);
    for (int i = 1; i <= globalPsize && isValid; i++){
      current = globalGrid[rowNum + 1][i];
      if (current == 0){
        //todo: method to complete row
        //todo: complete row
        // only if I can't complete the row will I return false
        isValid = false; //technically is incomplete
      }
      if (array[current - 1]  != -1 && current != 0){
        // the value corresponding to current has already been set meaning 
        // this is a repeat
        printf("repeat: %d\n", current);
        isValid = false;
      }
      array[current - 1] = current;
    }
  bool* boolPtr = malloc(isValid); //initialize pointer
  *boolPtr = (isValid) ? true : false; //define pointer
  return boolPtr;
}

// This method creates number of threads equivilent to number of rows.
// The logic for testing the validity of a row is in row() which this method 
// calls. 
void spawnRowThreads(struct gridInfo* currentGrid){
  bool gridIsValid = true;
  bool* rowIsValid;
  pthread_t rowNum[currentGrid->psize];
  
  //need two for loops to run threads simultainiously 
  for(int i = 0; i < currentGrid->psize; i++){
    pthread_create(&rowNum[i], NULL, row, (void*)i);
  }

  for(int i = 0; i < currentGrid->psize; i++){
    pthread_join(rowNum[i], (void **)&rowIsValid);
    if (!*rowIsValid) { gridIsValid = false; }
  }

  if (gridIsValid){
    printf("valid grid\n");
    currentGrid->isValid = true;
    //return true;
  }else{
    printf("not valid grid\n");
    currentGrid->isValid = false;
    //return false; 
  }
}

// takes puzzle size and grid[][] representing sudoku puzzle
// and tow booleans to be assigned: complete and valid.
// row-0 and column-0 is ignored for convenience, so a 9x9 puzzle
// has grid[1][1] as the top-left element and grid[9]9] as bottom right
// A puzzle is complete if it can be completed with no 0s in it
// If complete, a puzzle is valid if all rows/columns/boxes have numbers from 1
// to psize For incomplete puzzles, we cannot say anything about validity
void checkPuzzle(struct gridInfo* currentGrid) {
  // YOUR CODE GOES HERE and in HELPER FUNCTIONS
  
  //*valid = (spawnRowThreads(&complete, &valid, &grid)) ? true : false ;
  spawnRowThreads(&currentGrid);
}

// takes filename and pointer to grid[][]
// returns size of Sudoku puzzle and fills grid
struct gridInfo* readSudokuPuzzle(char *filename, int ***grid) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }

  //struct gridInfo currentGrid
  int psize;
  fscanf(fp, "%d", &psize);
  printf("psize: %d\n", psize);
  globalPsize = psize;
  int **agrid = (int **)malloc((psize + 1) * sizeof(int *));
  for (int row = 1; row <= psize; row++) {
    agrid[row] = (int *)malloc((psize + 1) * sizeof(int));
    for (int col = 1; col <= psize; col++) {
      fscanf(fp, "%d", &agrid[row][col]);
    }
  }
  fclose(fp);
  *grid = agrid;
  globalGrid = *grid;

  //struct gridInfo currentGrid = malloc(gridInfo);
  struct gridInfo currentGrid = {agrid, psize, true, true};
  // todo: properly initialized by not going to next function with same info
  //maybe the problem is a pointer to a local variable
  //return psize;
  return &currentGrid;
}

// takes puzzle size and grid[][]
// prints the puzzle
void printSudokuPuzzle(int psize, int **grid) {
  printf("%d\n", psize);
  for (int row = 1; row <= psize; row++) {
    for (int col = 1; col <= psize; col++) {
      printf("%d ", grid[row][col]);
    }
    printf("\n");
  }
  printf("\n");
}

// takes puzzle size and grid[][]
// frees the memory allocated
void deleteSudokuPuzzle(int psize, int **grid) {
  for (int row = 1; row <= psize; row++) {
    free(grid[row]);
  }
  free(grid);
}

int runTests(){
  int **grid = NULL;
  // find grid size and fill grid
  
  char* puzzleNames[] = {"puzzle2-valid.txt", "puzzle2-fill-valid.txt", "puzzle2-invalid.txt", 
                        "puzzle9-valid.txt"};
  int numOfPuzzles = sizeof(puzzleNames) / sizeof(puzzleNames[0]);
  for (int i = 0; i < numOfPuzzles; i++){
    //printf("size: %d\n", numOfPuzzles);
    //printf("i: %d\n", i);
    struct gridInfo* myGrid;
    myGrid = readSudokuPuzzle(puzzleNames[i], &grid);
    bool valid = false;
    bool complete = false;
    checkPuzzle(myGrid);
    printf("Complete puzzle? ");
    printf(complete ? "true\n" : "false\n");
    if (complete) {
      printf("Valid puzzle? ");
      printf(valid ? "true\n" : "false\n");
    }
    printSudokuPuzzle(myGrid->psize, myGrid->grid);
    deleteSudokuPuzzle(myGrid->psize, myGrid->grid);
  }
  return EXIT_SUCCESS;
}

// expects file name of the puzzle as argument in command line
int main(int argc, char **argv) {
  if (argc != 2) {
    //printf("usage: ./sudoku puzzle.txt\n");
    printf("Running Tests\n");
    return runTests();
  }
  
  // grid is a 2D array
  int **grid = NULL;
  struct gridInfo* myGrid;
  myGrid = readSudokuPuzzle(argv[1], &grid);
  bool valid = false;
  bool complete = false;
  checkPuzzle(&myGrid);
  printf("Complete puzzle? ");
  printf(complete ? "true\n" : "false\n");
  if (complete) {
    printf("Valid puzzle? ");
    printf(valid ? "true\n" : "false\n");
  }
  printSudokuPuzzle(myGrid->psize, myGrid->grid);
  deleteSudokuPuzzle(myGrid->psize, myGrid->grid);
  return EXIT_SUCCESS;
}

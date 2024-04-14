// Sudoku puzzle verifier and solver

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void* row(int psize, int rowNum, int tid){
  printf("thread: %d\n", tid);
  sleep(2);
  return NULL;
}

// takes puzzle size and grid[][] representing sudoku puzzle
// and tow booleans to be assigned: complete and valid.
// row-0 and column-0 is ignored for convenience, so a 9x9 puzzle
// has grid[1][1] as the top-left element and grid[9]9] as bottom right
// A puzzle is complete if it can be completed with no 0s in it
// If complete, a puzzle is valid if all rows/columns/boxes have numbers from 1
// to psize For incomplete puzzles, we cannot say anything about validity
void checkPuzzle(int psize, int **grid, bool *complete, bool *valid) {
  // YOUR CODE GOES HERE and in HELPER FUNCTIONS
  //*valid = true;
  //*complete = true;
  pthread_t t1, t2, t3; 
  pthread_create(&t1, NULL, row, NULL);
  pthread_create(&t2, NULL, row, NULL);
  pthread_create(&t3, NULL, row, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);

}

// takes filename and pointer to grid[][]
// returns size of Sudoku puzzle and fills grid
int readSudokuPuzzle(char *filename, int ***grid) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }
  int psize;
  fscanf(fp, "%d", &psize);
  int **agrid = (int **)malloc((psize + 1) * sizeof(int *));
  for (int row = 1; row <= psize; row++) {
    agrid[row] = (int *)malloc((psize + 1) * sizeof(int));
    for (int col = 1; col <= psize; col++) {
      fscanf(fp, "%d", &agrid[row][col]);
    }
  }
  fclose(fp);
  *grid = agrid;
  return psize;
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
  
  char* puzzleNames[] = {"puzzle2-fill-valid.txt", "puzzle2-invalid.txt", 
                        "puzzle2-valid.txt", "puzzle9-valid.txt"};
  int numOfPuzzles = sizeof(puzzleNames) / sizeof(puzzleNames[0]);
  for (int i = 0; i < numOfPuzzles; i++){
    //printf("size: %d\n", numOfPuzzles);
    //printf("i: %d\n", i);
    int sudokuSize = readSudokuPuzzle(puzzleNames[i], &grid);
    bool valid = false;
    bool complete = false;
    checkPuzzle(sudokuSize, grid, &complete, &valid);
    printf("Complete puzzle? ");
    printf(complete ? "true\n" : "false\n");
    if (complete) {
      printf("Valid puzzle? ");
      printf(valid ? "true\n" : "false\n");
    }
    printSudokuPuzzle(sudokuSize, grid);
    deleteSudokuPuzzle(sudokuSize, grid);
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
  // find grid size and fill grid
  int sudokuSize = readSudokuPuzzle(argv[1], &grid);
  bool valid = false;
  bool complete = false;
  checkPuzzle(sudokuSize, grid, &complete, &valid);
  printf("Complete puzzle? ");
  printf(complete ? "true\n" : "false\n");
  if (complete) {
    printf("Valid puzzle? ");
    printf(valid ? "true\n" : "false\n");
  }
  printSudokuPuzzle(sudokuSize, grid);
  deleteSudokuPuzzle(sudokuSize, grid);
  return EXIT_SUCCESS;
}

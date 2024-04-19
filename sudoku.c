// Sudoku puzzle verifier and solver

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>



struct gridInfo {
    int** grid;
    int psize;
    bool isComplete; 
    bool isValid;    
    int index;
};


double customSqrt(double x) {
    double guess = x / 2.0; // Initial guess
    double epsilon = 1e-3; // Tolerance for convergence
    
    while (1) {
        double newGuess = 0.5 * (guess + x / guess); // Newton-Raphson formula
        if (fabs(newGuess - guess) < epsilon) {
            return newGuess; // Converged to desired accuracy
        }
        guess = newGuess;
    }
}


void* row(struct gridInfo* myGrid){

  int array[myGrid->psize]; 
  for (int i = 0; i < myGrid->psize; i++){
    array[i] = -1;
  }

  bool isValid = true;
  int current;
  //printf("thread: %d\n", rowNum);
    for (int i = 1; i <= myGrid->psize && myGrid->isValid; i++){
      current = myGrid->grid[i][myGrid->index]; //todo: validate that this checks horizontal repeats
      if (current == 0){
        //todo: method to complete row
        //todo: complete row
        // only if I can't complete the row will I return false
        //printf("zero detected\n");
        myGrid->isComplete = false;
        isValid = false; //technically is incomplete
      }
      if (array[current - 1]  != -1 && current != 0){
        // the value corresponding to current has already been set meaning 
        // this is a repeat
        //printf("repeat: %d\n", current);
        myGrid->isValid = false;
        isValid = false;
      }
      array[current - 1] = current;
    }
  bool* boolPtr = malloc(isValid); //initialize pointer
  *boolPtr = (isValid) ? true : false; //define pointer
  return boolPtr;
}

void* column(struct gridInfo* myGrid){

  int array[myGrid->psize]; 
  for (int i = 0; i < myGrid->psize; i++){
    array[i] = -1;
  }

  bool isValid = true;
  int current;
  
    //for (int i = 1; i <= myGrid->psize && myGrid->isValid; i++){
    for (int i = 1; i <= myGrid->psize; i++){
      current = myGrid->grid[myGrid->index][i];
      if (current == 0){
        //todo: method to complete row
        //todo: complete row
        // only if I can't complete the row will I return false
        //printf("zero detected in column%d\n", i);
        myGrid->isComplete = false;
        isValid = false; //technically is incomplete
      }
      if (array[current - 1]  != -1 && current != 0){
        // the value corresponding to current has already been set meaning 
        // this is a repeat
        //printf("repeat: %d in column %d\n", current, i);
        myGrid->isValid = false;
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
    currentGrid->index = i;
    pthread_create(&rowNum[i], NULL, row, (void*)currentGrid);
  }

  for(int i = 0; i < currentGrid->psize; i++){
    pthread_join(rowNum[i], (void **)&rowIsValid);
    if (!*rowIsValid) { currentGrid->isValid = false; }
  }

}

void spawnColumnThreads(struct gridInfo* currentGrid){
  bool gridIsValid = true;
  bool* columnIsValid;
  pthread_t columnNum[currentGrid->psize];
  
  //need two for loops to run threads simultainiously 
  for(int i = 0; i < currentGrid->psize; i++){
    currentGrid->index = i;
    pthread_create(&columnNum[i], NULL, column, (void*)currentGrid);
  }

  for(int i = 0; i < currentGrid->psize; i++){
    pthread_join(columnNum[i], (void **)&columnIsValid);
    if (!*columnIsValid) { currentGrid->isValid = false; }
  }

}

void* quadrant(struct gridInfo* myGrid){

  bool isValid = true;
  int index = myGrid->index;
  double lengthOfQuadrant = customSqrt((double) myGrid->psize);
  //printf("length: %d  ", (int)lengthOfQuadrant);
  int quadRowBottom = 5;
  int increment = 0; 
  while (increment <= lengthOfQuadrant){

    if (index * increment < myGrid->psize){
      quadRowBottom = increment;
      if(increment == 1 && index > 3){
        printf("\t index: %d increment %d\n", index, increment);
        printf("\t mul: %d, tot: %d\n", (index * increment), myGrid->psize);
      }
      //break;
    } else {
      //printf("\t index %d increment %d \n", index, increment);
    }
    increment++;
  }
  printf("quadrowbottow: %d\n", quadRowBottom);
  //TOdo: work on finding correct values

  // int array[myGrid->psize]; 
  // for (int i = 0; i < myGrid->psize; i++){
  //   array[i] = -1;
  // }

  // bool isValid = true;
  // int current;
  
  //   //for (int i = 1; i <= myGrid->psize && myGrid->isValid; i++){
  //   for (int i = 1; i <= myGrid->psize; i++){
  //     current = myGrid->grid[myGrid->index][i];
  //     if (current == 0){
  //       //todo: method to complete row
  //       //todo: complete row
  //       // only if I can't complete the row will I return false
  //       //printf("zero detected in column%d\n", i);
  //       myGrid->isComplete = false;
  //       isValid = false; //technically is incomplete
  //     }
  //     if (array[current - 1]  != -1 && current != 0){
  //       // the value corresponding to current has already been set meaning 
  //       // this is a repeat
  //       //printf("repeat: %d in column %d\n", current, i);
  //       myGrid->isValid = false;
  //       isValid = false;
  //     }
  //     array[current - 1] = current;
  //   }
  // bool* boolPtr = malloc(isValid); //initialize pointer
  // *boolPtr = (isValid) ? true : false; //define pointer
  // return boolPtr;
  bool* boolPtr = malloc(isValid); //initialize pointer
  *boolPtr = (isValid) ? true : false; //define pointer
  return boolPtr;
}

void spawnQuadrantThreads(struct gridInfo* currentGrid){
  //psize will be equal to the number of quadrants
  //sqrt(psize) = size of quadrant
  // need to generate row lower and upper, column lower and upper from quadrant num
  //maybe just read quadrant into an array?
  bool gridIsValid = true;
  bool* quadrantIsValid;
  
  //double numQuadrants = customSqrt((double) currentGrid->psize);
  int numQuadrants = currentGrid->psize;
  //double a = 4.0;
  //double b = sqrt(a);
  pthread_t quadrantNum[(int) numQuadrants];

  //need two for loops to run threads simultainiously 
  for(int i = 0; i < numQuadrants; i++){
    currentGrid->index = i;
    pthread_create(&quadrantNum[i], NULL, quadrant, (void*)currentGrid);
    pthread_join(quadrantNum[i], (void **)&quadrantIsValid);
    if (!*quadrantIsValid) { currentGrid->isValid = false; }
  }

  // for(int i = 0; i < currentGrid->psize; i++){
  //   pthread_join(quadrantNum[i], (void **)&quadrantIsValid);
  //   if (!*quadrantIsValid) { currentGrid->isValid = false; }
  // }

}

// takes puzzle size and grid[][] representing sudoku puzzle
// and tow booleans to be assigned: complete and valid.
// row-0 and column-0 is ignored for convenience, so a 9x9 puzzle
// has grid[1][1] as the top-left element and grid[9]9] as bottom right
// A puzzle is complete if it can be completed with no 0s in it
// If complete, a puzzle is valid if all rows/columns/boxes have numbers from 1
// to psize For incomplete puzzles, we cannot say anything about validity
void checkPuzzle(struct gridInfo* currentGrid) {
  //spawnRowThreads(currentGrid);
  //spawnColumnThreads(currentGrid);
  spawnQuadrantThreads(currentGrid);
}

// takes filename and pointer to grid[][]
// returns size of Sudoku puzzle and fills grid
void readSudokuPuzzle(char *filename, struct gridInfo* myGrid) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }

  fscanf(fp, "%d", &myGrid->psize);
  int **agrid = (int **)malloc((myGrid->psize + 1) * sizeof(int *));
  for (int row = 1; row <= myGrid->psize; row++) {
    agrid[row] = (int *)malloc((myGrid->psize + 1) * sizeof(int));
    for (int col = 1; col <= myGrid->psize; col++) {
      fscanf(fp, "%d", &agrid[row][col]);
    }
  }

  fclose(fp);
  myGrid->grid = agrid;
  myGrid->isComplete = true;
  myGrid->isValid = true;
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
  //todo: free the struct
}

int runTests(){
  int **grid = NULL;
  struct gridInfo* myGrid = malloc(sizeof(struct gridInfo));
  //myGrid->psize = malloc(sizeof(int));
  // find grid size and fill grid
  
  char* puzzleNames[] = {"puzzle2-valid.txt", "puzzle2-fill-valid.txt", "puzzle2-invalid.txt", 
                        "puzzle9-valid.txt", "vertical-repeat-puzzle.txt", "edgeCase-vertical-repeat.txt"};
  int numOfPuzzles = sizeof(puzzleNames) / sizeof(puzzleNames[0]);
  for (int i = 0; i < numOfPuzzles; i++){
    readSudokuPuzzle(puzzleNames[i], myGrid);
    checkPuzzle(myGrid);
    printf("Complete puzzle? ");
    printf(myGrid->isComplete ? "true\n" : "false\n");
    if (myGrid->isComplete) {
      printf("Valid puzzle? ");
      printf(myGrid->isValid ? "true\n" : "false\n");
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
  
  //todo: update this to look like runtests
  // grid is a 2D array
  int **grid = NULL;
  //struct gridInfo* myGrid;
  struct gridInfo* myGrid = malloc(sizeof(struct gridInfo));
  readSudokuPuzzle(argv[1], &myGrid);
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

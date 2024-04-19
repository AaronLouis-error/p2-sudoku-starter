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
    int indexTwo;
    int sqrt;

};

struct coordinate {
  int x;
  int y;
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
    currentGrid->index = i + 1; //does this cause synchronization problem?
    pthread_create(&columnNum[i], NULL, column, (void*)currentGrid);
  }

  for(int i = 0; i < currentGrid->psize; i++){
    pthread_join(columnNum[i], (void **)&columnIsValid);
    if (!*columnIsValid) { currentGrid->isValid = false; }
  }

}

//maybe an array of pointer to pass in a struct and coordinates
void* quadrant(struct gridInfo* myGrid){

  bool isValid = true;
  int current;

  int array[myGrid->psize]; 
  for (int i = 0; i < myGrid->psize; i++){
    array[i] = -1;
  }

  for(int i = myGrid->index; i < myGrid->index + myGrid->sqrt; i++){
    for(int j = myGrid->indexTwo; j < myGrid->indexTwo + myGrid->sqrt; j++){
      //printf("i:%d,j:%d\n",i,j);
      //printf("%d ", myGrid->grid[i][j]);
      current = myGrid->grid[i][j];
      if (array[current - 1]  != -1 && current != 0){
        // the value corresponding to current has already been set meaning 
        // this is a repeat
        //printf("\trepeat: %d in quadrant %d,%d\n", current, i,j);
        myGrid->isValid = false;
        isValid = false;
      }
      array[current - 1] = current;
    }
  }
  
  
  bool* boolPtr = malloc(isValid); //initialize pointer
  *boolPtr = (isValid) ? true : false; //define pointer
  return boolPtr;
}

void spawnQuadrantThreads(struct gridInfo* currentGrid){
  
  bool gridIsValid = true;
  bool* quadrantIsValid;
  int numQuadrants = currentGrid->psize;
  int sqrt = (int) customSqrt(currentGrid->psize);
  currentGrid->sqrt = sqrt;
  pthread_t quadrantNum[(int) numQuadrants];

  int threadIdindex = 0;
  for(int i = 1; i <= currentGrid->psize; i = i + sqrt){
    for(int j = 1; j <= currentGrid->psize; j = j + sqrt){
      //printf("coordinates: %d,%d\n", i, j);
      currentGrid->index = i;
      currentGrid->indexTwo = j;
      pthread_create(&quadrantNum[threadIdindex], NULL, quadrant, (void*)currentGrid);
      threadIdindex++;
    }
  }

  for (int i = 0; i < currentGrid->psize; i++){
    pthread_join(quadrantNum[i ], (void **)&quadrantIsValid);
      if (!*quadrantIsValid) { currentGrid->isValid = false; }
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
  spawnRowThreads(currentGrid);
  spawnColumnThreads(currentGrid);
  spawnQuadrantThreads(currentGrid);
  if (!currentGrid->isComplete){
    complete(currentGrid);
  }
}

//create two threads one incrementing and one decrementing
void complete(struct gridInfo* currentGrid){
  printf("pee\n");
  int arrayRow[currentGrid->psize];
  int arrayColumn[currentGrid->psize];
  int arrayBox[currentGrid->psize];

  for(int a = 1; a <= currentGrid->psize; a++){
    for(int b = 1; b <= currentGrid->psize; b++){
      if (currentGrid->grid[a][b] == 0){
        printf("zero at %d,%d\n", a, b);
        int coordinateX = a;
        int coordinateY = b;
        //run array creation here
        possibleRowVals(currentGrid, coordinateX, coordinateY, arrayRow);
        possibleColumnVals(currentGrid, coordinateX, coordinateY, arrayColumn);
        
          // printf("\tValid options for %d,%d:  ",coordinateX, coordinateY);
          // for (int i = 0; i < currentGrid->psize; i++){
          //   printf("%d, ", arrayColumn[i]); 
          // }
          // printf("\n");
        
        for(int i = 0; i < currentGrid->psize && arrayRow[i] != 0; i++){
          for(int j = 0; j < currentGrid->psize && arrayColumn[j] != 0; j++){
            for (int k = 0; k < currentGrid->psize && arrayBox[k] != 0; k++){
              if (arrayRow[i] == arrayColumn[j] && arrayColumn[j] == arrayBox[k]){
                currentGrid->grid[coordinateX][coordinateY] = arrayRow[i];
              }
            }
          }
        }
      }
    }
  }
}


void possibleColumnVals(struct gridInfo* myGrid, int CoordinateX, int CoordinateY, int *array){
  int invalidArray[myGrid->psize];
  for (int i = 0; i < myGrid->psize; i++){
    array[i] = 0;
    invalidArray[i] = 0;
  }

  for(int i = 0; i < myGrid->psize; i++){
    int current = myGrid->grid[i + 1][CoordinateY];
    if (current != 0){invalidArray[current - 1] = current;} //store invalid values
  }

  int counter = 0;
  for(int i = 0; i < myGrid->psize; i++){
    if(invalidArray[i] == 0){
      array[counter] = i + 1;
      counter++;
    }
  }

  // printf("\tinvalid array: \t\t");
  // for(int i = 0; i < myGrid->psize; i++){
  //     printf("%d, ", invalidArray[i]);    
  // }
  // printf("\n");

  // printf("\tValid options for %d,%d:  ",CoordinateX, CoordinateY);
  // for (int i = 0; i < myGrid->psize; i++){
  //   printf("%d, ", array[i]); 
  // }
  // printf("\n");
}

void possibleRowVals(struct gridInfo* myGrid, int CoordinateX, int CoordinateY, int *array){
  int invalidArray[myGrid->psize];
  for (int i = 0; i < myGrid->psize; i++){
    array[i] = 0;
    invalidArray[i] = 0;
  }

  for(int i = 0; i < myGrid->psize; i++){
    int current = myGrid->grid[CoordinateX][i + 1];
    if (current != 0){invalidArray[current - 1] = current;} //store invalid values
  }

  int counter = 0;
  for(int i = 0; i < myGrid->psize; i++){
    if(invalidArray[i] == 0){
      array[counter] = i + 1;
      counter++;
    }
  }

  // printf("\tinvalid array: \t\t");
  // for(int i = 0; i < myGrid->psize; i++){
  //     printf("%d, ", invalidArray[i]);    
  // }
  // printf("\n");

  // printf("\tValid options for %d,%d:  ",CoordinateX, CoordinateY);
  // for (int i = 0; i < myGrid->psize; i++){
  //   printf("%d, ", array[i]); 
  // }
  // printf("\n");
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
  
  char* puzzleNames[] = {"puzzle2-valid.txt", "puzzle2-fill-valid.txt", 
                        "puzzle2-invalid.txt", "puzzle9-valid.txt",
                        "vertical-repeat-puzzle.txt", 
                        "edgeCase-vertical-repeat.txt",
                        "puzzle9-invalid-diagRepeat.txt"};
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

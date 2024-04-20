// Sudoku puzzle verifier and solver
//Aaron Gropper

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//holds key information about the sudoku puzzle
struct gridInfo {
    int** grid;
    int psize;
    bool isComplete; 
    bool isValid;    
    int index;
    int indexTwo;
    int sqrt;
    char* fileName;

};

struct completionInfo{
  struct gridInfo* myGrid;
  int CoordinateX;
  int CoordinateY;
};

//returns square root
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

//validates the row
void* row(struct gridInfo* myGrid){

  int array[myGrid->psize]; 
  for (int i = 0; i < myGrid->psize; i++){
    array[i] = -1;
  }
  bool isValid = true;
  int current;
    for (int i = 1; i <= myGrid->psize && myGrid->isValid; i++){
      current = myGrid->grid[i][myGrid->index]; //todo: validate that this checks horizontal repeats
      if (current == 0){
        myGrid->isComplete = false;
        isValid = false; //technically is incomplete
      }
      if (array[current - 1]  != -1 && current != 0){
        // the value corresponding to current has already been set meaning 
        // this is a repeat
        myGrid->isValid = false;
        isValid = false;
      }
      array[current - 1] = current;
    }
  bool* boolPtr = malloc(isValid); //initialize pointer
  *boolPtr = (isValid) ? true : false; //define pointer
  return boolPtr;
}

//validates the column
void* column(struct gridInfo* myGrid){
  int array[myGrid->psize]; 
  for (int i = 0; i < myGrid->psize; i++){
    array[i] = -1;
  }

  bool isValid = true;
  int current;
  for (int i = 1; i <= myGrid->psize; i++){
    current = myGrid->grid[myGrid->index][i];
    if (current == 0){
      myGrid->isComplete = false;
      isValid = false; //technically is incomplete
    }
    if (array[current - 1]  != -1 && current != 0){
      // the value corresponding to current has already been set meaning 
        // this is a repeat
      myGrid->isValid = false;
      isValid = false;
    }
    array[current - 1] = current;
  }
  bool* boolPtr = malloc(isValid); //initialize pointer
  *boolPtr = (isValid) ? true : false; //define pointer
  return boolPtr;
}

//validates a box in the puzzle
void* quadrant(struct gridInfo* myGrid){

  bool isValid = true;
  int current;

  int array[myGrid->psize]; 
  for (int i = 0; i < myGrid->psize; i++){
    array[i] = -1;
  }

  for(int i = myGrid->index; i < myGrid->index + myGrid->sqrt; i++){
    for(int j = myGrid->indexTwo; j < myGrid->indexTwo + myGrid->sqrt; j++){
      current = myGrid->grid[i][j];
      if (array[current - 1]  != -1 && current != 0){
        // the value corresponding to current has already been set meaning 
        // this is a repeat
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
    if (!*rowIsValid) { 
      currentGrid->isValid = false; }
  }

}

// This method creates number of threads equivilent to numberof columns
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

// This method creates number of threads equivilent to number boxes
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
      currentGrid->index = i;
      currentGrid->indexTwo = j;
      pthread_create(&quadrantNum[threadIdindex], NULL, quadrant, (void*)currentGrid);
      threadIdindex++;
    }
  }

  for (int i = 0; i < currentGrid->psize; i++){
    pthread_join(quadrantNum[i ], (void **)&quadrantIsValid);
      if (!*quadrantIsValid) { 
        currentGrid->isValid = false; }
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

//returns true if the puzzle has no zeros
bool isComplete(struct gridInfo* myGrid){
  for(int i = 1; i <= myGrid->psize; i++){
    for(int j = 1; j <= myGrid->psize; j++){
      if (myGrid->grid[i][j] == 0){
        return false;
      }
    }
  }
  return true;
}

//generates possible values to fill a specific Row
void * possibleRowVals(struct completionInfo * info){

  int *array = (int *)malloc(sizeof(int) * info->myGrid->psize);
  int invalidArray[info->myGrid->psize];
  for (int i = 0; i < info->myGrid->psize; i++){
    array[i] = 0;
    invalidArray[i] = 0;
  }

  for(int i = 0; i < info->myGrid->psize; i++){
    int current = info->myGrid->grid[info->CoordinateX][i + 1];
    if (current != 0){invalidArray[current - 1] = current;} //store invalid values
  }

  int counter = 0;
  for(int i = 0; i < info->myGrid->psize; i++){
    if(invalidArray[i] == 0){
      array[counter] = i + 1;
      counter++;
    }
  }

  return (void *)array;
}

//generates possible values to fill a specific Row
void * possibleColumnVals(struct completionInfo * info){

  int *array = (int *)malloc(sizeof(int) * info->myGrid->psize);
  int a = info->myGrid->psize;
  int invalidArray[info->myGrid->psize];
  for (int i = 0; i < info->myGrid->psize; i++){
    array[i] = 0;
    invalidArray[i] = 0;
  }

  for(int i = 0; i < info->myGrid->psize; i++){
    int current = info->myGrid->grid[i + 1][info->CoordinateY];
    if (current != 0){invalidArray[current - 1] = current;} //store invalid values
  }

  int counter = 0;
  for(int i = 0; i < info->myGrid->psize; i++){
    if(invalidArray[i] == 0){
      array[counter] = i + 1;
      counter++;
    }
  }

  return (void *)array;
}

//attempts to complete the puzzle using multithreading
void complete(struct gridInfo* currentGrid){
  int *arrayRow; 
  int *arrayColumn;
  int arrayBox[currentGrid->psize];

  struct completionInfo* info = malloc(sizeof(struct completionInfo));
  
  int numThreads = 3;
  pthread_t threadNum[(int) numThreads];
  
    for(int a = 1; a <= currentGrid->psize; a++){
      for(int b = 1; b <= currentGrid->psize; b++){
        if (currentGrid->grid[a][b] == 0){
          int coordinateX = a;
          int coordinateY = b;

          info->CoordinateX = coordinateX;
          info->CoordinateY = coordinateY;
          info->myGrid = currentGrid;
         
          pthread_create(&threadNum[0], NULL, possibleRowVals, info);
          pthread_create(&threadNum[1], NULL, possibleColumnVals, info);
          pthread_join(threadNum[0], (void *) &arrayRow);
          pthread_join(threadNum[1], (void *) &arrayColumn);
          possibleBoxVals(currentGrid, coordinateX, coordinateY, arrayBox);
          for(int i = 0; i < currentGrid->psize && arrayRow[i] != 0; i++){
            for(int j = 0; j < currentGrid->psize && arrayColumn[j] != 0; j++){
              for (int k = 0; k < currentGrid->psize && arrayBox[k] != 0; k++){
                if (arrayRow[i] == arrayColumn[j] && arrayColumn[j] == arrayBox[k]){
                  if (arrayRow[i] != 0){
                    currentGrid->grid[coordinateX][coordinateY] = arrayRow[i];
                  }
                }
              }
            }
          }
        }
      }
    }
    if(isComplete(currentGrid)){
      currentGrid->isComplete = true;
      currentGrid->isValid = true;
      checkPuzzle(currentGrid);
    }
}

//generates possible values to fill a specific box
void possibleBoxVals(struct gridInfo* myGrid, int CoordinateX, int CoordinateY, int *array){
  int invalidArray[myGrid->psize];
  for (int i = 0; i < myGrid->psize; i++){
    array[i] = 0;
    invalidArray[i] = 0;
  }

  int sqrt = (int) customSqrt(myGrid->psize);
  int xStartCoord;
  int yStartCoord;
  for(int i = 1; i <= myGrid->psize; i = i + sqrt){
    for(int j = 1; j <= myGrid->psize; j = j + sqrt){
      if (i <= CoordinateX){ xStartCoord = i;}
      if (j <= CoordinateY){ yStartCoord = j;}
    }
  }

  int a = xStartCoord;// to fix strange bug where xStartCoord randomly changes
  int b = yStartCoord;
  for(int i = a; i < a + sqrt; i++){
    for(int j = b; j < b + sqrt; j++){
      int current = myGrid->grid[i][j];
      if (current != 0){invalidArray[current - 1] = current;} //store invalid values
    }
  }

  int counter = 0;
  for(int i = 0; i < myGrid->psize; i++){
    if(invalidArray[i] == 0){
      array[counter] = i + 1;
      counter++;
    }
  }
}

// takes filename and pointer to grid[][]
// returns size of Sudoku puzzle and fills grid
void readSudokuPuzzle(char *filename, struct gridInfo* myGrid) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }
  myGrid->fileName = filename;

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
}

//runs tests using puzzles
int runTests(){
  int **grid = NULL;
  struct gridInfo* myGrid = malloc(sizeof(struct gridInfo));
  char* puzzleNames[] = {"easy-complete.txt", "edgeCase-vertical-repeat.txt", 
                        "puzzle2-fill-valid.txt", "puzzle2-invalid.txt",
                        "puzzle2-valid.txt", "puzzle4-fill-hard.txt",
                        "puzzle9-invalid-diagRepeat.txt", 
                        "puzzle9-valid.txt"};
  int numOfPuzzles = sizeof(puzzleNames) / sizeof(puzzleNames[0]);
  for (int i = 0; i < numOfPuzzles; i++){
    readSudokuPuzzle(puzzleNames[i], myGrid);
    checkPuzzle(myGrid);
    printf("\t%s\n", myGrid->fileName);
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
    printf("Running Tests\n");
    return runTests();
  }
  
  int **grid = NULL;
  struct gridInfo* myGrid = malloc(sizeof(struct gridInfo));
  readSudokuPuzzle(argv[1], myGrid);
  checkPuzzle(myGrid);
  printf("\t%s\n", myGrid->fileName);
  printf("Complete puzzle? ");
  printf(myGrid->isComplete ? "true\n" : "false\n");
  if (myGrid->isComplete) {
    printf("Valid puzzle? ");
    printf(myGrid->isValid ? "true\n" : "false\n");
  }
  printSudokuPuzzle(myGrid->psize, myGrid->grid);
  deleteSudokuPuzzle(myGrid->psize, myGrid->grid);
  return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREADS 27
#define NUM 9

// Update the array to 1 if each thread find its section to be valid
int valid[NUM_THREADS] = {
    0
};

// Sudoku puzzle
int sudoku [9][9];

// Struct to store the data to be passed to threads
typedef struct {
		int row;
		int column;		
} parameters;

// Method to check the validity of a column
void *checkColumns(void* param) {
    // Confirm that parameters indicate a valid col subsection
	parameters *params = (parameters*) param;
	int col = params->column;
	int validArray[10] = {0};
	int i;	
	for (i = 0; i < NUM; i++) {
		int val = sudoku[i][col];
		if(validArray[val] != 0)
			pthread_exit(NULL);
		else
			validArray[val] = 1;
	}

    // If reached this point, col subsection is valid.
	valid[18 + col] = 1;
	pthread_exit(NULL);
}

// Method to check the validity of a row
void *checkRows(void* param) {
    // Confirm that parameters indicate a valid row subsection
	parameters *params = (parameters*) param;
	int row = params->row;
	int validArray[10] = {0};
	int i;
	for (i = 0; i < NUM; i++) {
		int val = sudoku[row][i];
		if(validArray[val] != 0)
			pthread_exit(NULL);
		else
			validArray[val] = 1;
	}

    // If reached this point, row subsection is valid.
	valid[9 + row] = 1;
	pthread_exit(NULL);
}

// Method to check 3x3 subsection is valid
void *check3A3(void* param) {
    // Confirm that parameters indicate a valid 3x3 subsection
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;

	int validArray[10] = {0};
	int i, j;
	for (i = row; i < row + 3; i++) {
		for (j = col; j < col + 3; j++) {
			int val = sudoku[i][j];
			if(validArray[val] != 0)
				pthread_exit(NULL);
			else
				validArray[val] = 1;
		}
	}

    // If reached this point, 3x3 subsection is valid.
	valid[row + col/3] = 1; 
	pthread_exit(NULL);
}

// Main or driver function
int main(int argc, char* argv[]) {
	char fileName[256];
	printf("\nEnter the filename to be opened: ");
    // Read the file name
	scanf("%[^\n]%*c", fileName);
    // Open the file in a read mode
	FILE* file = fopen(fileName, "r");
	char line[256];
	char delim[] = ",";
	size_t row = 0, count = 0;
    // Store the input in 2D array
	while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, delim);
        while(token != NULL){
            if((count != 0) && (count % 9 == 0)) {
                row++;
            }
            
            sudoku[row][(count % 9)] = atoi(token);
            count++;
            token = strtok(NULL, delim);
        }
	}
    // Close the file
	fclose(file);

	pthread_t threads[NUM_THREADS];
	int threadIndex = 0;	
	int i,j;

	// Row threads, 9 of them
	for(count=0;count<NUM;count++){
		parameters *data = (parameters *) malloc(sizeof(parameters));
		data->row = count;
		data->column = 0;
		pthread_create(&threads[threadIndex++], NULL, checkRows, data); 
	}
	// Column threads, 9 of them 
	for(count=0; count<NUM; count++){
		parameters *data = (parameters *) malloc(sizeof(parameters));
		data->row = 0;
		data->column = count;
		pthread_create(&threads[threadIndex++], NULL, checkColumns, data);

	}
	// 3x3 threads, 9 of them 
	for(count=0; count<NUM;count++){
		for(j=0; j<NUM;j++){
			if(count%3 == 0 && j%3 ==0){
				parameters *data = (parameters *) malloc(sizeof(parameters));
				data->row = count;
				data->column = j;
				pthread_create(&threads[threadIndex++], NULL, check3A3, data); 
			}
		}
	}

    // Wait for all threads to finish
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

    // If any of the entries in the array are 0, then the sudoku solution is invalid
	for (i = 0; i < NUM_THREADS; i++) {
		if (valid[i] == 0) {
			printf("The Sudoku solution is not valid!\n\n");
			return 0;
		}
	}
    // Otherwise given sudoku is valid
	printf("The Sudoku solution is valid!\n\n");
	return 0;
}



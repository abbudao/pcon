#include <stdio.h>
#include <stdlib.h>
#define SUCCESS 1
#define ERROR 0

void subtract(float** cols, float *multipliers_array, int myrank, int pivot_column,int pivot_line, int col_count, int col_size);
float* getMultipliers(float **cols,int col_size,int pivot_line,int pivot_column);
void changeRows(float **cols, int pivot_line, int target_index, int pivot_column, int col_count);
int findPivot(float **cols, int col_size, int col, int *index);
int findPivot(float **cols, int col_size, int col, int *index){
  /*
     @param cols: the collums of the process
     @param col_size: number of lines
     @param col: index of the collum
     @param index: reference of the index of the pivot
     Find the first non zero element and return it's index
     or return -1 if all elements are zero
     */
  int row;

  for (row = 0; row < col_size; row++){
    if(cols[row][col] != 0){
      *index = row;
      return SUCCESS;
    }

  }
  return ERROR; 
}


void changeRows(float **cols, int pivot_line, int target_index, int pivot_column, int col_count){
  //Cols eh uma matriz que contem as colunas recebidas pelo slave
  int i;
  float aux;
  for(i = pivot_column;i < col_count;i++){
    aux = cols[target_index][i];
    cols[target_index][i] = cols[pivot_line][i];
    cols[pivot_line][i] = aux;
  }
}

float* getMultipliers(float **cols,int col_size,int pivot_line,int pivot_column){
  int pivot,i;
  float *multipliers_array;
  multipliers_array=(float*) malloc(sizeof(float)*col_size);
  pivot= cols[pivot_line][pivot_column];
  for(i=0;i<col_size;i++){
    multipliers_array[i]= cols[i][pivot_column]/pivot;
  }
  multipliers_array[pivot_line]=pivot;
  return multipliers_array;
}
void subtract(float** cols, float *multipliers_array, int myrank, int pivot_column,int pivot_line, int col_count, int col_size){
  int i, k;
  float aux;
  for(i=pivot_column;i<col_count;i++){
    for(k=0;k<col_size;k++){
      if(k==pivot_line){
        cols[k][i] = cols[k][i]/multipliers_array[k];
      }
      else{
        aux = cols[pivot_line][i]*multipliers_array[k];
        cols[k][i] = cols[k][i] - aux;
      }
    }

  }
  return;
}

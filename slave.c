#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include "slave_func.c"
#define SUCCESS 1
#define ERROR 0

int main(int argc, char *argv[])
{
  int i,my_rank,num_proc,pivot_line,token,target_line;
  int pivot_column=0,col_count,col_size,IS_LAST_PROC=0;
  int pivot_found;
  float *multipliers_array;
  float **cols;
  MPI_Comm        inter_comm; 
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
  MPI_Comm_get_parent(&inter_comm);
  MPI_Datatype column_type;

  printf("Olar");
  /*Receive matrix order from Master*/
  MPI_Bcast(&col_size,1,MPI_INT,MPI_ROOT,inter_comm);
  /*Receive number of columns from Master */
  MPI_Scatter(NULL,1,MPI_INT,&col_count,1,MPI_INT,0,inter_comm);
  printf("Col count do processo %d %d \n",my_rank,col_count);
  /*Allocate memory chunk for array of columns */
  cols = (float **) malloc(sizeof(float *)*col_count);
  for (i = 0; i < col_count; i++) {
    cols[i] = (float *) malloc(sizeof(float)*col_size);
  }
  multipliers_array=(float*) malloc(sizeof(float)*col_size);
  /*Receive array of columns from Master */
  MPI_Type_contiguous(col_size,MPI_FLOAT,&column_type);
  MPI_Type_commit(&column_type);
  MPI_Scatterv(NULL,NULL,NULL,MPI_INT,cols,col_count,column_type,0,inter_comm);
  printf("Scaberabou");
  /*Iterates over all rows of the matrix */
  for(target_line=0;target_line<col_size;target_line++){
    /* Search for pivot inside the column array */
    if(token==my_rank){
      pivot_found=ERROR;
      /*In the last process we don't want to process the last column of the matrix (as it is the expanded one)*/
       if(my_rank==num_proc-1){ 
         IS_LAST_PROC=1; 
       } 
       for(i = pivot_column; i < col_count-IS_LAST_PROC; i++){ 
         if(findPivot (cols, col_size, i, &pivot_line) == SUCCESS){ 
           pivot_found=SUCCESS; 
           pivot_column=i; 
           multipliers_array = getMultipliers(cols,col_size,pivot_line,pivot_column); 
           break; 
         } 
       } 
       if(!pivot_found){ 
         printf("Passing token onward from process %d token %d",num_proc,token);
         token++; 
       } 
      /* Change this to struct */
       MPI_Bcast(&token,1,MPI_INT,my_rank,MPI_COMM_WORLD); 
       MPI_Bcast(&pivot_line,1,MPI_INT,my_rank,MPI_COMM_WORLD); 
       MPI_Bcast(&pivot_column,1,MPI_INT,my_rank,MPI_COMM_WORLD); 
       MPI_Bcast(multipliers_array,col_size,MPI_FLOAT,my_rank,MPI_COMM_WORLD); 
     } 
     else{ 
      /* Change this to struct */
       MPI_Bcast(&token,1,MPI_INT,token,MPI_COMM_WORLD); 
       MPI_Bcast(&pivot_line,1,MPI_INT,token,MPI_COMM_WORLD); 
       MPI_Bcast(&pivot_column,1,MPI_INT,token,MPI_COMM_WORLD); 
       MPI_Bcast(multipliers_array,col_size,MPI_FLOAT,token,MPI_COMM_WORLD); 
     } 
     changeRows(cols,pivot_line,target_line,pivot_column,col_count); 
     subtract(cols,multipliers_array,my_rank,pivot_column,pivot_line,col_count,col_size); 
     target_line++; 
   } 

  printf("Acabei from %d",my_rank);
  /*GatherV*/
  MPI_Type_free(&column_type);
  MPI_Finalize();
  return 0;

}


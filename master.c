#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "master_func.c"
#define SUCCESS 1
#define ERROR 0
#define NUMBER_OF_CLUSTER_NODES 13
int main(int argc, char *argv[])
{
  char program_name[]="slave";
  int i,j,my_rank,num_proc,num_slaves;
  int divis,r;
  int order=10;
  int *sendcounts;
  int *displs;
  /*AB is an array of columns representing the matrix */
  float **AB; 
  int *array_of_errcodes; 
  MPI_Datatype column_type;
  MPI_Comm  inter_comm;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
  if(my_rank==0){
    AB= (float **) malloc((order+1)*(sizeof(float*) ));
    for(i=0;i<order;i++){
      AB[i]=(float *) malloc((order) * (sizeof(float)));
    }
    if(order+1>NUMBER_OF_CLUSTER_NODES){
      num_slaves=NUMBER_OF_CLUSTER_NODES;
    }
    else{
      num_slaves=order+1;
    }
    array_of_errcodes = (int*) malloc((num_slaves)*sizeof(int));
    MPI_Comm_spawn(program_name, MPI_ARGV_NULL, num_slaves, MPI_INFO_NULL,0, MPI_COMM_WORLD, &inter_comm, array_of_errcodes);
    num_slaves=num_slaves-countErrors(array_of_errcodes,num_slaves);
    for(i=0;i<order;i++){
      for(j=0;j<order+1;j++){
        AB[i][j] = i+j;
      }
    }
    sendcounts= (int*) malloc(sizeof(int)*num_slaves);
    displs= (int*) malloc(sizeof(int)*num_slaves);
    divis=order+1/num_slaves;
    r=(order+1)%num_slaves;
    for(i=0;i<order;i++){
      if(i<r){
        sendcounts[i]=divis+1;
      }
      else{
        sendcounts[i]=divis;
      }
    }
    displs[0]=0;
    for(i=1;i<order;i++){
      displs[i]=sendcounts[i]+displs[i-1];
    }
    MPI_Type_vector(1,order,order+1,MPI_FLOAT,&column_type);
    MPI_Type_commit(&column_type);
    MPI_Bcast(&order,1,MPI_INT,MPI_ROOT,inter_comm);
    MPI_Scatter(sendcounts,1,MPI_INT,NULL,0,MPI_INT,MPI_ROOT,inter_comm);
    MPI_Scatterv(AB,sendcounts,displs,column_type,NULL,0,MPI_INT,MPI_ROOT,inter_comm);
  }


  MPI_Finalize();
  return 0;
}

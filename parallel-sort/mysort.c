#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/select.h>

#include "merge.h"


int main(int argc, char *argv[]) {
    printf("\n");
    char *input_file;
    char *sorting1;
    char *sorting2;
    int num;

    for(int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) { //we know that after "-i" we have our input file name and store it in a variable
            if (i + 1 < argc) {
                input_file = argv[i + 1];
                i++;
            }
        } else if(strcmp(argv[i], "-k") == 0) { //we know that after "-k" we have the number of layer one nodes etc
            if (i + 1 < argc) {
                num = atoi(argv[i + 1]);
                i++;
            }
        }
        else if(strcmp(argv[i], "-e1") == 0) { //we know that after "-e1" is the first sorting program name 
            if (i + 1 < argc) {
                sorting1 = argv[i + 1];
                i++;
            }
        }
        else if(strcmp(argv[i], "-e2") == 0) { //we know that after "-e2" is the second sorting program name
            if (i + 1 < argc) {
                sorting2 = argv[i + 1];
                i++;
            }
        }
    };


    FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        printf("%s could not be opened\n", input_file);
        return 1;
    }

    MyRecord rec;

    fseek (file , 0 , SEEK_END);
    long lSize = ftell (file);
    rewind (file);
    int num_of_voters = (int) lSize/sizeof(rec);

    char line[500];
    int  	custid;
	char 	LastName[SIZEofBUFF];
	char 	FirstName[SIZEofBUFF];
	char	postcode[SSizeofBUFF];

    int arr_num = num_of_voters;
    char arr_num_string[20]; //used later,in exec
    sprintf(arr_num_string, "%d ",arr_num);

    char sorting1_string[20];
    sprintf(sorting1_string,"./%s",sorting1);

    char sorting2_string[20];
    sprintf(sorting2_string,"./%s",sorting2);

    MyRecord *arr = (MyRecord *)malloc(arr_num * sizeof(MyRecord)); //

    for (int i = 0; i < num_of_voters ; i++) {
        fread(arr, sizeof(MyRecord), num_of_voters, file);
   	}
    fclose(file);

    int partition_layer1 = num_of_voters / num; //calculating how many voters each process is going to be given to sort
    int remaining_layer1 = num_of_voters % num; //calculating the remainder of the above calculation (that will be given for sorting the last process of that layer

    int pipes[num][2];
    int i;

    for(i = 0; i < num; i++) { //making the pipes from the root process to the first layer processes
        pipe(pipes[i]);
    }


    for(i = 0; i < num; i++) {
        int id = fork();


        int start = i * partition_layer1;   //calculating the start and end index that the layer 1 process is going to give to his children processes to sort and then he will them to give back to the root
        int end = (start + partition_layer1)-1;
        if(i == num-1) end+= remaining_layer1;
        int prev_end;
        int prev_start = 0;
        if( i==0 ){
            prev_end = end;
        }

        if(id == 0) { //first layer (children of root)
            int j;
            int id2;

            int partition_layer2 = ((end-start)+1)/(num-i); //calculating how many voters each process is going to be given to sort in layer2
            int remaining_layer2 = ((end-start)+1)%(num-i);

            int pipe_to_sorters[num-i][2];
            for(j = 0; j < num-i; j++) {
                pipe(pipe_to_sorters[j]);   //making the pipes from the layer 1 process to the second layer processes
            }

            read(pipes[i][READ], arr, arr_num * sizeof(MyRecord));  //reading the starting array from root

            for(j = 0; j < num-i; j++) { //second layer (children of the layer 1 processes)
                id2 = fork();

                int start2 = start + j * partition_layer2; //calculating the start and end index that the second layer process is going to sort
                int end2 = start2 + partition_layer2-1;
                if(j == (num-i)-1) end2+= remaining_layer2;
                int prev_end2;
                int prev_start2;
                if(j == 0){ 
                    prev_end2 = end2;
                    prev_start2 = start2;
                }

                if(id2 == 0) { //actual second layer
                    
                    char start_string[10];
                    char end_string[10];

                    sprintf(start_string, "%d ",start2);
                    sprintf(end_string, "%d ",end2);
                    
                    dup2(pipe_to_sorters[j][READ], STDIN_FILENO); //redirecting stdin so we can read the array from the pipes
                    dup2(pipe_to_sorters[j][WRITE], STDOUT_FILENO); //redirecting stdin so we can write the (partly) sorted array to the pipes
                    if( j%2==0 ) execlp(sorting1_string,sorting1_string,arr_num_string,start_string,end_string,NULL);
                    else execlp(sorting2_string, sorting2_string,arr_num_string,start_string,end_string,NULL);
                    return 0;

                } else { //first layer - parent of second layer
                    
                    write(pipe_to_sorters[j][WRITE],arr, arr_num * sizeof(MyRecord));
                    wait(NULL);
                    read(pipe_to_sorters[j][READ],arr, arr_num * sizeof(MyRecord)); //array comes back from exec
                    
                    if(j > 0) { //merging (from the second sorter and after) the sorted parts of the array that we currently are in with the with all the previous  already merged(so sorted) parts of the array
                        merge(arr,prev_start2,prev_end2,start2,end2);
                        prev_end2 = end2;
                    }
                    close(pipe_to_sorters[j][WRITE]);
                    close(pipe_to_sorters[j][READ]);
                }
            }

            write(pipes[i][WRITE],arr, arr_num * sizeof(MyRecord)); //returning the array after the doing all the proccesses of each node in the first layer
            close(pipes[i][WRITE]);
            return 0; //ends 1st layer
        } else {  //ROOT 
            
            write(pipes[i][WRITE], arr, arr_num * sizeof(MyRecord)); //writting the starting array to the i-th layer 1 process 
            wait(NULL); //waiting for the layer one process to do what it is supposed to
            read(pipes[i][READ], arr, arr_num * sizeof(MyRecord)); //reading the array from layer1 after it finished its work

            if(i > 0) {
                int prev_start = 0;
                merge(arr,prev_start,prev_end,start,end);
                prev_end = end;
            }

            close(pipes[i][WRITE]);
            close(pipes[i][READ]); // Close the write end in the parent process
        }
    }
    printf("\n\n");

    for (int i = 0; i < num_of_voters; i++) { //printing the result
        printf("%s %s %d %s\n",arr[i].LastName,arr[i].FirstName, arr[i].custid, arr[i].postcode);
    }
    
    free(arr);

    return 0;
}
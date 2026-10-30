#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>
#include <string.h>

#include "myrec.h"

// Insertion sort
int main(int argc, char *argv[]){
    int arr_num = atoi(argv[1]);
    int start = atoi(argv[2]);
    int end = atoi(argv[3]);
    MyRecord *rec_arr = (MyRecord *)malloc(arr_num * sizeof(MyRecord)); //allocating memory for the to be recieved array

    read(READ, rec_arr, arr_num*sizeof(MyRecord));

    for (int i = start + 1; i <= end; i++) {
        MyRecord key = rec_arr[i];
        int j = i - 1;

        while (j >= start && (strcmp(rec_arr[j].LastName, key.LastName) > 0 || (strcmp(rec_arr[j].LastName, key.LastName) == 0 
            && (strcmp(rec_arr[j].FirstName, key.FirstName) > 0
            || (strcmp(rec_arr[j].FirstName, key.FirstName) == 0 && rec_arr[j].custid > key.custid))))) {
            rec_arr[j + 1] = rec_arr[j]; //moving elements that should be below the current key one position ahead of their current one
            j--;
        }
        rec_arr[j + 1] = key;
    }
    write(WRITE, rec_arr, arr_num*sizeof(MyRecord));
    free(rec_arr);
    return 0;
}
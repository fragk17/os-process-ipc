#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "merge.h"

void merge(MyRecord arr[], int start_index1, int end_index1, int start_index2, int end_index2) {
    
    int size = (end_index1 - start_index1 + 1) + (end_index2 - start_index2 + 1);
    MyRecord *merged_arr = (MyRecord *)malloc(size * sizeof(MyRecord));

    int i = start_index1;
    int j = start_index2;
    int index = 0;

    while (i <= end_index1 && j <= end_index2) { // merging elements firstly based on LastName,secondly on FirstName and thirdly on custid
        int cmpLastName = strcmp(arr[i].LastName, arr[j].LastName);
        int cmpFirstName = strcmp(arr[i].FirstName, arr[j].FirstName);

        if (cmpLastName < 0 || (cmpLastName == 0 && cmpFirstName < 0) || (cmpLastName == 0 && cmpFirstName == 0 && arr[i].custid <= arr[j].custid)) {
            merged_arr[index++] = arr[i++];
        } else {
            merged_arr[index++] = arr[j++];
        }
    }

    while (i <= end_index1) {   //copying the possible remaining elements of the first segment into merged_arr 
        merged_arr[index++] = arr[i++];
    }

    while (j <= end_index2) {   //copying the possible remaining elements of the second segment into merged_arr 
        merged_arr[index++] = arr[j++];
    }

    for (i = start_index1, index = 0; i <= end_index2; i++, index++) { //copying merged_arr into the main array
        arr[i] = merged_arr[index];
    }

    free(merged_arr);
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>
#include <string.h>

#include "myrec.h"

// Bubble sort
int main(int argc, char *argv[]){
   int arr_num = atoi(argv[1]);
   int start = atoi(argv[2]);
   int end = atoi(argv[3]);
   MyRecord *rec_arr = (MyRecord *)malloc(arr_num * sizeof(MyRecord)); //allocating memory for the to be recieved array

   read(READ, rec_arr, arr_num*sizeof(MyRecord));

   for (int i = start ; i <= end; i++) {
      for (int j = start; j < end; j++) {
         if (strcmp(rec_arr[j].LastName, rec_arr[j + 1].LastName) > 0) { //comparing based on LastName
            MyRecord temp;
            temp = rec_arr[j];
            rec_arr[j] = rec_arr[j + 1];
            rec_arr[j + 1] = temp;
         } else if (strcmp(rec_arr[j].LastName, rec_arr[j + 1].LastName) == 0) {

            if (strcmp(rec_arr[j].FirstName, rec_arr[j + 1].FirstName) > 0) {   //comparing based on FirstName
               MyRecord temp;
               temp = rec_arr[j];
               rec_arr[j] = rec_arr[j + 1];
               rec_arr[j + 1] = temp;
            } else if (strcmp(rec_arr[j].FirstName, rec_arr[j + 1].FirstName) == 0) {

               if (rec_arr[j].custid > rec_arr[j + 1].custid) {  //comparing based on custid
                  MyRecord temp;
                  temp = rec_arr[j];
                  rec_arr[j] = rec_arr[j + 1];
                  rec_arr[j + 1] = temp;
               }

            }
         }
      }
   }

   write(WRITE, rec_arr, arr_num*sizeof(MyRecord));
   free(rec_arr);
   return 0;
}
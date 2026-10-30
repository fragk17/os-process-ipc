#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#include "stats.h"
#include "myrec.h"


int main(int argc,char *argv[]) {

    char *input_file;
    int acc_id;
    int mid;
    int time_del;
    int value;
    int numOfrecords;
    int rec_id;

    time_t writer_start = time(NULL);

    for(int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) { //we know that after "-f" we have our input file name and store it in a variable
            if (i + 1 < argc) {
                input_file = argv[i + 1];
                i++;
            }
        } else if(strcmp(argv[i], "-l") == 0) { //we know that after "-l" we have the recids that that reader must read
            if (i + 1 < argc) {
                rec_id = atoi(argv[i + 1]);
                i++;
            }
        }
        else if(strcmp(argv[i], "-v") == 0) { //we know that after "-v" is the value that the balance is going to change
            if (i + 1 < argc) {
                value = atoi(argv[i + 1]);
                i++;
            }
        }
        else if(strcmp(argv[i], "-d") == 0) { //we know that after "-d" is the max time the reader will be working
            if (i + 1 < argc) {
                time_del = atoi(argv[i + 1]);
                i++;
            }
        }
        else if(strcmp(argv[i], "-s") == 0) { //we know that after "-s" is the shared memory id
            if (i + 1 < argc) {
                mid = atoi(argv[i + 1]);
                i++;
            }
        }
    }

    void *memory = shmat(mid , NULL , 0);
    if (memory == (void *)-1) perror ("Attaching shared memory");


    stats *shared_mem = (stats *)memory;

    sem_wait(&(shared_mem->write_sem));
    time_t critical_start = time(NULL);

    FILE *file = fopen(input_file, "r+");
    if (file == NULL) {
        printf("%s could not be opened\n", input_file);
        return 1;
    }

    fseek(file,(rec_id - 1)*sizeof(MyRecord),SEEK_SET);

    MyRecord temp;
    fread(&temp , sizeof(MyRecord) , 1 , file);
    temp.balance +=value;
    fseek(file , -1*sizeof(MyRecord) , SEEK_CUR);
    fwrite(&temp , sizeof(MyRecord) , 1 , file);

    srand(time(0));
    int random_sleep = (rand() % time_del) + 1; //generating random sleep time from 1 to time_del(which is the number we got after -d in the comand line arguments)
    sleep(random_sleep);
    sleep(time_del);

    printf("\n%d %s %s %d\n\n",temp.custid,temp.FirstName,temp.LastName,temp.balance);
    fclose(file);
    time_t writer_end = time(NULL);

    shared_mem->finished_writers +=1;
    shared_mem->records_handled +=1;
    shared_mem->writer_time += (writer_end - writer_start);
    if((critical_start - writer_start) >= shared_mem->max_wait) shared_mem->max_wait = (critical_start - writer_start);

    sem_post(&(shared_mem->write_sem));

    int err = shmdt(shared_mem);
    if (err == -1) perror ("Detaching shared memory");

    return 0;
}
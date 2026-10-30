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
    int mid ;
    int time_del;
    int sum;
    float avg;
    char *rec_ids, *f_id_string, *l_id_string;
    int f_id, l_id;

    time_t reader_start = time(NULL);

    for(int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) { //we know that after "-f" we have our input file name and store it in a variable
            if (i + 1 < argc) {
                input_file = argv[i + 1];
                i++;
            }
        } else if(strcmp(argv[i], "-l") == 0) { //we know that after "-l" we have the recids that that reader must read
            if (i + 1 < argc) {
                rec_ids = argv[i + 1];
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

    f_id_string = strtok(rec_ids,",");
    l_id_string = strtok(NULL,",");
    f_id = atoi(f_id_string);
    if(l_id_string == NULL) l_id = f_id;
    else l_id = atoi(l_id_string);

    void *memory = shmat(mid , NULL , 0);
    if (memory == (void *)-1) perror ("Attaching shared memory");


    stats *shared_mem = (stats *)memory;

    sem_wait(&(shared_mem->mutex));
    shared_mem->curr_readers +=1;

    if(shared_mem->curr_readers == 1) sem_wait(&(shared_mem->write_sem));
    sem_post(&(shared_mem->mutex));
    time_t critical_start = time(NULL); //counting time


    FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        printf("%s could not be opened\n", input_file);
        return 1;
    }

    fseek(file,(f_id - 1)*sizeof(MyRecord),SEEK_SET); //getting index before the first id that the reader is going to read

    MyRecord temp;
    sum = 0;
    printf("\n");
    for(int i = f_id;i <= l_id;i++) {
        fread(&temp,sizeof(MyRecord),1,file);
        sum += temp.balance;
        printf("%d %s %s %d\n",temp.custid,temp.FirstName,temp.LastName,temp.balance);
    }
    srand(time(0));
    int random_sleep = (rand() % time_del) + 1; //generating random sleep time from 1 to time_del(which is the number we got after -d in the comand line arguments)
    sleep(random_sleep);
    avg = ((float) sum) / (l_id - f_id + 1);
    printf("\nAverage balance of this reader = %.3f \n",avg);

    fclose(file);
    time_t reader_end = time(NULL);

    shared_mem->finished_readers +=1;
    shared_mem->records_handled +=(l_id - f_id + 1);
    shared_mem->reader_time += (reader_end - reader_start);
    if((critical_start - reader_start) >= shared_mem->max_wait) shared_mem->max_wait = (critical_start - reader_start);

    sem_wait(&(shared_mem->mutex));
    shared_mem->curr_readers -=1;

    if(shared_mem->curr_readers == 0) sem_post(&(shared_mem->write_sem));
    sem_post(&(shared_mem->mutex));


    int err = shmdt(shared_mem);
    if (err == -1) perror ("Detaching shared memory");

    return 0;
}
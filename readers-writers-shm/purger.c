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

int main(int argc,char *argv[]) {

    // key_t key = atoi(argv[2]);

    // int mid = shmget(key, sizeof(stats), 0666);
    // if (mid < 0) perror("shmget failure"); //couldnt find shegment with key "key"
    int mid = atoi(argv[2]);

    void *memory = shmat(mid , NULL , 0);
    if (memory == (void *)-1) perror ("Attaching shared memory");

    stats *shared_mem = (stats *)memory;

    sem_destroy(&(shared_mem->mutex));
    sem_destroy(&(shared_mem->write_sem));


    //1
    printf("\n\n1.Finished Readers = %d\n",shared_mem->finished_readers);
    //2
    if(shared_mem->finished_readers == 0) printf("2.Average Reader Time => Can't compute average reader time as no readers were run\n");
    else {
        float avg = ((float) shared_mem->reader_time) / (shared_mem->finished_readers);
        printf("2.Average Reader Time = %.3f sec\n",avg);
    }
    //3
    printf("3.Finished Writers = %d\n",shared_mem->finished_writers);
    //4
    if(shared_mem->finished_writers == 0) printf("4.Average Writer Time => Can't compute average writer time as no writers were run\n");
    else {
        float avg = ((float) shared_mem->writer_time) / (shared_mem->finished_writers);
        printf("4.Average Writer Time = %.3f sec\n",avg);
    }
    //5
    if(shared_mem->max_wait == -1) printf("5.Max Waiting Time => Can't compute as no readers/writers were run\n");
    else printf("5.Max Waiting Time = %.3f sec\n",(float)shared_mem->max_wait);
    //6
    printf("6.Records Handled = %d\n\n\n",shared_mem->records_handled);

    
    int err = shmdt(shared_mem);
    if (err == -1) perror ("Detaching shared memory");

    err = shmctl(mid , IPC_RMID , 0);
    if (err == -1) perror("Removing shared memory");


    return 0;
}
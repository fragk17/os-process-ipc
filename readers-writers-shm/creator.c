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
// #include "myrec.h"

int main() {
    
    int mid = shmget(IPC_PRIVATE , sizeof(stats) , 0666);
    if(mid == -1) perror("Creating shared memory");

    void *memory = shmat(mid , NULL , 0);
    if (memory == (void *)-1) perror ("Attaching shared memory");

    stats *shared_mem = (stats *)memory;

    shared_mem->finished_readers = 0;
    shared_mem->finished_writers = 0;
    shared_mem->records_handled = 0;
    shared_mem->curr_readers = 0;
    shared_mem->reader_time = 0;
    shared_mem->writer_time = 0;
    shared_mem->max_wait = -1; //if no reader or writer comes
    sem_init(&(shared_mem->mutex),1,1);
    sem_init(&(shared_mem->write_sem),1,1);

    int err = shmdt(shared_mem);
    if (err == -1) perror ("Detaching shared memory");


    printf("Created shared memory with memory id = %d\n",mid);

    return 0;
}
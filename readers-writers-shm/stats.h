#include <semaphore.h>
#include <time.h>

typedef struct {
    int finished_readers;
    int finished_writers;
    int records_handled;
    sem_t mutex;
    sem_t write_sem;
    int curr_readers;
    time_t writer_time;
    time_t reader_time;
    time_t max_wait;
} stats;
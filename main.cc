#include "helper.h"

#define MUTEX 0
#define SPACE 1
#define ITEM 2

void* producer (void* id);
void* consumer (void* id);

// Initialise job queue and parameters.
// s_queue: the size of the queue
// in: the position for producer to put the job
// out: the position for consumer to get the job from
// semid: the semaphore id used for this program
// n_jobs: the number of jobs each producer would produce
int* job_buffer;
int s_queue, in = 0, out = 0, semid, n_jobs;

int main (int argc, char **argv)
{

  int n_producer = 0, n_consumer = 0;

  // Read in and check arguments.
  if (argc != 5) {
    fprintf(stderr, "Oh no! Please supply correct number of arguments.\n");
    return -1;
  }

  s_queue = check_arg(argv[1]);
  n_jobs = check_arg(argv[2]);
  n_producer = check_arg(argv[3]);
  n_consumer = check_arg(argv[4]);
  if (s_queue == -1 || n_jobs == -1 || n_producer == -1 || n_consumer == -1) {
    fprintf(stderr, "Oh no! All arguments should be a positive integer!\n");
    return -1;
  }

  job_buffer = new int[s_queue];

  // Initialise semaphores.
  semid = sem_create(SEM_KEY , 3);
  if (semid == -1) {
    fprintf(stderr, "Oh no! The semophore id is not available. Try another!\n");
    return -1;
  }
  if (sem_init(semid, MUTEX, 1)) {
    fprintf(stderr, "Oh no! MUTEX initialisation failed!\n");
    sem_close(semid);
    return -1;
  }
	if (sem_init(semid, SPACE, s_queue)) {
    fprintf(stderr, "Oh no! SPACE initialisation failed!\n");
    sem_close(semid);
    return -1;
  }
	if (sem_init(semid, ITEM, 0)) {
    fprintf(stderr, "Oh no! ITEM initialisation failed!\n");
    sem_close(semid);
    return -1;
  }

  // Create the required producers and consumers.
  pthread_t producerid_list[n_producer];
  pthread_t consumerid_list[n_consumer];
  int pid_list[n_producer];
  int cid_list[n_consumer];
  for (int i = 0; i < n_producer; i++) {
    pid_list[i] = i + 1;
  }
  for (int i = 0; i < n_consumer; i++) {
    cid_list[i] = i + 1;
  }

  for (int i = 0; i < n_producer; i++) {
    if (pthread_create(&producerid_list[i], NULL, producer, &pid_list[i])) {
      fprintf(stderr, "Oh no! Creation of producer thread %i failed!\n", pid_list[i]);
      sem_close(semid);
      return -1;
    }
  }
  for (int i = 0; i < n_consumer; i++) {
    if (pthread_create(&consumerid_list[i], NULL, consumer, &cid_list[i])) {
      fprintf(stderr, "Oh no! Creation of consumer thread %i failed!\n", cid_list[i]);
      sem_close(semid);
      return -1;
    }
  }

  // Clean up all the processes.
  for (int i = 0; i < n_producer; i++) {
    if (pthread_join(producerid_list[i], NULL)) {
      fprintf(stderr, "Oh no! Producer thread %i refused to join!\n", pid_list[i]);
      sem_close(semid);
      return -1;
    }
  }
  for (int i = 0; i < n_consumer; i++) {
    if (pthread_join(consumerid_list[i], NULL)) {
      fprintf(stderr, "Oh no! Consumer thread %i refused to join!\n", cid_list[i]);
      sem_close(semid);
      return -1;
    }
  }
  
  // Clean dynamic memory and semaphores.
  delete job_buffer;
  sem_close(semid);

  return 0;
}

// The producer process
// The process follows this procedure: while (1) { produce item; down (space); down (mutex); 
//                                     deposit item; up (mutex); up (item);} end producer; 
// The job_buffer circular queue is achieved by using mod and keeping track of queue front index.
// @param: a void pointer to the corresponding producer id.
void* producer (void* input_p_id) 
{
  int* curr_p_id = (int*) input_p_id;

  int jobs_remaining = n_jobs;
  while(jobs_remaining) {

    if (sem_wait_timeout(semid, SPACE)) {
      fprintf(stderr, "Producer(%i): No more jobs to generate.\n", *curr_p_id);
      pthread_exit(0);
    }
    if (sem_wait(semid, MUTEX)) {
      fprintf(stderr, "Oh no! Setting MUTEX in Producer(%i) failed!\n", *curr_p_id);
      pthread_exit(0);
    }

    job_buffer[in] = rand() % 10 + 1;
    fprintf(stderr, "Producer(%i): Job id %i duration %i.\n", *curr_p_id, in + 1, job_buffer[in]);
    in = (in + 1) % s_queue;

    if (sem_signal(semid, MUTEX)) {
      fprintf(stderr, "Oh no! Releasing MUTEX in Producer(%i) failed!\n", job_buffer[in]);
      pthread_exit(0);
    }
    if (sem_signal(semid, ITEM)) {
      fprintf(stderr, "Oh no! Updating ITEM in Producer(%i) failed!\n", job_buffer[in]);
      pthread_exit(0);
    }

    jobs_remaining--;
    sleep (rand() % 5 + 1);
  }
  pthread_exit(0);
}

// The consumer process.
// The process follows this procedure: while(true) {down (item); down (mutex); fetch item;
//                                     up (mutex); up (space); consume item; } end consumer;
// The job_buffer circular queue is achieved by using mod and keeping track of queue end index.
// @param: a void pointer to the corresponding consumer id.
void* consumer (void* input_c_id)
{
  int* curr_c_id = (int*) input_c_id;
  int curr_job_id;
  int curr_job;

  while(true) {
    if (sem_wait_timeout(semid, ITEM)) {
      fprintf(stderr, "Consumer(%i): No more jobs left.\n", *curr_c_id);
      pthread_exit (0);
    }
    if (sem_wait(semid, MUTEX)) {
      fprintf(stderr, "Oh no! Setting MUTEX in Consumer(%i) failed!\n", *curr_c_id);
      pthread_exit(0);
    }
    
    curr_job_id = out + 1;
    curr_job = job_buffer[out];
    out = (out + 1) % s_queue;

    if (sem_signal(semid, MUTEX)) {
      fprintf(stderr, "Oh no! Releasing MUTEX in Consumer(%i) failed!\n", *curr_c_id);
      pthread_exit(0);
    }
    if (sem_signal(semid, SPACE)) {
      fprintf(stderr, "Oh no! Updating SPACE in Consumer(%i) failed!\n", *curr_c_id);
      pthread_exit(0);
    }    
    
    fprintf(stderr, "Consumer(%i): Job id %i executing sleep duration %i.\n", *curr_c_id, curr_job_id, curr_job);
    sleep(curr_job);
    fprintf(stderr, "Consumer(%i): Job id %i completed.\n", *curr_c_id, curr_job_id);
  }
}

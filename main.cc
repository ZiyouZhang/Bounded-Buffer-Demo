#include "helper.h"

#define MUTEX 0
#define SPACE 1
#define ITEM 2
#define PID 3
#define CID 4

void *producer (void *id);
void *consumer (void *id);

// Initialise job queue and parameters
int* job_buffer;
int n_jobs = 0, in = 0, out = 0, semid = 0, p_id = 0, c_id = 0;


int main (int argc, char **argv)
{

  int s_queue = 0, n_producer = 0, n_consumer = 0;

  // Read in and check arguments
  if (argc != 5){
    cerr << "Oh no! Please supply correct number of arguments." << endl;
    return -1;
  }

  s_queue = check_arg(argv[1]);
  n_jobs = check_arg(argv[2]);
  n_producer = check_arg(argv[3]);
  n_consumer = check_arg(argv[4]);
  if (s_queue == -1 || n_jobs == -1 || n_producer == -1 || n_consumer == -1) {
    cerr << "Oh no! All arguments should be a positive integer!" << endl;
    return -1;
  }

  job_buffer = new int[n_jobs];

  // Initialise semaphores
  semid = sem_create(SEM_KEY , 3);
  if (semid == -1) {
    cerr << "Oh no! The semophore id is not available. Try another!" << endl;
    return -1;
  }
  sem_init(semid, MUTEX, 1);
	sem_init(semid, SPACE, n_jobs);
	sem_init(semid, ITEM, 0);
  sem_init(semid, PID, 1);
  sem_init(semid, CID, 1);

  // Create the required producers and consumers
  pthread_t p_id_list[n_producer];
  pthread_t c_id_list[n_consumer];

  for (int i = 0; i < n_producer; i++) {
    sem_wait(semid, PID);
    pthread_create(&p_id_list[i], NULL, producer, NULL);
  }

  for (int i = 0; i < n_consumer; i++) {
    sem_wait(semid, CID);
    pthread_create(&c_id_list[i], NULL, consumer, NULL);
  }

  // Clean up all the processes.
  for (int i = 0; i < n_producer; i++) {
    pthread_join(p_id_list[i], NULL);
  }

  for (int i = 0; i < n_consumer; i++) {
    pthread_join(c_id_list[i], NULL);
  }
  
  // Clean semaphores.
  delete job_buffer;
  sem_close(semid);

  return 0;
}


/*
producer 
while (1) { 
produce item; 
down (space); 
down (mutex); 
deposit item;
up (mutex); 
up (item);
}
end producer; 
*/
void *producer (void *input_p_id) 
{
  sem_wait(semid, PID);
  p_id += 1;
  int curr_p_id = p_id;
  sem_signal(semid, PID);

  int jobs_remaining = n_jobs;
  while(jobs_remaining) {
    if(sem_wait_timeout(semid, SPACE)) {
    cerr << "Producer(" << curr_p_id << "): : No more jobs to generate." << endl;
    pthread_exit(0);
    }
    sem_wait(semid, MUTEX);
    job_buffer[in] = rand() % 10 + 1;
    in = (in + 1) % n_jobs;
    cerr << "Producer(" << curr_p_id << "): Job id " << in + 1;
    cerr << " duration " << job_buffer[in] << endl;
    sem_signal(semid, MUTEX);
    sem_signal(semid, ITEM);
    jobs_remaining--;
    sleep (rand() % 5 + 1);
  }
  pthread_exit(0); // rm
}

/*
consumer 
while (1) {
down (item);
down (mutex);
fetch item;
up (mutex);
up (space);
consume item;
}
end consumer;
*/
void *consumer (void *input_c_id)
{
  sem_wait(semid, CID);
  c_id += 1;
  int curr_c_id = c_id;
  sem_signal(semid, CID);

  int curr_job_id;
  int curr_job;
  while(1) { //rm
    if (sem_wait_timeout(semid, ITEM)){
      cerr << "Consumer(" << curr_c_id << "): : No more jobs left." << endl;
      pthread_exit (0);
    };
    sem_wait(semid, MUTEX);
    curr_job_id = out + 1;
    curr_job = job_buffer[out];
    out = (out + 1) % n_jobs;
    sem_signal(semid, MUTEX);
    sem_signal(semid, SPACE);
    
    cerr << "Consumer(" << curr_c_id << "): " << curr_job_id ;
    cerr << " executing sleep duration " << curr_job << endl;
    sleep(curr_job);
    cerr << "Consumer(" << curr_c_id << "): Job id " << curr_job_id << " completed" << endl;
  }
}

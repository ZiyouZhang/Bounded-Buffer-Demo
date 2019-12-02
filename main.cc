/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"

void *producer (void *id);
void *consumer (void *id);

int main (int argc, char **argv)
{

  // Read in and check arguments
  int s_queue = 0, n_jobs = 0, n_producer = 0, n_consumer = 0;

  string arg_string = argv[1];
  int arguments = check_arg(arg_string.c_str());
  if (arguments == -1) {
    cout << "Oh no! Please supply correct form of arguments." << endl;
    return -1;
  } else {
    s_queue = arguments / 1000 % 10;
    n_jobs = arguments / 100 % 10;
    n_producer = arguments / 10 % 10;
    n_consumer = arguments % 10;
  }

  cout << s_queue << n_jobs << n_producer << n_consumer << endl;
  return 0;


  // if (argc != 5){
  //   cout << "Oh no! Please supply correct number of arguments." << endl;
  //   return -1;
  // }

  // int s_queue = 0, n_jobs = 0, n_producer = 0, n_consumer = 0;
  // s_queue = atoi(argv[1]);
  // n_jobs = atoi(argv[2]);
  // n_producer = atoi(argv[3]);
  // n_consumer = atoi(argv[4]);
  // if (!(s_queue && n_jobs && n_producer && n_consumer)) {
  //   cout << "Oh no! All arguments should be a positive integer!" << endl;
  //   return -1;
  // }

  // Initialise circular queue
  int job_buffer[s_queue] = {0};

  // Initialise semaphores
  int mutex = 0, space = 1, item = 2;
  int sem_id = sem_create(SEM_KEY , 3);
  if (sem_id == -1) {
    cout << "Oh no! The semophore id is not available. Try another!" << endl;
    return -1;
  }
  sem_init(sem_id, mutex, 1);
	sem_init(sem_id, space, n_jobs);
	sem_init(sem_id, item, 0);

  // Create the required producers and consumers

  // Quit, but ensure that there is process clean-up.
  // Clean semaphores

  
  
  pthread_t producerid;
  int parameter = 5;

  pthread_create (&producerid, NULL, producer, (void *) &parameter);

  pthread_join (producerid, NULL);

  sem_close(sem_id);

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
void *producer (void *parameter) 
{

  // TODO

  int *param = (int *) parameter;

  cout << "Parameter = " << *param << endl;

  sleep (1);

  cout << "\nThat was a good sleep - thank you \n" << endl;

  pthread_exit(0);
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
void *consumer (void *id) 
{
    // TODO 

  pthread_exit (0);

}

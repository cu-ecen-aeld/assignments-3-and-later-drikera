#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

   // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
   // hint: use a cast like the one below to obtain thread arguments from your parameter
   //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
   struct thread_data *thread_func_args;
   pthread_mutex_t    *mutex_p;

   int rc;

   thread_func_args = ( struct thread_data * ) thread_param;
   mutex_p = thread_func_args->mutex_p; 


   // wait
   usleep ( thread_func_args->wait_to_obtain_ms * 1000 );
   // obtain mutex
   rc = pthread_mutex_lock( mutex_p );
   if ( rc != 0 ) {
      // mutex not obtained successfully
      thread_func_args->thread_complete_success = false;
   } else {
      // mutex obtained successfully
      // wait
      usleep ( thread_func_args->wait_to_release_ms * 1000 );
      // release mutex
      rc = pthread_mutex_unlock( mutex_p );
      if ( rc != 0 ) {
         // mutex not released
         thread_func_args->thread_complete_success = false;
      } else {
         // mutex released
         thread_func_args->thread_complete_success = true;
      }
   }

   return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
   /**
    * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
    * using threadfunc() as entry point.
    *
    * return true if successful.
    *
    * See implementation details in threading.h file comment block
    */
   struct thread_data *thread_param;
   int rc;

   thread_param                     = (struct thread_data *) malloc ( sizeof ( struct thread_data ));
   thread_param->mutex_p            = mutex;
   thread_param->wait_to_obtain_ms  = wait_to_obtain_ms;
   thread_param->wait_to_release_ms = wait_to_release_ms; 

   // Not needed: thread must remain joinable
   // pthread_attr_t attr;
   // pthread_attr_init ( &attr );
   // pthread_attr_setdetachstate ( &attr, PTHREAD_CREATE_DETACHED );


   rc = pthread_create ( thread,
                    NULL,
                    threadfunc,
                    thread_param );

   if ( rc != 0 ) {
      // thread create unsuccessful
      return false;
   } else {
      // thread create successful
      return true; 
   }

}



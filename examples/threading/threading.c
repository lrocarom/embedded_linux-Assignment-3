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
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    usleep(thread_func_args->wait_to_obtain_ms * 1000);

    int rc = pthread_mutex_lock(thread_func_args->mutex);

    if(rc != 0){

        ERROR_LOG("mutex cannot lock correctly");

        return NULL;
    }

    usleep(thread_func_args->wait_to_release_ms * 1000);

    rc = pthread_mutex_unlock(thread_func_args->mutex);

    if(rc != 0){

        ERROR_LOG("mutex cannot unlock correctly");

        return NULL;
    }
    thread_func_args->thread_complete_success = true;
    
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{

    bool rv = false;


    //First allocate memory dynamically with the default size of the struct thread_data

    struct thread_data* data = malloc(sizeof(struct thread_data));

    
    if( data == NULL){
    
        ERROR_LOG("Cannot assign memory dynamically to the struct");
            
    }
    else {

        data->mutex = mutex;
        
        data->wait_to_obtain_ms = wait_to_obtain_ms;
        
        data->wait_to_release_ms = wait_to_release_ms;

        data -> thread_complete_success = false;

        int rc = pthread_create(thread,NULL,&threadfunc,(void*) data);

        if (rc != 0){

            ERROR_LOG("mutex cannot create correctly");

            rv = false;

        }
        else{

            rv = true;
        }

    }
    



    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    return rv;
}


/*
 * Solution to the hungry birds problem using semaphores and pthreads.
 *
 * Usage linux:
 *      gcc -o hungry_birds hungry_birds.c -pthread 
 *      ./hungry_birds <num_babies>
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_BABIES 50
#define MAX_SLEEP_SECONDS 5

int num_babies; 
int dish_capacity;
int dish;

sem_t mutex;
sem_t cond;

// baby_function. baby eats from the dish and signals to parent when it's empty.
void* baby_function(void* args) {
    long id = (long) args;

    int eaten = 0;
    while (true) {
        sem_wait(&mutex); // wait for access to the dish of worms.

        dish--;
        eaten++;
        printf("> Baby nr %ld ate its %d worm, there are %d worms left on the dish\n", id, eaten, dish);

        if (dish == 0) {
            printf("> BABY %ld: CHIRP CHIRP CHIRP, REEEEEEEEEEEEEEE!!!\n", id);
            sem_post(&cond); // signal parent.
        } else {
            sem_post(&mutex); // give next baby a chance to eat by unlocking mutex.
        }

        sleep(rand() % MAX_SLEEP_SECONDS + 1);
    }
}

// parent_function. parent waits until a baby bird wakes her up and then refills the dish with worms.
void* parent_function(void* args) {
    while (true) {
        sem_wait(&cond); // wait until dish is empty, i.e. until a baby signals.

        printf("> MAMA BIRD: WHY YOU WAKING ME UP KID?\n");
        printf("> MAMA BIRD: OH YOU NEED FOOD HUH? I'LL GO GET SOME WORMIES\n");

        dish = dish_capacity;

        printf("> MAMA BIRD: THE DISH IS FILLED WITH WORMS, COME EAT BABIES!\n");
        printf("> MAMA BIRD: IM GOING BACK TO SLEEP\n");

        sem_post(&mutex); // let babies access dish again.
    }
}

// main, command line args and creating threads.
int main(int argc, char *argv[]) {
    // command line args
    if (argc == 1) {
        num_babies = MAX_BABIES;
    } else if (argc == 2) {
        num_babies = atoi(argv[1]);
        if (num_babies < 1) {
            printf("Must be a positive number of birds!\n");
            return 0;
        }
        if (num_babies > MAX_BABIES) {
            num_babies = MAX_BABIES;
        }
    } else {
        printf("Invalid argument, usage: ./hungry_birds <num_babies>\n");
        return 0;
    }
    dish_capacity = num_babies * 4;
    dish = dish_capacity;

    srand(time(NULL));
    sem_init(&mutex, 0, 1);
    sem_init(&cond, 0, 0);

    printf("\n> Lets watch as a parent bird feeds her %d hungry baby birds!\n\n", num_babies);
    
    // creating threads
    pthread_t parent;
    if(pthread_create(&parent, NULL, parent_function, NULL) != 0) {
        printf("Creating parent thread failed\n");
        return 0;
    }  
    pthread_t babies[MAX_BABIES];
    for (long i = 1; i <= num_babies; i++) {
        if(pthread_create(&babies[i-1], NULL, baby_function, (void*) i) != 0) {
            printf("Creating baby thread %ld failed\n", i);
            return 0;
        } 
    }
    
    pthread_exit(NULL);
}




/*
 * Solution to the bear and honeybees problem using semaphores and pthreads.
 *
 * Usage linux:
 *      gcc -o bear_honeybee bear_honeybee.c -pthread 
 *      ./bear_honeybee
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_BEES 50

int num_bees;
int pot;
int pot_capacity;

sem_t mutex;
sem_t cond;

// honeybee_function.
void* honeybee_function(void* args) {
    long id = (long) args;

    int created = 0;
    while (true) {
        sem_wait(&mutex); // wait for access to the honey pot.

        pot++;
        created++;

        printf("> Bee nr %ld filled the pot for the %d:th time\n", id, created);
        printf("> The pot contains %d honey and its capacity is %d\n", pot, pot_capacity);

        if (pot == pot_capacity) {
            printf("> HONEYBEE %ld: BUZZ BUZZ BUZZ, WAKE UP BEAR!!!\n", id);
            sem_post(&cond); // signal bear.
        } else {
            sem_post(&mutex); // give next honeybee access to the pot.
        }

        sleep(1);
    }
}

// bear_function.
void* bear_function(void* args) {
    while (true) {
        sem_wait(&cond); // wait until pot is full, i.e. when a bee signals.

        printf("> BEAR: IM AWAKE\n");
        printf("> BEAR: OOOOH SOME HONEY!\n");

        pot = 0;

        printf("> BEAR: THANKS FOR THE HONEY, NOW GO MAKE ME SOME MORE!\n");

        sem_post(&mutex); // let bees access dish again.
    }
}

// main, command line args and creating threads.
int main(int argc, char *argv[]) {
    // command line args
    if (argc == 1) {
        num_bees = MAX_BEES;
    } else if (argc == 2) {
        num_bees = atoi(argv[1]);
        if (num_bees < 1) {
            printf("Must be a positive number of bees!\n");
            return 0;
        }
        if (num_bees> MAX_BEES) {
            num_bees = MAX_BEES;
        }
    } else {
        printf("Invalid argument, usage: ./bear_honeybee <num_bees>\n");
        return 0;
    }

    pot = 0;
    pot_capacity = num_bees * 4;

    sem_init(&mutex, 0, 1);
    sem_init(&cond, 0, 0);

    printf("\n> Lets watch as these %d bees fills the honey pot for the bear!\n\n", num_bees);
    
    // creating threads
    pthread_t bear;
    if(pthread_create(&bear, NULL, bear_function, NULL) != 0) {
        printf("Creating bear thread failed\n");
        return 0;
    }  
    pthread_t honeybees[MAX_BEES];
    for (long i = 1; i <= num_bees; i++) {
        if(pthread_create(&honeybees[i-1], NULL, honeybee_function, (void*) i) != 0) {
            printf("Creating bee thread %ld failed\n", i);
            return 0;
        } 
    }
    
    pthread_exit(NULL);
}
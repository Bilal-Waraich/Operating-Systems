#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_PRISONERS 100
#define NUM_DRAWERS 100
#define MAX_OPENS 50

int drawers[NUM_DRAWERS];
pthread_mutex_t global_lock;
pthread_mutex_t drawer_locks[NUM_DRAWERS];

// Counters to track the number of wins for each strategy
int wins_random_global = 0;
int wins_random_drawer = 0;
int wins_smart_global = 0;
int wins_smart_drawer = 0;

// Function to shuffle drawers
void shuffle(int *array, size_t n) {
    if (n > 1) {
        for (size_t i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[i];
            array[i] = array[j];
            array[j] = t;
        }
    }
}

// Random global strategy
void* random_global(void* arg) {
    int prisoner_num = *((int*) arg);  

    pthread_mutex_lock(&global_lock);
    for (int i = 0; i < MAX_OPENS; i++) {
        int random_drawer = rand() % NUM_DRAWERS;  
        if (drawers[random_drawer] == prisoner_num) {
            pthread_mutex_unlock(&global_lock);
            return (void*) 1;  // Prisoner found their number
        }
    }
    pthread_mutex_unlock(&global_lock);
    return (void*) 0;  // Prisoner failed to find their number
}

// Random drawer strategy
void* random_drawer(void* arg) {
    int prisoner_num = *((int*) arg);

    for (int i = 0; i < MAX_OPENS; i++) {
        int random_drawer = rand() % NUM_DRAWERS;

        pthread_mutex_lock(&drawer_locks[random_drawer]);
        if (drawers[random_drawer] == prisoner_num) {
            pthread_mutex_unlock(&drawer_locks[random_drawer]);
            return (void*) 1;  // Prisoner found their number
        }
        pthread_mutex_unlock(&drawer_locks[random_drawer]);
    }

    return (void*) 0;  // Prisoner failed to find their number
}

// Smart global strategy
void* smart_global(void* arg) {
    int prisoner_num = *((int*) arg);
    int drawer_to_open = prisoner_num;

    pthread_mutex_lock(&global_lock);
    for (int i = 0; i < MAX_OPENS; i++) {
        if (drawers[drawer_to_open] == prisoner_num) {
            pthread_mutex_unlock(&global_lock);
            return (void*) 1;  // Prisoner found their number
        }
        drawer_to_open = drawers[drawer_to_open];
    }
    pthread_mutex_unlock(&global_lock);
    return (void*) 0;  // Prisone failed to find their number
}

// Smart drawer strategy
void* smart_drawer(void* arg) {
    int prisoner_num = *((int*) arg);
    int drawer_to_open = prisoner_num;

    for (int i = 0; i < MAX_OPENS; i++) {
        pthread_mutex_lock(&drawer_locks[drawer_to_open]);

        if (drawers[drawer_to_open] == prisoner_num) {
            pthread_mutex_unlock(&drawer_locks[drawer_to_open]);
            return (void*) 1;  // Prisoner found their number
        }

        int next_drawer = drawers[drawer_to_open];
        pthread_mutex_unlock(&drawer_locks[drawer_to_open]);

        drawer_to_open = next_drawer;
    }

    return (void*) 0;  // Prisoner failed to find their number
}

// Thread management function
void run_threads(int n, void* (*strategy)(void *), int* win_counter) {
    pthread_t threads[n];
    int prisoner_nums[n];
    int result;
    int all_found;

    for (int i = 0; i < n; i++) {
        prisoner_nums[i] = i + 1;
        pthread_create(&threads[i], NULL, strategy, &prisoner_nums[i]);
    }

    all_found = 1;  // Assume all prisoners will find their numbers
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], (void**) &result);
        if (result == 0) {
            all_found = 0;  // If anyy prisoner fails, set all_found to 0
        }
    }

    if (all_found) {
        (*win_counter)++;  // Incremnt the win counter if all prisoners found their numbers
    }
}

// Time measurement function
double timeit(int n, void* (*strategy)(void *), int* win_counter) {
    clock_t t1, t2;

    t1 = clock();
    run_threads(n, strategy, win_counter);
    t2 = clock();

    return ((double) (t2 - t1)) / CLOCKS_PER_SEC * 1000;
}

int main(int argc, char* argv[]) {
    int num_games = 100;
    srand(time(NULL));
    for (int i = 0; i < NUM_DRAWERS; i++) {
        drawers[i] = i + 1;
    }

    // Run the strategies and measure the time
    double time_random_global = 0;
    double time_random_drawer = 0;
    double time_smart_global = 0;
    double time_smart_drawer = 0;

    for (int i = 0; i < num_games; i++) {
        shuffle(drawers, NUM_DRAWERS);  // Shuffle drawers before each game
        time_random_global += timeit(NUM_PRISONERS, random_global, &wins_random_global);
        time_random_drawer += timeit(NUM_PRISONERS, random_drawer, &wins_random_drawer);
        time_smart_global += timeit(NUM_PRISONERS, smart_global, &wins_smart_global);
        time_smart_drawer += timeit(NUM_PRISONERS, smart_drawer, &wins_smart_drawer);
    }
    // Output results in the desired format
    printf("random_global %d/%d wins/games = %.2f%% %.3f/100 ms = %.3f ms\n", wins_random_global, num_games, (wins_random_global * 100.0) / num_games,time_random_global / num_games, time_random_global);

    printf("random_drawer %d/%d wins/games = %.2f%% %.3f/100 ms = %.3f ms\n",wins_random_drawer, num_games, (wins_random_drawer * 100.0) / num_games, time_random_drawer / num_games, time_random_drawer);

    printf("smart_global %d/%d wins/games = %.2f%% %.3f/100 ms = %.3f ms\n", wins_smart_global, num_games, (wins_smart_global * 100.0) / num_games, time_smart_global / num_games, time_smart_global);

    printf("smart_drawer %d/%d wins/games = %.2f%% %.3f/100 ms = %.3f ms\n", wins_smart_drawer, num_games, (wins_smart_drawer * 100.0) / num_games, time_smart_drawer / num_games, time_smart_drawer);

    return 0;
}

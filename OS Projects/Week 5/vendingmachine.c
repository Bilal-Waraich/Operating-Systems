/*
Operating Systems
Assignment 5
Bilal Waraich
bwaraich@constructor.university
*/

/*
a) Synchronization solution in pseudocode:

Shared Variables:
sem drink_sem = 0           // Initially, no drinks in the machine (students must wait for supplier)
sem coin_sem = 0            // Semaphore to track the coins inserted (student increments this)
sem mutex = 1               // Binary semaphore for mutual exclusion (ensures safe access to shared resources)
int drinks_in_machine = C    // Initial capacity of the vending machine
const int N = 5             // Number of coins needed to get a drink

machine()
{
    while true do
        for i = 1 to N do
            wait(coin_sem)         // Wait for N coins to be inserted

        wait(mutex)                // Ensure mutual exclusion when dispensing
        if drinks_in_machine > 0 then
            dispense_drink()       // Dispense one drink
            drinks_in_machine -= 1 // Decrease available drinks by 1
            signal(drink_sem)      // Signal that a drink is available
        else
            signal(mutex)          // Release mutex
            wait(drink_sem)        // Wait for the supplier to refill the machine
        signal(mutex)              // Release mutex after dispensing
}

student()
{
    for i = 1 to N do
        wait(mutex)                // Ensure mutual exclusion
        insert_coin()              // Insert one coin
        signal(coin_sem)           // Signal that a coin was inserted
        signal(mutex)              // Release the lock

    wait(drink_sem)                // Wait for a drink to be dispensed
    pickup_drink()                 // Pick up the dispensed drink
}

supplier()
{
    while true do
        wait(mutex)                // Ensure mutual exclusion for refilling
        if drinks_in_machine == 0 then
            for i = 1 to C do
                load_drink()       // Load drinks one by one
            drinks_in_machine = C  // Reset drink count to full capacity
        collect_coins()            // Collect the inserted coins
        signal(drink_sem)          // Signal the machine is ready to dispense again
        signal(mutex)              // Release the mutex
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Default settings
#define DEFAULT_STUDENTS 2
#define DEFAULT_CAPACITY 4
#define COINS_REQUIRED 5

// Shared resources
int drink_count = 0;
int coins_inserted = 0;
int total_coins = 0;
int machine_capacity = DEFAULT_CAPACITY;
int num_students = DEFAULT_STUDENTS;
int students_served = 0;
int current_student = -1; // Tracks current student
int stop = 0; // Flag for stopping threads

// Mutex and condition variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t machine_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t supplier_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t student_cond = PTHREAD_COND_INITIALIZER;

// Supplier thread function
void* supplier(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // Check if it's time to stop
        if (stop) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Supplier waits until the machine is empty
        while (drink_count > 0 && !stop) {
            pthread_cond_wait(&supplier_cond, &mutex);
        }

        if (stop) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Refill machine if empty
        if (drink_count == 0) {
            printf("energy: [%d/%d drinks, %d coins, %d inserted] supplier arriving\n", drink_count, machine_capacity, total_coins, coins_inserted);
            printf("energy: [%d/%d drinks, %d coins, %d inserted] supplier loading %d drinks\n", drink_count, machine_capacity, total_coins, coins_inserted, machine_capacity);
            printf("energy: [%d/%d drinks, %d coins, %d inserted] supplier collected %d coins\n", drink_count, machine_capacity, total_coins, coins_inserted, total_coins);
            ff
            total_coins = 0;
            drink_count = machine_capacity;
            pthread_cond_broadcast(&student_cond); // Notify students
        }

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// Student thread function
void* student(void* arg) {
    int id = *((int*)arg);

    pthread_mutex_lock(&mutex);
    printf("energy: [%d/%d drinks, %d coins, %d inserted] student %d requires an energy drink\n", drink_count, machine_capacity, total_coins, coins_inserted, id);

    // Wait if another student is using the machine
    while (current_student != -1 && current_student != id) {
        pthread_cond_wait(&student_cond, &mutex);
    }

    current_student = id; // This student now using machine

    // Insert coins until enough are inserted
    while (coins_inserted < COINS_REQUIRED) {
        coins_inserted++;
        total_coins++;
        printf("energy: [%d/%d drinks, %d coins, %d inserted] student %d inserted another coin\n", drink_count, machine_capacity, total_coins, coins_inserted, id);
        pthread_cond_signal(&machine_cond); // Notify machine
    }

    // Wait for drink to be dispensed
    while (coins_inserted > 0) {
        pthread_cond_wait(&student_cond, &mutex);
    }

    // Pickup the drink
    printf("energy: [%d/%d drinks, %d coins, %d inserted] student %d picked up a drink\n", drink_count, machine_capacity, total_coins, coins_inserted, id);
    printf("energy: [%d/%d drinks, %d coins, %d inserted] student %d enjoying an energy drink\n", drink_count, machine_capacity, total_coins, coins_inserted, id);

    students_served++;

    current_student = -1;
    pthread_cond_broadcast(&student_cond); // Let next student go

    // Stop system when all students are served
    if (students_served == num_students) {
        stop = 1;
        pthread_cond_signal(&supplier_cond); // Wake up supplier
        pthread_cond_signal(&machine_cond); // Wake up machine
    }

    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL); // End student thread
    return NULL;
}

// Machine thread function
void* machine(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        if (stop) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Machine waits for sufficient coins
        while (coins_inserted < COINS_REQUIRED && !stop) {
            printf("energy: [%d/%d drinks, %d coins, %d inserted] machine waiting for more coins\n", drink_count, machine_capacity, total_coins, coins_inserted);
            pthread_cond_wait(&machine_cond, &mutex);
        }

        if (stop) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Dispense drink if available
        if (drink_count > 0) {
            printf("energy: [%d/%d drinks, %d coins, %d inserted] machine dispensing drink\n", drink_count, machine_capacity, total_coins, coins_inserted);
            drink_count--;
            coins_inserted = 0;
            pthread_cond_broadcast(&student_cond); // Notify student to pick up drink
        } else {
            // Signal supplier if no drinks available
            printf("energy: [0/%d drinks, %d coins, %d inserted] machine cannot dispense, no drinks available\n", machine_capacity, total_coins, coins_inserted);
            pthread_cond_signal(&supplier_cond); // Wake up supplier
        }

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    // Command line argument parsing for number of students and capacity
    int opt;
    while ((opt = getopt(argc, argv, "n:c:")) != -1) {
        switch (opt) {
            case 'n':
                num_students = atoi(optarg);
                break;
            case 'c':
                machine_capacity = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-n num_students] [-c capacity]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    printf("energy: [%d/%d drinks, %d coins, %d inserted] machine booting up\n", drink_count, machine_capacity, total_coins, coins_inserted);

    pthread_t supplier_thread, machine_thread, student_threads[num_students];

    // Supplier thread
    pthread_create(&supplier_thread, NULL, supplier, NULL);

    // Machine thread
    pthread_create(&machine_thread, NULL, machine, NULL);

    // Student threads
    int student_ids[num_students];
    for (int i = 0; i < num_students; i++) {
        student_ids[i] = i;
        printf("energy: [%d/%d drinks, %d coins, %d inserted] student %d established\n", drink_count, machine_capacity, total_coins, coins_inserted, i);
        pthread_create(&student_threads[i], NULL, student, &student_ids[i]);
    }

    // Wait for students to finish
    for (int i = 0; i < num_students; i++) {
        pthread_join(student_threads[i], NULL);
    }

    // Wait for machine and supplier threads to finish
    pthread_join(machine_thread, NULL);
    pthread_join(supplier_thread, NULL);

    // Cleanup
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&machine_cond);
    pthread_cond_destroy(&supplier_cond);
    pthread_cond_destroy(&student_cond);

    printf("All students served and system shutting down.\n");
    return 0;
}

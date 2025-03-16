/*
Operating Systems
Assignment 3
Bilal Waraich
bwaraich@constructor.university
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

// Struct to hold arguments for the thread function
struct ThreadArgs {
    int start; // start range for the thread
    int end; // end range for the thread
    int thread_id; // unique ID for each thread
    int verbose; // flag to indicate verbose mode
};

// Function to calculate sum of digits raised to the given power
int sum_of_powers(int number, int power) {
    int sum = 0; // initializing sum to 0
    int temp = number; // temporary variable to hold the number
    while (temp > 0) { 
        // extracting the last digit of the number
        int digit = temp % 10; 
        // summing up the power of the digit
        sum += pow(digit, power); 
        // removing the last digit from number
        temp /= 10; 
    }
    // returning the calculated sum
    return sum;
}

// Function to check if a number is a Perfect Digital Invariant
int is_pdi(int number) {
    // finding the number of digits
    int digit_count = (int)log10(number) + 1; 
    // checking if sum of digit powers is equal to the number
    if (number == sum_of_powers(number, digit_count)) {
        // returning 1 if it is a PDI
        return 1; 
    }
    // returning 0 if it is not a PDI
    return 0; 
}

// Thread function to search for PDIs in a specified range
void* find_pdi_numbers(void* args) {
    struct ThreadArgs* threadArgs = (struct ThreadArgs*) args; // casting argument to ThreadArgs pointer
    int start = threadArgs->start; // extracting start range from args
    int end = threadArgs->end; // extracting end range from args
    int thread_id = threadArgs->thread_id; // extracting thread ID from args
    int verbose = threadArgs->verbose; // extracting verbose flag from args

    // printing verbose start message if verbose flag is set
    if (verbose) {
        fprintf(stderr, "pdi-numbers: t%d searching [%d,%d]\n", thread_id, start, end);
    }

    // looping through the specified range to check for PDIs
    for (int i = start; i <= end; i++) {
        if (is_pdi(i)) {
            // printing the PDI number if found
            printf("%d\n", i);
        }
    }

    // printing verbose end message if verbose flag is set
    if (verbose) {
        fprintf(stderr, "pdi-numbers: t%d finishing\n", thread_id);
    }

    // exiting the thread
    pthread_exit(NULL);
}

// Function to parse command line arguments
void parse_arguments(int argc, char *argv[], int *start, int *end, int *threads, int *verbose) {
    int opt; // variable to store options from getopt
    // setting default values for arguments
    *start = 1; 
    *end = 10000; 
    *threads = 1; 
    *verbose = 0; 

    // parsing command line options
    while ((opt = getopt(argc, argv, "s:e:t:v")) != -1) {
        switch (opt) {
            // case for start range
            case 's':
                *start = atoi(optarg); // converting string to integer
                break;
            // case for end range
            case 'e':
                *end = atoi(optarg); // converting string to integer
                break;
            // case for number of threads
            case 't':
                *threads = atoi(optarg); // converting string to integer
                break;
            // case for verbose flag
            case 'v':
                *verbose = 1; // enabling verbose mode
                break;
            // default case for invalid option
            default:
                fprintf(stderr, "Usage: %s [-s start] [-e end] [-t threads] [-v]\n", argv[0]);
                // exiting with failure code
                exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {
    int start, end, threads, verbose; // variables to store parsed arguments
    // parsing command line arguments
    parse_arguments(argc, argv, &start, &end, &threads, &verbose);

    pthread_t thread_ids[threads]; // array to hold thread IDs
    struct ThreadArgs threadArgs[threads]; // array to hold arguments for each thread

    // calculating the range each thread should handle
    int range_per_thread = (end - start + 1) / threads; 

    // creating threads to search for PDI numbers
    for (int i = 0; i < threads; i++) {
        // assigning start and end range for each thread
        threadArgs[i].start = start + i * range_per_thread; 
        // ensuring the last thread gets the remaining range
        threadArgs[i].end = (i == threads - 1) ? end : (start + (i + 1) * range_per_thread - 1);
        // assigning thread ID
        threadArgs[i].thread_id = i; 
        // passing verbose flag
        threadArgs[i].verbose = verbose; 

        // creating the thread
        pthread_create(&thread_ids[i], NULL, find_pdi_numbers, (void*) &threadArgs[i]);
    }

    // waiting for all threads to complete and then join
    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }
    
    return 0;
}
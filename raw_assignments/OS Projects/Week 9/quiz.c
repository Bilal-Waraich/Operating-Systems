#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "quiz.h"

// Global quiz structure
static quiz_t quiz;

//Function to end the game
static void end_game() {
    printf("\nGame Over. Your final score is: %u points.\n", quiz.score);
    exit(0);
}

// Signal handle for graceful exit on SIGINT
static void signal_handler(int sig) {
    if (sig == SIGINT) {
        end_game();
    }
}

// Main function to initiate a nd run the quiz game
int main() {
    signal(SIGINT, signal_handler);

    printf("Welcome to the quiz game!\nAnswer with the letter of your choice (a, b, c, or d).\n");
    while (1) {
        char *json_data = fetch("https://opentdb.com/api.php?amount=1&category=18&type=multiple");
        if (json_data == NULL || parse(&quiz, json_data) == -1) {
            fprintf(stderr, "Failed to retrieve question.\n");
            free(json_data);
            continue;
        }

        play(&quiz);
        free(json_data);
    }

    end_game();
    return 0;
}

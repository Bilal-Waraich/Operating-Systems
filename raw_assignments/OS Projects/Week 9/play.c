#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "quiz.h"

// Function to get the user's answer
static int get_answer() {
    char *line = NULL;
    size_t len = 0;
    int answer = -1;

    if (getline(&line, &len, stdin) != -1) {
        answer = line[0] - 'a';
    }
    free(line);
    return answer;
}

// Function to display the question and choices
static void show_question(const quiz_t *quiz) {
    printf("\n%s\n", quiz->question);
    for (int i = 0; i < 4; i++) {
        printf("[%c] %s\n", 'a' + i, quiz->choices[i]);
    }
}

// Main play function to handle a quiz round
int play(quiz_t *quiz) {
    int attempts = 3; // Setting max atempts
    int answer = -1;

    show_question(quiz);

    while (attempts-- > 0) {
        printf("Your choice: ");
        answer = get_answer();

        if (answer == -1) break;

        if (strcmp(quiz->choices[answer], quiz->answer) == 0) {
            printf("Correct!\n");
            quiz->score++;
            return 0;
        } else {
            printf("Incorrect. Try again.\n");
        }
    }

    printf("Out of attempts! The correct answer was: %s\n", quiz->answer);
    return -1;
}

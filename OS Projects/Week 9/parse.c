#include <jansson.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "quiz.h"

// Parse function that fills quiz structure with question data
int parse(quiz_t *quiz, char *json_data) {
    json_t *root, *question, *correct_answer, *choices;
    json_error_t error;

    // Ensuring pointers are valid
    assert(quiz && json_data);

    // Loading JSON data
    root = json_loads(json_data, 0, &error);
    if (root == NULL) {
        fprintf(stderr, "JSON parse error at line %d: %s\n", error.line, error.text);
        return -1;
    }

    // Retrieving fiels from JSON object
    question = json_object_get(root, "question");
    correct_answer = json_object_get(root, "correct_answer");
    choices = json_object_get(root, "choices");

    //Checking types of JSON fields
    if (!json_is_string(question) || !json_is_string(correct_answer) || !json_is_array(choices)) {
        fprintf(stderr, "Invalid JSON structure\n");
        json_decref(root); // Freeing root object
        return -1;
    }

    // Allocating and copying question and correct answer to quiz structure
    quiz->question = strdup(json_string_value(question));
    quiz->answer = strdup(json_string_value(correct_answer));

    if (quiz->question == NULL || quiz->answer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        json_decref(root);
        return -1;
    }

    // Copying up to 4 choices
    size_t num_choices = json_array_size(choices);
    for (size_t i = 0; i < num_choices && i < 4; i++) {
        json_t *choice = json_array_get(choices, i);
        if (json_is_string(choice)) {
            quiz->choices[i] = strdup(json_string_value(choice));
            if (quiz->choices[i] == NULL) {
                fprintf(stderr, "Memory allocation failed for choice %zu\n", i);
                json_decref(root);
                return -1;
            }
        }
    }

    json_decref(root); // Freing JSON root object
    return 0;
}

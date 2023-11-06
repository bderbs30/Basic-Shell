#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "vect.h"
#include "tokens.h"

int is_single_spec_char(char character_to_check)
{
    return character_to_check == '<' || character_to_check == '>' || character_to_check == ';' || character_to_check == '|' || character_to_check == '(' || character_to_check == ')';
}

int is_word_alphanum_other(char character_to_check)
{
    return isalnum(character_to_check) || character_to_check == '/' || character_to_check == '-' || character_to_check == '.' || character_to_check == '_' || character_to_check == '~';
}

/**
 * Reads a string from the given index until a non-alphanumeric character is found.
 * @param string The string to read from.
 * @param index The starting index to read from.
 * @return A dynamically allocated string containing the characters read from the input string.
 *         The caller is responsible for freeing the memory allocated for the returned string.
 */
char *read_string(char *string, int index)
{
    int additional_in_string = 0;
    while (is_word_alphanum_other(string[index + additional_in_string]))
    {
        additional_in_string++;
    }
    char *result = strndup(string + index, additional_in_string);
    return result;
}

/**
 * Reads a string within quotes starting from the given index.
 *
 * @param string The string to search within.
 * @param index The index of the opening quote.
 *
 * @return A pointer to the newly allocated string containing the contents within the quotes.
 *         Returns NULL if no matching closing quote is found.
 */
char *read_in_quotes(char *string, int index)
{
    char quote = '"';
    int additional_in_string = 1; // we don't want to check the quote itself so initialize to + 1
    while (string[index + additional_in_string] != quote && (index + additional_in_string) < strlen(string))
    {
        if (string[index + additional_in_string] == '\0') // have we reached the end of the line
        {
            return NULL; // we didn't find any context within qoutes
        }
        additional_in_string++;
    }
    char *result = strndup(string + index + 1, additional_in_string - 1);
    return result;
}

/**
 * Tokenizes a given input string into individual tokens.
 * @param input The input string to tokenize.
 * @return A pointer to a vector of tokens.
 */

vect_t *tokenize(char *input)
{

    vect_t *v = vect_new();
    int amountToParse = strlen(input);
    int current_point_cmd = 0; // the point at which we are looking through the input

    while (current_point_cmd < amountToParse)
    {
        if (input[current_point_cmd] == '\'')
        {
            current_point_cmd++;
        }
        else if (isspace(input[current_point_cmd]))
        {
            current_point_cmd++;
        }
        else if (input[current_point_cmd] == '"') // if the current char is a double or single qoute
        {
            char *quoted_string = read_in_quotes(input, current_point_cmd);
            if (quoted_string != NULL)
            {
                vect_add(v, quoted_string);
                current_point_cmd += strlen(quoted_string) + 2; // add 2 to account for the quotes
                free(quoted_string);
            }
            else
            {
                current_point_cmd++;
            }
        }
        else if (is_single_spec_char(input[current_point_cmd]))
        {
            char op[2] = {input[current_point_cmd], '\0'};
            vect_add(v, op);
            current_point_cmd++;
        }
        else if (is_word_alphanum_other(input[current_point_cmd]))
        {
            char *word = read_string(input, current_point_cmd);

            vect_add(v, word);
            current_point_cmd += strlen(word);
            free(word);
        }
        else
        {
            printf("it got lost");
        }
    }
    return v;
}

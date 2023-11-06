#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "vect.h"
#include "tokens.h"

int main(int argc, char **argv)
{
  char buffer[256]; // Define a buffer to store the input.

  char *input_line = fgets(buffer, sizeof(buffer), stdin);

  if (input_line != NULL)
  {
    vect_t *tokens = tokenize(input_line);

    for (int i = 0; i < vect_size(tokens); i++)
    {
      printf("%s\n", (char *)vect_get(tokens, i));
    }

    vect_delete(tokens);
  }

  return 0;
}

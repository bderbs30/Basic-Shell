#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "vect.h"

/** Construct a new empty vector. */
vect_t *vect_new()
{

  vect_t *v = malloc(sizeof(vect_t));
  assert(v != NULL);
  v->capacity = VECT_INITIAL_CAPACITY; // set the initial capacity
  v->size = 0;
  v->data = malloc(sizeof(char *) * v->capacity);
  assert(v->data != NULL);

  return v;
}

/** Delete the vector, freeing all memory it occupies. */
void vect_delete(vect_t *v)
{

  for (unsigned int i = 0; i < v->size; i++)
  {
    free(v->data[i]);
  }
  free(v->data);
  free(v);
}

/** Get the element at the given index. */
const char *vect_get(vect_t *v, unsigned int idx)
{
  assert(v != NULL);
  assert(idx <= v->size);

  return v->data[idx];
}

/** Add an element to the back of the vector. */
void vect_add(vect_t *v, const char *elt)
{
  if (v->size == v->capacity)
  {
    v->capacity *= VECT_GROWTH_FACTOR;
    v->data = (char **)realloc(v->data, v->capacity * sizeof(char *));
  }
  v->data[v->size] = strdup(elt);
  v->size++;
}

// returns the index of the first occurence of the given itemToFind in the vector
// returns -1 if the itemToFind is not in the vector
int vect_find(vect_t *v, const char *itemToFind)
{
  assert(v != NULL);

  for (unsigned int i = 0; i < v->size; i++)
  {
    if (strcmp(v->data[i], itemToFind) == 0)
    {
      return i;
    }
  }

  return -1;
}

// returns a new vector from all the elements of the start to the end, including start and exluding end.
vect_t *partitioned_vector(vect_t *v, int start, int end)
{
  assert(v != NULL);
  assert(start >= 0 && start <= end && end <= v->size);

  vect_t *new_v = vect_new();
  for (int i = start; i < end; i++)
  {
    vect_add(new_v, vect_get(v, i));
  }

  return new_v;
}

/** The number of items currently in the vector. */
unsigned int vect_size(vect_t *v)
{
  assert(v != NULL);

  return v->size;
}

// does the given vector have the string has one of the tokens
// returns 1 if the item is in the vector, 0 otherwise
int within_vect(vect_t *vector, char *string)
{
  assert(vector != NULL);
  assert(string != NULL);

  for (unsigned int i = 0; i < vector->size; i++)
  {
    if (strcmp(vector->data[i], string) == 0)
    {
      return 1;
    }
  }
  return 0;
}


#ifndef VECT_H
#define VECT_H

typedef struct vect
{
    int capacity;
    int size;
    char **data;
} vect_t;

/** Construct a new empty vector. */
vect_t *vect_new();

/** Delete the vector, freeing all memory it occupies. */
void vect_delete(vect_t *v);

/** Get the element at the given index. */
const char *vect_get(vect_t *v, unsigned int idx);

/** Add an element to the back of the vector. */
void vect_add(vect_t *v, const char *elt);

// finds the index of the given char in the vector
int vect_find(vect_t *v, const char *item);

// returns a new vector from all the elements of the start to the end, including start and exluding end.
vect_t *partitioned_vector(vect_t *v, int start, int end);

// returns the size of the given vector
unsigned int vect_size(vect_t *v);

int within_vect(vect_t *vector, char *string);


/* Vector configuration. */
#define VECT_INITIAL_CAPACITY 2
#define VECT_GROWTH_FACTOR 2

#define VECT_MAX_CAPACITY UINT_MAX

#endif /* ifndef VECT_H */

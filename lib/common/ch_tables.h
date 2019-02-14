/*
 *
 *      Filename: ch_tables.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-10 13:57:08
 * Last Modified: 2017-01-10 13:57:08
 */

#ifndef CH_TABLES_H
#define CH_TABLES_H

/** the table abstract data type */
typedef struct ch_table_t ch_table_t;

/** @see ch_array_header_t */
typedef struct ch_array_header_t ch_array_header_t;

/**
 * The (opaque) structure for string-content tables.
 */
typedef struct ch_table_entry_t ch_table_entry_t;

#include "ch_mpool.h"

/** An opaque array type */
struct ch_array_header_t {
    /** The pool the array is allocated out of */
    ch_pool_t *pool;
    /** The amount of memory allocated for each element of the array */
    int elt_size;
    /** The number of active elements in the array */
    int nelts;
    /** The number of elements allocated in the array */
    int nalloc;
    /** The elements in the array */
    char *elts;
};



/** The type for each entry in a string-content table */
struct ch_table_entry_t {
    /** The key for the current table entry */
    char *key;          /* maybe NULL in future;
                         * check when iterating thru table_elts
                         */
    /** The value for the current table entry */
    char *val;

    /** A checksum for the key, for use by the ch_table internals */
    uint32_t key_checksum;
};

/**
 * Get the elements from a table.
 * @param t The table
 * @return An array containing the contents of the table
 */
extern const ch_array_header_t * ch_table_elts(const ch_table_t *t);

/**
 * Determine if the table is empty (either NULL or having no elements).
 * @param t The table to check
 * @return True if empty, False otherwise
 */
extern int ch_is_empty_table(const ch_table_t *t);

/**
 * Determine if the array is empty (either NULL or having no elements).
 * @param a The array to check
 * @return True if empty, False otherwise
 */
extern int ch_is_empty_array(const ch_array_header_t *a);

/**
 * Create an array.
 * @param p The pool to allocate the memory out of
 * @param nelts the number of elements in the initial array
 * @param elt_size The size of each element in the array.
 * @return The new array
 */
extern ch_array_header_t * ch_array_make(ch_pool_t *p,
                                                 int nelts, int elt_size);

/**
 * Add a new element to an array (as a first-in, last-out stack).
 * @param arr The array to add an element to.
 * @return Location for the new element in the array.
 * @remark If there are no free spots in the array, then this function will
 *         allocate new space for the new element.
 */
extern void * ch_array_push(ch_array_header_t *arr);

/** A helper macro for accessing a member of an GW array.
 *
 * @param ary the array
 * @param i the index into the array to return
 * @param type the type of the objects stored in the array
 *
 * @return the item at index i
 */
#define CH_ARRAY_IDX(ary,i,type) (((type *)(ary)->elts)[i])

/** A helper macro for pushing elements into an GW array.
 *
 * @param ary the array
 * @param type the type of the objects stored in the array
 *
 * @return the location where the new object should be placed
 */
#define CH_ARRAY_PUSH(ary,type) (*((type *)ch_array_push(ary)))

/**
 * Remove an element from an array (as a first-in, last-out stack).
 * @param arr The array to remove an element from.
 * @return Location of the element in the array.
 * @remark If there are no elements in the array, NULL is returned.
 */
extern void * ch_array_pop(ch_array_header_t *arr);

/**
 * Remove all elements from an array.
 * @param arr The array to remove all elements from.
 * @remark As the underlying storage is allocated from a pool, no
 * memory is freed by this operation, but is available for reuse.
 */
extern void ch_array_clear(ch_array_header_t *arr);

/**
 * Concatenate two arrays together.
 * @param dst The destination array, and the one to go first in the combined 
 *            array
 * @param src The source array to add to the destination array
 */
extern void ch_array_cat(ch_array_header_t *dst,
			        const ch_array_header_t *src);

/**
 * Copy the entire array.
 * @param p The pool to allocate the copy of the array out of
 * @param arr The array to copy
 * @return An exact copy of the array passed in
 * @remark The alternate ch_array_copy_hdr copies only the header, and arranges 
 *         for the elements to be copied if (and only if) the code subsequently
 *         does a push or arraycat.
 */
extern ch_array_header_t * ch_array_copy(ch_pool_t *p,
                                      const ch_array_header_t *arr);
/**
 * Copy the headers of the array, and arrange for the elements to be copied if
 * and only if the code subsequently does a push or arraycat.
 * @param p The pool to allocate the copy of the array out of
 * @param arr The array to copy
 * @return An exact copy of the array passed in
 * @remark The alternate ch_array_copy copies the *entire* array.
 */
extern ch_array_header_t * ch_array_copy_hdr(ch_pool_t *p,
                                      const ch_array_header_t *arr);

/**
 * Append one array to the end of another, creating a new array in the process.
 * @param p The pool to allocate the new array out of
 * @param first The array to put first in the new array.
 * @param second The array to put second in the new array.
 * @return A new array containing the data from the two arrays passed in.
*/
extern ch_array_header_t * ch_array_append(ch_pool_t *p,
                                      const ch_array_header_t *first,
                                      const ch_array_header_t *second);

/**
 * Generate a new string from the ch_pool_t containing the concatenated 
 * sequence of substrings referenced as elements within the array.  The string 
 * will be empty if all substrings are empty or null, or if there are no 
 * elements in the array.  If sep is non-NUL, it will be inserted between 
 * elements as a separator.
 * @param p The pool to allocate the string out of
 * @param arr The array to generate the string from
 * @param sep The separator to use
 * @return A string containing all of the data in the array.
 */
extern char * ch_array_pstrcat(ch_pool_t *p,
				      const ch_array_header_t *arr,
				      const char sep);

/**
 * Make a new table.
 * @param p The pool to allocate the pool out of
 * @param nelts The number of elements in the initial table.
 * @return The new table.
 * @warning This table can only store text data
 */
extern ch_table_t * ch_table_make(ch_pool_t *p, int nelts);

/**
 * Create a new table and copy another table into it.
 * @param p The pool to allocate the new table out of
 * @param t The table to copy
 * @return A copy of the table passed in
 * @warning The table keys and respective values are not copied
 */
extern ch_table_t * ch_table_copy(ch_pool_t *p,
                                          const ch_table_t *t);

/**
 * Create a new table whose contents are deep copied from the given
 * table. A deep copy operation copies all fields, and makes copies
 * of dynamically allocated memory pointed to by the fields.
 * @param p The pool to allocate the new table out of
 * @param t The table to clone
 * @return A deep copy of the table passed in
 */
extern ch_table_t * ch_table_clone(ch_pool_t *p,
                                           const ch_table_t *t);

/**
 * Delete all of the elements from a table.
 * @param t The table to clear
 */
extern void ch_table_clear(ch_table_t *t);

/**
 * Get the value associated with a given key from the table.  After this call,
 * the data is still in the table.
 * @param t The table to search for the key
 * @param key The key to search for (case does not matter)
 * @return The value associated with the key, or NULL if the key does not exist. 
 */
extern const char * ch_table_get(const ch_table_t *t, const char *key);

/**
 * Get values associated with a given key from the table.      If more than one
 * value exists, return a comma separated list of values.  After this call, the
 * data is still in the table.
 * @param p The pool to allocate the combined value from, if necessary
 * @param t The table to search for the key
 * @param key The key to search for (case does not matter)
 * @return The value associated with the key, or NULL if the key does not exist.
 */
extern const char * ch_table_getm(ch_pool_t *p, const ch_table_t *t,
                                         const char *key);

/**
 * Add a key/value pair to a table.  If another element already exists with the
 * same key, this will overwrite the old data.
 * @param t The table to add the data to.
 * @param key The key to use (case does not matter)
 * @param val The value to add
 * @remark When adding data, this function makes a copy of both the key and the
 *         value.
 */
extern void ch_table_set(ch_table_t *t, const char *key,
                                const char *val);

/**
 * Add a key/value pair to a table.  If another element already exists with the
 * same key, this will overwrite the old data.
 * @param t The table to add the data to.
 * @param key The key to use (case does not matter)
 * @param val The value to add
 * @warning When adding data, this function does not make a copy of the key or 
 *          the value, so care should be taken to ensure that the values will 
 *          not change after they have been added..
 */
extern void ch_table_setn(ch_table_t *t, const char *key,
                                 const char *val);

/**
 * Remove data from the table.
 * @param t The table to remove data from
 * @param key The key of the data being removed (case does not matter)
 */
extern void ch_table_unset(ch_table_t *t, const char *key);

/**
 * Add data to a table by merging the value with data that has already been 
 * stored. The merging is done by concatenating the two values, separated
 * by the string ", ".
 * @param t The table to search for the data
 * @param key The key to merge data for (case does not matter)
 * @param val The data to add
 * @remark If the key is not found, then this function acts like ch_table_add
 */
extern void ch_table_merge(ch_table_t *t, const char *key,
                                  const char *val);

/**
 * Add data to a table by merging the value with data that has already been 
 * stored. The merging is done by concatenating the two values, separated
 * by the string ", ".
 * @param t The table to search for the data
 * @param key The key to merge data for (case does not matter)
 * @param val The data to add
 * @remark If the key is not found, then this function acts like ch_table_addn
 */
extern void ch_table_mergen(ch_table_t *t, const char *key,
                                   const char *val);

/**
 * Add data to a table, regardless of whether there is another element with the
 * same key.
 * @param t The table to add to
 * @param key The key to use
 * @param val The value to add.
 * @remark When adding data, this function makes a copy of both the key and the
 *         value.
 */
extern void ch_table_add(ch_table_t *t, const char *key,
                                const char *val);

/**
 * Add data to a table, regardless of whether there is another element with the
 * same key.
 * @param t The table to add to
 * @param key The key to use
 * @param val The value to add.
 * @remark When adding data, this function does not make a copy of the key or the
 *         value, so care should be taken to ensure that the values will not 
 *         change after they have been added.
 */
extern void ch_table_addn(ch_table_t *t, const char *key,
                                 const char *val);

/**
 * Merge two tables into one new table.
 * @param p The pool to use for the new table
 * @param overlay The first table to put in the new table
 * @param base The table to add at the end of the new table
 * @return A new table containing all of the data from the two passed in
 */
extern ch_table_t * ch_table_overlay(ch_pool_t *p,
                                             const ch_table_t *overlay,
                                             const ch_table_t *base);

/**
 * Declaration prototype for the iterator callback function of ch_table_do()
 * and ch_table_vdo().
 * @param rec The data passed as the first argument to ch_table_[v]do()
 * @param key The key from this iteration of the table
 * @param value The value from this iteration of the table
 * @remark Iteration continues while this callback function returns non-zero.
 * To export the callback function for ch_table_[v]do() it must be declared 
 * in the _NONSTD convention.
 */
typedef int (ch_table_do_callback_fn_t)(void *rec, const char *key, 
                                                    const char *value);

/** 
 * Iterate over a table running the provided function once for every
 * element in the table.  The varargs array must be a list of zero or
 * more (char *) keys followed by a NULL pointer.  If zero keys are
 * given, the @param comp function will be invoked for every element
 * in the table.  Otherwise, the function is invoked only for those
 * elements matching the keys specified.
 *
 * If an invocation of the @param comp function returns zero,
 * iteration will continue using the next specified key, if any.
 *
 * @param comp The function to run
 * @param rec The data to pass as the first argument to the function
 * @param t The table to iterate over
 * @param ... A varargs array of zero or more (char *) keys followed by NULL
 * @return FALSE if one of the comp() iterations returned zero; TRUE if all
 *            iterations returned non-zero
 * @see ch_table_do_callback_fn_t
 */
int ch_table_do(ch_table_do_callback_fn_t *comp,
                                     void *rec, const ch_table_t *t, ...);

/** 
 * Iterate over a table running the provided function once for every
 * element in the table.  The @param vp varargs parameter must be a
 * list of zero or more (char *) keys followed by a NULL pointer.  If
 * zero keys are given, the @param comp function will be invoked for
 * every element in the table.  Otherwise, the function is invoked
 * only for those elements matching the keys specified.
 *
 * If an invocation of the @param comp function returns zero,
 * iteration will continue using the next specified key, if any.
 *
 * @param comp The function to run
 * @param rec The data to pass as the first argument to the function
 * @param t The table to iterate over
 * @param vp List of zero or more (char *) keys followed by NULL
 * @return FALSE if one of the comp() iterations returned zero; TRUE if all
 *            iterations returned non-zero
 * @see ch_table_do_callback_fn_t
 */
extern int ch_table_vdo(ch_table_do_callback_fn_t *comp,
                               void *rec, const ch_table_t *t, va_list vp);

/** flag for overlap to use ch_table_setn */
#define CH_OVERLAP_TABLES_SET   (0)
/** flag for overlap to use ch_table_mergen */
#define CH_OVERLAP_TABLES_MERGE (1)
/**
 * For each element in table b, either use setn or mergen to add the data
 * to table a.  Which method is used is determined by the flags passed in.
 * @param a The table to add the data to.
 * @param b The table to iterate over, adding its data to table a
 * @param flags How to add the table to table a.  One of:
 *          CH_OVERLAP_TABLES_SET        Use ch_table_setn
 *          CH_OVERLAP_TABLES_MERGE      Use ch_table_mergen
 * @remark  When merging duplicates, the two values are concatenated,
 *          separated by the string ", ".
 * @remark  This function is highly optimized, and uses less memory and CPU cycles
 *          than a function that just loops through table b calling other functions.
 */
/**
 * Conceptually, ch_table_overlap does this:
 *
 * <pre>
 *  ch_array_header_t *barr = ch_table_elts(b);
 *  ch_table_entry_t *belt = (ch_table_entry_t *)barr->elts;
 *  int i;
 *
 *  for (i = 0; i < barr->nelts; ++i) {
 *      if (flags & CH_OVERLAP_TABLES_MERGE) {
 *          ch_table_mergen(a, belt[i].key, belt[i].val);
 *      }
 *      else {
 *          ch_table_setn(a, belt[i].key, belt[i].val);
 *      }
 *  }
 * </pre>
 *
 *  Except that it is more efficient (less space and cpu-time) especially
 *  when b has many elements.
 *
 *  Notice the assumptions on the keys and values in b -- they must be
 *  in an ancestor of a's pool.  In practice b and a are usually from
 *  the same pool.
 */

extern void ch_table_overlap(ch_table_t *a, const ch_table_t *b,
                                     unsigned flags);

/**
 * Eliminate redundant entries in a table by either overwriting
 * or merging duplicates.
 *
 * @param t Table.
 * @param flags CH_OVERLAP_TABLES_MERGE to merge, or
 *              CH_OVERLAP_TABLES_SET to overwrite
 * @remark When merging duplicates, the two values are concatenated,
 *         separated by the string ", ".
 */
extern void ch_table_compress(ch_table_t *t, unsigned flags);

#endif /* CH_TABLES_H */

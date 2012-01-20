#ifndef TEMPERED_H
#define TEMPERED_H

#include <stdbool.h>

#include "temper_type.h"

struct tempered_device_list {
	struct tempered_device_list *next;
	char *path;
	const struct temper_type *type;
};

/** Get the error message for the last error that occurred. */
const char *tempered_error( void );

/** Initialize the TEMPered library.
 *
 * This function initializes the TEMPered library. Calling it is not strictly
 * necessary, as it will be called automatically when needed, but should be
 * called at the start of execution if there's a chance the library will be
 * used by multiple threads simultaneously.
 * @return true on success, false on error.
 */
bool tempered_init( void );

/** Finalize the TEMPered library.
 *
 * This function should be called at the end of execution to avoid memory leaks.
 * @return true on success, false on error.
 */
bool tempered_exit( void );

/** Enumerate the TEMPer devices.
 *
 * This function returns a linked list of all the recognized TEMPer devices
 * attached to the system (excluding the ones that are ignored).
 *
 * @return A pointer to the first device in the enumerated list, or NULL on
 * error. This list should be freed with tempered_free_device_list when you
 * are done with it.
 */
struct tempered_device_list* tempered_enumerate( void );

/** Free the memory used by the given device list.
 *
 * Once this method has been called with a list, the given list should not be
 * dereferenced.
 *
 * @param list The device list to be freed. Can be NULL to not free anything.
 */
void tempered_free_device_list( struct tempered_device_list *list );

#endif

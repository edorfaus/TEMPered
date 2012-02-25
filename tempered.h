#ifndef TEMPERED_H
#define TEMPERED_H

#include <stdbool.h>

#include "temper_type.h"

struct tempered_device_list {
	struct tempered_device_list *next;
	char *path;
	const struct temper_type *type;
};

struct tempered_device_;
typedef struct tempered_device_ tempered_device;

/** Initialize the TEMPered library.
 *
 * This function initializes the TEMPered library. Calling it is not strictly
 * necessary, as it will be called automatically when needed, but should be
 * called at the start of execution if there's a chance the library will be
 * used by multiple threads simultaneously.
 * @param error If an error occurs and this is not NULL, it will be set to the
 * error message.
 * @return true on success, false on error.
 */
bool tempered_init( char **error );

/** Finalize the TEMPered library.
 *
 * This function should be called at the end of execution to avoid memory leaks.
 * @param error If an error occurs and this is not NULL, it will be set to the
 * error message.
 * @return true on success, false on error.
 */
bool tempered_exit( char **error );

/** Enumerate the TEMPer devices.
 *
 * This function returns a linked list of all the recognized TEMPer devices
 * attached to the system (excluding the ones that are ignored).
 *
 * @param error If an error occurs and this is not NULL, it will be set to the
 * error message.
 * @return A pointer to the first device in the enumerated list, or NULL on
 * error. This list should be freed with tempered_free_device_list when you
 * are done with it.
 */
struct tempered_device_list* tempered_enumerate( char **error );

/** Free the memory used by the given device list.
 *
 * Once this method has been called with a list, the given list should not be
 * dereferenced.
 *
 * @param list The device list to be freed. Can be NULL to not free anything.
 */
void tempered_free_device_list( struct tempered_device_list *list );

/** Open a given device from the device list.
 *
 * The returned handle should be closed with tempered_close() when you are done
 * using the device.
 * @param list The device list entry that should be opened.
 * @param error If an error occurs and this is not NULL, it will be set to the
 * error message.
 * @return The opened device, or NULL on error.
 * @see tempered_close()
 * @see tempered_enumerate()
 */
tempered_device* tempered_open( struct tempered_device_list *list, char **error );

/** Close an open device.
 *
 * Once a device handle has been closed, it should no longer be used.
 * @param device The device handle to close.
 * @see tempered_open()
 */
void tempered_close( tempered_device *device );

/** Get the type name of the given device.
 * @param device The device to get the type name of.
 * @param error If an error occurs and this is not NULL, it will be set to the
 * error message.
 * @return The type name of the given device, or NULL on error.
 *
 * The returned type name must not be freed, might be statically allocated,
 * and is only valid as long as the device handle is open.
 */
char const * tempered_get_type_name( tempered_device *device, char **error );

#endif

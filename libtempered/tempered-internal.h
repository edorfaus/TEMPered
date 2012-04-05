#ifndef TEMPERED_INTERNAL_H
#define TEMPERED_INTERNAL_H

/** This file contains the headers for internal types and functions that are
 * used by several of the files in this project, but that are not supposed to
 * be used by the programs that use this library.
 */

#include "tempered.h"

#include "temper_type.h"

/** This is the actual struct the tempered_device opaque type is built from.
 */
struct tempered_device_ {
	/** The type of temper this device is. */
	struct temper_type const *type;
	
	/** The subtype of the temper type this device is. */
	struct temper_subtype const *subtype;
	
	/** The path for this device. */
	char *path;
	
	/** The last error that occurred with this device. */
	char *error;
	
	/** Device-specific data for this device. */
	void *data;
};

/** Set the last error message for the given device.
 * @param device The device for which to set the last error message.
 * Note that if this parameter is NULL, the given error message will not be
 * taken ownership of, leaving you to free it yourself.
 * @param error The error message to set for the device.
 * This message must be dynamically allocated, as the device will take
 * ownership of the string and will pass the given pointer to free()
 * when the device is closed or another error is set.
 */
void tempered_set_error( tempered_device *device, char *error );

#endif

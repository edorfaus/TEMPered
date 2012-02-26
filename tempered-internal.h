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
	
	/** The path for this device. */
	char *path;
	
	/** The last error that occurred with this device. */
	char *error;
	
	/** HID device pointer for this device.
	 * TODO: move this into type-specific data.
	 */
	hid_device *hid_dev;
};

/** Set the last error message for the given device.
 * @param device The device for which to set the last error message.
 * @param error The error message to set for the device.
 * This message must be allocated on the heap, as it will be passed to free()
 * when the device is closed or another error is set.
 */
void tempered_set_error( tempered_device *device, char *error );

#endif

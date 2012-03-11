#ifndef TEMPER_TYPE_H
#define TEMPER_TYPE_H

#include <stdbool.h>

#include "tempered.h"

/** This struct represents a type of recognized device, containing some useful
 * information on it that is used to recognize it and to read the temperature
 * from its sensor.
 */
struct temper_type {
	
	/** Name of device type, and end-of-list marker (this is NULL at EOL).
	 */
	char *name;
	
	/** Whether or not devices with this type should be ignored.
	 *
	 * This is used to mark those interfaces that belong to recognized devices
	 * but are not themselves useful to us, usually because the device has
	 * multiple interfaces where only one can be used to read the temperature.
	 */
	bool ignored;
	
	/** USB Device Vendor ID for this type.
	 */
	unsigned short vendor_id;
	
	/** USB Device Product ID for this type.
	 */
	unsigned short product_id;
	
	/** USB Interface number for this type.
	 */
	int interface_number;
	
	/** Type specific data for this type.
	 */
	void *data;
	
	/** The method to use to open a device of this type.
	 */
	bool (*open)( tempered_device* );
	
	/** The method to use to close a device of this type.
	 */
	void (*close)( tempered_device* );
	
	/** The method to use to read the sensors on a device of this type.
	 */
	bool (*read_sensors)( tempered_device* );
	
	/** The method to use to get the temperature from a device of this type.
	 */
	bool (*get_temperature)( tempered_device*, int, float* );
	
	/** The method to use to get the relative humidity from this type of device.
	 */
	bool (*get_humidity)( tempered_device*, int, float* );
	
};

/** Find the temper_type struct that matches the given USB device information.
 * @param vendor_id The USB vendor ID to find a temper_type for.
 * @param product_id The USB product ID to find a temper_type for.
 * @param interface_number The USB interface number to find a temper_type for.
 * @return The matching type, or NULL if none was found.
 * Note that the returned type may have the ignored field set to true.
 */
struct temper_type* temper_type_find(
	unsigned short vendor_id, unsigned short product_id, int interface_number
);

/** Initialize the TEMPer types. */
bool temper_type_init( char **error );

/** Finalize the TEMPer types. */
bool temper_type_exit( char **error );

/** Enumerate the known TEMPer devices.
 *
 * This function returns a linked list of all the recognized TEMPer devices
 * attached to the system (excluding the ones that are ignored).
 *
 * @param error If an error occurs and this is not NULL, it will be set to the
 * error message. The returned string is dynamically allocated, and should be
 * freed when you're done with it.
 * @return A pointer to the first device in the enumerated list, or NULL on
 * error. This list should be freed with tempered_free_device_list when you
 * are done with it.
 */
struct tempered_device_list* temper_type_enumerate( char **error );

#endif

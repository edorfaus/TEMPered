#ifndef TEMPER_TYPE_H
#define TEMPER_TYPE_H

#include <stdbool.h>

#include "tempered.h"

/** This struct represents a subtype of a recognized device type.
 */
struct temper_subtype {
	/** The ID for this subtype, as retrieved by the get_subtype_id method.
	 */
	unsigned char id;
	
	/** The name of this device subtype.
	 */
	char *name;
	
	/** The method to use to open a device of this subtype.
	 * Note that this only does the subtype-specific parts; the type-specific
	 * parts are handled by the temper_type->open method.
	 * If no subtype-specific code is necessary, this can be NULL.
	 */
	bool (*open)( tempered_device* );
	
	/** The method to use to open a device of this subtype.
	 * Note that this only does the subtype-specific parts; the type-specific
	 * parts are handled by the temper_type->close method.
	 * If no subtype-specific code is necessary, this can be NULL.
	 */
	void (*close)( tempered_device* );
	
	/** The method to use to read the sensors on a device of this subtype.
	 */
	bool (*read_sensors)( tempered_device* );
	
	/** The method to use to get the sensor count for a device of this subtype.
	 */
	int (*get_sensor_count)( tempered_device* );
	
	/** The method to use to get the sensor type of a given sensor on a device
	 * of this subtype.
	 */
	int (*get_sensor_type)( tempered_device*, int );
	
	/** The method to use to get the temperature from a device of this subtype.
	 */
	bool (*get_temperature)( tempered_device*, int, float* );
	
	/** The method to use to get the relative humidity from this device subtype.
	 */
	bool (*get_humidity)( tempered_device*, int, float* );
};

/** This struct represents a type of recognized device, containing some useful
 * information on it that is used to recognize it and to read the temperature
 * from its sensor.
 */
struct temper_type {
	
	/** Name of device type, and end-of-list marker (this is NULL at EOL).
	 */
	char *name;
	
	/** USB Device Vendor ID for this type.
	 */
	unsigned short vendor_id;
	
	/** USB Device Product ID for this type.
	 */
	unsigned short product_id;
	
	/** USB Interface number for this type.
	 */
	int interface_number;
	
	/** The method to use to open a device of this type.
	 * If this is NULL, this entry represents a recognized but ignored type.
	 * (This is usually used for devices with multiple interfaces where some
	 * of the interfaces cannot be used to read the sensors).
	 */
	bool (*open)( tempered_device* );
	
	/** The method to use to close a device of this type.
	 */
	void (*close)( tempered_device* );
	
	/** The method to use to get the subtype ID from this kind of device.
	 */
	bool (*get_subtype_id)( tempered_device*, unsigned char* );
	
	/** Type specific data for getting the subtype off this kind of device.
	 */
	void *get_subtype_data;
	
	/** The list of subtypes for this device type.
	 */
	struct temper_subtype **subtypes;
	
};

/** Find the temper_type struct that matches the given USB device information.
 * @param vendor_id The USB vendor ID to find a temper_type for.
 * @param product_id The USB product ID to find a temper_type for.
 * @param interface_number The USB interface number to find a temper_type for.
 * @return The matching type, or NULL if none was found.
 * Note that the returned type may be an ignored type (no open method set).
 */
struct temper_type* temper_type_find(
	unsigned short vendor_id, unsigned short product_id, int interface_number
);

/** Find the subtype of the given temper_type that has the given subtype ID.
 * @param type The temper_type the subtype belongs to.
 * @param subtype_id The subtype ID of the subtype to find.
 * @return The subtype of the given type that has the given subtype ID, or NULL
 * if no such subtype was found.
 */
struct temper_subtype* temper_type_find_subtype(
	struct temper_type const * type, unsigned char subtype_id
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

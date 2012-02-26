#ifndef TEMPER_TYPE_H
#define TEMPER_TYPE_H

#include <stdbool.h>

#include "hidapi.h"

#include "tempered.h"

/** This struct represents a type of recognized device, containing some useful
 * information on it that is used to recognize it and to read the temperature
 * from its sensor.
 */
typedef struct temper_type {
	
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
	bool (*get_temperature)( tempered_device*, float* );
	
	/** The method to use to get the relative humidity from this type of device.
	 */
	bool (*get_humidity)( tempered_device*, float* );
	
} temper_type;

/** This is the array of known types, terminated by an element with a NULL name.
 */
extern temper_type known_temper_types[];

/** Get the temper_type struct that matches the given hid_device_info struct.
 * @param hid_device_info* info The HID device info to get the type for.
 * @return temper_type*|NULL The matching type, or NULL if none was found.
 * Note that the returned type may have the ignored field set to true.
 */
temper_type* get_temper_type( struct hid_device_info *info );

#endif


#ifndef TEMPER_TYPE_H
#define TEMPER_TYPE_H

#include <stdbool.h>

#include "hidapi.h"

#include "tempered.h"

/** The maximum length of the temperature report data.
 */
#define TEMPER_TYPE_TEMP_REPORT_MAX_LENGTH 8

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
	
	/** The method to use to open a device of this type.
	 */
	bool (*open)( tempered_device* );
	
	/** The method to use to close a device of this type.
	 */
	void (*close)( tempered_device* );
	
	/** The method to use to get the temperature from a device of this type.
	 */
	bool (*get_temperature)( tempered_device*, float* );
	
	/** Report to send to the device to read the temperature.
	 * Only valid if ignored is false.
	 */
	unsigned char temp_report[TEMPER_TYPE_TEMP_REPORT_MAX_LENGTH];
	
	/** Length in bytes of the temperature report (temp_report field).
	 * Only valid if ignored is false.
	 */
	int temp_report_length;
	
	/** Offset in the response containing the high byte of the temperature.
	 * Only valid if ignored is false.
	 */
	int temperature_high_byte_offset;
	
	/** Offset in the response containing the low byte of the temperature.
	 * Only valid if ignored is false.
	 */
	int temperature_low_byte_offset;
	
	/** Whether or not this device has a humidity sensor.
	 * Only valid if ignored is false.
	 */
	bool has_humidity;
	
	/** Offset in the response containing the high byte of the humidity.
	 * Only valid if ignored is false and has_humidity is true.
	 */
	int humidity_high_byte_offset;
	
	/** Offset in the response containing the low byte of the humidity.
	 * Only valid if ignored is false and has_humidity is true.
	 */
	int humidity_low_byte_offset;
	
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


#ifndef TEMPERED__TYPE_HID__TYPE_INFO_H
#define TEMPERED__TYPE_HID__TYPE_INFO_H

/** This file holds the type definitions that are used both internally by the
 * HID code, and as part of the temper_type list.
 */

#include "../temper_type.h"

/** This struct represents the data for a HID query to send to the device. */
struct tempered_type_hid_query
{
	/** How many bytes of data should be written to the device. */
	int length;
	
	/** The bytes of data that should be written to the device.
	 * Note that the first byte of this should be the HID report ID.
	 */
	unsigned char *data;
};

/** This struct stores the data that was read back after a given query. */
struct tempered_type_hid_query_result
{
	/** How many bytes of data was read from the device. */
	int length;
	
	/** The data that was read from the device. */
	unsigned char data[64];
};

/** This struct stores the data used to get the subtype ID from the device. */
struct tempered_type_hid_subtype_data
{
	/** The query to send to the device to get the subtype ID back. */
	struct tempered_type_hid_query query;
	
	/** The offset in the returned data that holds the subtype ID. */
	int id_offset;
};

/** This struct stores the data used to get the subtype ID for the device when
 * the device responds with a subtype string instead of a numeric ID.
 */
struct tempered_type_hid_subtype_from_string_data
{
	/** The query to send to the device to get the subtype string back. */
	struct tempered_type_hid_query query;
	
	/** The number of responses to read to get the complete subtype string. */
	int response_count;
	
	/** The strings that correspond to each subtype, as a NULL-terminated list.
	 * The offset into this list is used as the subtype ID.
	 */
	char **subtype_strings;
};

/** This struct represents a single sensor from a sensor group. */
struct tempered_type_hid_sensor
{
	/** The method used to get the temperature from the sensor group's data. */
	bool (*get_temperature)(
		tempered_device*, struct tempered_type_hid_sensor*,
		struct tempered_type_hid_query_result*, float*
	);
	
	/** The method used to get the humidity from the sensor group's data.
	 * This is NULL if the sensor does not support humidity.
	 */
	bool (*get_humidity)(
		tempered_device*, struct tempered_type_hid_sensor*,
		struct tempered_type_hid_query_result*, float*
	);
	
	/** The offset in the group data that holds the high byte of the
	 * temperature value.
	 */
	int temperature_high_byte_offset;
	
	/** The offset in the group data that holds the low byte of the
	 * temperature value.
	 */
	int temperature_low_byte_offset;
	
	/** The offset in the group data that holds the high byte of the
	 * humidity value.
	 */
	int humidity_high_byte_offset;
	
	/** The offset in the group data that holds the low byte of the
	 * humidity value.
	 */
	int humidity_low_byte_offset;
};

/** This struct represents a group of sensors that are read simultaneously. */
struct tempered_type_hid_sensor_group
{
	/** The HID query that is used to get the data for this sensor group. */
	struct tempered_type_hid_query query;
	
	/** The method that is used to read the sensors for this sensor group. */
	bool (*read_sensors)(
		tempered_device*, struct tempered_type_hid_sensor_group*,
		struct tempered_type_hid_query_result*
	);
	
	/** The number of sensors that are in this group. */
	int sensor_count;
	
	/** The array of sensor_count sensors that are in this group. */
	struct tempered_type_hid_sensor *sensors;
};

/** This struct represents a HID temper subtype. */
struct temper_subtype_hid
{
	/** The base temper_subtype holding the common subtype data. */
	struct temper_subtype base;
	
	/** The number of sensor groups that are in this subtype. */
	int sensor_group_count;
	
	/** The array of sensor_group_count sensor groups that are in this subtype.
	 */
	struct tempered_type_hid_sensor_group *sensor_groups;
};

#endif

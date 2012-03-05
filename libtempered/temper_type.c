#include <stdlib.h>
#include <stdbool.h>

#include "temper_type.h"

#include "type_hid/common.h"
#include "type_hid/1130_660c.h"

// This is an array of known TEMPer types.
temper_type known_temper_types[]={
	{
		.name="TEMPer2HumiV1.x",
		.ignored=true,
		.vendor_id=0x0c45,
		.product_id=0x7402,
		.interface_number=0
	},
	{
		.name="TEMPer2HumiV1.x",
		.ignored=false,
		.vendor_id=0x0c45,
		.product_id=0x7402,
		.interface_number=1,
		.open = temper_type_hid_open,
		.close = temper_type_hid_close,
		.read_sensors = temper_type_hid_read_sensors,
		.get_temperature = temper_type_hid_get_temperature,
		.get_humidity = temper_type_hid_get_humidity,
		.data = &(struct temper_type_hid_data){
			.report_length = 8,
			.report_data = { 1, 0x80, 0x33, 1, 0, 0, 0, 0 },
			.temperature_high_byte_offset = 2,
			.temperature_low_byte_offset = 3,
			.has_humidity = true,
			.humidity_high_byte_offset = 4,
			.humidity_low_byte_offset = 5
		}
	},
	{
		.name="TEMPerV1.2",
		.ignored=true,
		.vendor_id=0x0c45,
		.product_id=0x7401,
		.interface_number=0
	},
	{
		.name="TEMPerV1.2",
		.ignored=false,
		.vendor_id=0x0c45,
		.product_id=0x7401,
		.interface_number=1,
		.open = temper_type_hid_open,
		.close = temper_type_hid_close,
		.read_sensors = temper_type_hid_read_sensors,
		.get_temperature = temper_type_hid_get_temperature,
		.data = &(struct temper_type_hid_data){
			.report_length = 8,
			.report_data = { 1, 0x80, 0x33, 1, 0, 0, 0, 0 },
			.temperature_high_byte_offset = 2,
			.temperature_low_byte_offset = 3
		},
	},
	{
		.name="HidTEMPer1, 2, NTC or HUM",
		.ignored=true,
		.vendor_id=0x1130,
		.product_id=0x660c,
		.interface_number=0
	},
	{
		.name="HidTEMPer1, 2, NTC or HUM (experimental)",
		.ignored=false,
		.vendor_id=0x1130,
		.product_id=0x660c,
		.interface_number=1,
		.open = temper_type_hid_1130_660c_open,
		.close = temper_type_hid_close,
		.read_sensors = temper_type_hid_read_sensors,
		.get_temperature = temper_type_hid_get_temperature,
		.get_humidity = temper_type_hid_get_humidity,
		.data = &(struct temper_type_hid_1130_660c_data[]){
			{
				.id = 0x58,
				.name = "HidTEMPer1 (experimental)",
				.data = {
					.report_length = 8,
					.report_data = { 0x54, 0, 0, 0, 0, 0, 0, 0 },
					.temperature_high_byte_offset = 0,
					.temperature_low_byte_offset = 1
				}
			},
			{
				.id = 0x59,
				.name = "HidTEMPer2 (experimental)",
				// Note: this thing has 2 sensors, internal and external
				// The internal sensor uses 0x54, the external 0x53...
				.data = {
					.report_length = 8,
					.report_data = { 0x54, 0, 0, 0, 0, 0, 0, 0 },
					.temperature_high_byte_offset = 0,
					.temperature_low_byte_offset = 1
				}
			},
			{
				.id = 0x5b,
				.name = "HidTEMPerNTC (experimental)",
				// Note: this thing has 2 sensors, internal and external
				// The internal sensor uses 0x54, and is the usual FM75 type
				// The external sensor uses 0x41, and is completely different.
				.data = {
					.report_length = 8,
					.report_data = { 0x54, 0, 0, 0, 0, 0, 0, 0 },
					.temperature_high_byte_offset = 0,
					.temperature_low_byte_offset = 1
				}
			},
			/*
			{
				.id = 0x??,
				.name = "HidTEMPerHUM (experimental)",
				.data = {
					.report_length = 8,
					.report_data = { 0x48, 0, 0, 0, 0, 0, 0, 0 },
					.temperature_high_byte_offset = 0,
					.temperature_low_byte_offset = 1,
					.has_humidity = true,
					.humidity_high_byte_offset = 2,
					.humidity_low_byte_offset = 3
				}
			},
			*/
			{ .name = NULL } // Subtype list terminator
		}
	},
	{ .name=NULL } // List terminator
};

// Get the temper_type that matches the given USB device information
temper_type* temper_type_find(
	unsigned short vendor_id, unsigned short product_id, int interface_number
) {
	temper_type *type;
	for ( type = known_temper_types; type->name != NULL; type++ )
	{
		if (
			type->vendor_id == vendor_id &&
			type->product_id == product_id &&
			type->interface_number == interface_number
		) {
			return type;
		}
	}
	return NULL;
}

/** Initialize the TEMPer types. */
bool temper_type_init( char **error )
{
	return temper_type_hid_init( error );
}

/** Finalize the TEMPer types. */
bool temper_type_exit( char **error )
{
	return temper_type_hid_exit( error );
}

/** Enumerate the known TEMPer devices. */
struct tempered_device_list* temper_type_enumerate( char **error )
{
	return temper_type_hid_enumerate( error );
}

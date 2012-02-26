#include <stdlib.h>
#include <stdbool.h>

#include "temper_type.h"

#include "temper_type_hid.h"

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
		.get_temperature = temper_type_hid_get_temperature,
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
		.get_temperature = temper_type_hid_get_temperature,
		.data = &(struct temper_type_hid_data){
			.report_length = 8,
			.report_data = { 1, 0x80, 0x33, 1, 0, 0, 0, 0 },
			.temperature_high_byte_offset = 2,
			.temperature_low_byte_offset = 3
		},
	},
	{
		.name="TEMPer, TEMPerNTC or TEMPerHUM",
		.ignored=true,
		.vendor_id=0x1130,
		.product_id=0x660c,
		.interface_number=0
	},
	{
		.name="TEMPer (experimental)",
		.ignored=false,
		.vendor_id=0x1130,
		.product_id=0x660c,
		.interface_number=1,
		.open = temper_type_hid_open,
		.close = temper_type_hid_close,
		.get_temperature = temper_type_hid_get_temperature,
		.data = &(struct temper_type_hid_data){
			.report_length = 8,
			.report_data = { 0x54, 0, 0, 0, 0, 0, 0, 0 },
			.temperature_high_byte_offset = 0,
			.temperature_low_byte_offset = 1
		}
	},
	{
		.name="TEMPerHUM (experimental)",
		.ignored=false,
		.vendor_id=0x1130,
		.product_id=0x660c,
		.interface_number=1,
		.open = temper_type_hid_open,
		.close = temper_type_hid_close,
		.get_temperature = temper_type_hid_get_temperature,
		.data = &(struct temper_type_hid_data){
			.report_length = 8,
			.report_data = { 0x48, 0, 0, 0, 0, 0, 0, 0 },
			.temperature_high_byte_offset = 0,
			.temperature_low_byte_offset = 1,
			.has_humidity = true,
			.humidity_high_byte_offset = 2,
			.humidity_low_byte_offset = 3
		}
	},
	{ .name=NULL } // List terminator
};

// Get the temper_type that matches the given hid_device_info
temper_type* get_temper_type( struct hid_device_info *info )
{
	temper_type *type;
	for ( type = known_temper_types; type->name != NULL; type++ )
	{
		if (
			type->vendor_id == info->vendor_id &&
			type->product_id == info->product_id &&
			type->interface_number == info->interface_number
		)
		{
			return type;
		}
	}
	return NULL;
}

#include <stdbool.h>
#include "temper_type.h"

// This is an array of known TEMPer types.
temper_type known_temper_types[]={
	{
		.name="TEMPer2HumiV1.0",
		.ignored=true,
		.vendor_id=0x0c45,
		.product_id=0x7402,
		.interface_number=0
	},
	{
		.name="TEMPer2HumiV1.0",
		.ignored=false,
		.vendor_id=0x0c45,
		.product_id=0x7402,
		.interface_number=1,
		.temp_report_length=8,
		.temp_report={1,0x80,0x33,1,0,0,0,0},
		.high_byte_offset=2,
		.low_byte_offset=3
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
		.temp_report_length=8,
		.temp_report={1,0x80,0x33,1,0,0,0,0},
		.high_byte_offset=2,
		.low_byte_offset=3
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


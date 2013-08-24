#include <stdlib.h>
#include <stdbool.h>

#include "temper_type.h"

#include "tempered.h"
#include "type_hid/type-info.h"
#include "type_hid/common.h"
#include "type_hid/fm75.h"
#include "type_hid/sht1x.h"
#include "type_hid/ntc.h"
#include "type_hid/si7005.h"

// This is an array of known TEMPer types.
struct temper_type known_temper_types[]={
	{
		.name="TEMPer2HumiV1.x",
		.vendor_id=0x0c45,
		.product_id=0x7402,
		.interface_number=0
	},
	{
		.name="TEMPer2HumiV1.x",
		.vendor_id=0x0c45,
		.product_id=0x7402,
		.interface_number=1,
		.open = tempered_type_hid_open,
		.close = tempered_type_hid_close,
		.get_subtype_id = tempered_type_hid_get_subtype_id_from_string,
		.get_subtype_data = &(struct tempered_type_hid_subtype_from_string_data)
		{
			.query = {
				.length = 9,
				.data = (unsigned char[]){ 0, 1, 0x86, 0xFF, 1, 0, 0, 0, 0 }
			},
			.response_count = 2,
			.subtype_strings = (char *[]){
				"TEMPerHumV1.0rHu",
				"TEMPerHumM12V1.0",
				NULL
			}
		},
		.subtypes = (struct temper_subtype*[]){
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 0,
					.name = "TEMPer2HumiV1.x",
					.open = tempered_type_hid_subtype_open,
					.read_sensors = tempered_type_hid_read_sensors,
					.get_temperature = tempered_type_hid_get_temperature,
					.get_humidity = tempered_type_hid_get_humidity
				},
				.sensor_group_count = 1,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{
						.query = {
							.length = 9,
							.data = (unsigned char[]){ 0, 1, 0x80, 0x33, 1, 0, 0, 0, 0 }
						},
						.read_sensors = tempered_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_sht1x,
								.get_humidity = tempered_type_hid_get_humidity_sht1x,
								.temperature_high_byte_offset = 2,
								.temperature_low_byte_offset = 3,
								.humidity_high_byte_offset = 4,
								.humidity_low_byte_offset = 5
							}
						}
					}
				}
			},
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 1,
					.name = "TEMPerHumM12V1.0",
					.open = tempered_type_hid_subtype_open,
					.read_sensors = tempered_type_hid_read_sensors,
					.get_temperature = tempered_type_hid_get_temperature,
					.get_humidity = tempered_type_hid_get_humidity
				},
				.sensor_group_count = 1,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{
						.query = {
							.length = 9,
							.data = (unsigned char[]){ 0, 1, 0x80, 0x33, 1, 0, 0, 0, 0 }
						},
						.read_sensors = tempered_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_si7005,
								.get_humidity = tempered_type_hid_get_humidity_si7005,
								.temperature_high_byte_offset = 2,
								.temperature_low_byte_offset = 3,
								.humidity_high_byte_offset = 4,
								.humidity_low_byte_offset = 5
							}
						}
					}
				}
			},
			NULL // List terminator for subtypes
		}
	},
	{
		.name="TEMPerV1.2 or TEMPer2V1.3",
		.vendor_id=0x0c45,
		.product_id=0x7401,
		.interface_number=0
	},
	{
		.name="TEMPerV1.2 or TEMPer2V1.3",
		.vendor_id=0x0c45,
		.product_id=0x7401,
		.interface_number=1,
		.open = tempered_type_hid_open,
		.close = tempered_type_hid_close,
		.get_subtype_id = tempered_type_hid_get_subtype_id,
		.get_subtype_data =  &(struct tempered_type_hid_subtype_data){
			.id_offset = 1,
			.query = {
				.length = 9,
				.data = (unsigned char[]){ 0, 1, 0x82, 0x77, 1, 0, 0, 0, 0 }
			}
			// Technically I think offset 1 says how many bytes of data follow,
			// but it is 1 for TEMPer and 2 for TEMPer2, so it's usable as ID.
			// TODO: we may want to use the 82 FF query for initialization.
		},
		.subtypes = (struct temper_subtype*[]){
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 1,
					.name = "TEMPerV1.2",
					.open = tempered_type_hid_subtype_open,
					.read_sensors = tempered_type_hid_read_sensors,
					.get_temperature = tempered_type_hid_get_temperature,
				},
				.sensor_group_count = 1,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{
						.query = {
							.length = 9,
							.data = (unsigned char[]){ 0, 1, 0x80, 0x33, 1, 0, 0, 0, 0 }
						},
						.read_sensors = tempered_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 2,
								.temperature_low_byte_offset = 3
							}
						}
					}
				}
			},
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 2,
					.name = "TEMPer2V1.3",
					.open = tempered_type_hid_subtype_open,
					.read_sensors = tempered_type_hid_read_sensors,
					.get_sensor_count = tempered_type_hid_get_sensor_count,
					.get_temperature = tempered_type_hid_get_temperature,
				},
				.sensor_group_count = 1,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{
						.query = {
							.length = 9,
							.data = (unsigned char[]){ 0, 1, 0x80, 0x33, 1, 0, 0, 0, 0 }
						},
						.read_sensors = tempered_type_hid_read_sensor_group,
						.sensor_count = 2,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 2,
								.temperature_low_byte_offset = 3
							},
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 4,
								.temperature_low_byte_offset = 5
							}
						}
					}
				}
			},
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 3,
					.name = "TEMPerNTC1.0",
					.open = tempered_type_hid_subtype_open,
					.read_sensors = tempered_type_hid_read_sensors,
					.get_sensor_count = tempered_type_hid_get_sensor_count,
					.get_temperature = tempered_type_hid_get_temperature,
				},
				.sensor_group_count = 1,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{
						.query = {
							.length = 9,
							.data = (unsigned char[]){ 0, 1, 0x80, 0x33, 1, 0, 0, 0, 0 }
						},
						.read_sensors = tempered_type_hid_read_sensor_group,
						.sensor_count = 3,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 2,
								.temperature_low_byte_offset = 3
							},
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 4,
								.temperature_low_byte_offset = 5
							},
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 6,
								.temperature_low_byte_offset = 7
							}
						}
					}
				}
			},
			NULL // List terminator for subtypes
		}
	},
	{
		.name="HidTEMPer1, 2, NTC or HUM",
		.vendor_id=0x1130,
		.product_id=0x660c,
		.interface_number=0
	},
	{
		.name="HidTEMPer1, 2, NTC or HUM",
		.vendor_id=0x1130,
		.product_id=0x660c,
		.interface_number=1,
		.open = tempered_type_hid_open,
		.close = tempered_type_hid_close,
		.get_subtype_id = tempered_type_hid_get_subtype_id,
		.get_subtype_data = &(struct tempered_type_hid_subtype_data){
			.id_offset = 2,
			.query = {
				.length = 9,
				.data = (unsigned char[]){ 0, 0x52, 0, 0, 0, 0, 0, 0, 0 }
			}
			// calibration is in offsets 2,3 for internal and 4,5 for external
			// sensor, at least for some types - but I don't know how to use it
		},
		.subtypes = (struct temper_subtype*[]){
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 0x58,
					// TODO: use .open to initialize the high-resolution data
					// (by sending 0x43 to the device)
					.open = tempered_type_hid_subtype_open,
					.name = "HidTEMPer1 (experimental)",
					.read_sensors = tempered_type_hid_read_sensors,
					.get_temperature = tempered_type_hid_get_temperature
				},
				.sensor_group_count = 1,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{
						.query = {
							.length = 9,
							.data = (unsigned char[]){ 0, 0x54, 0, 0, 0, 0, 0, 0, 0 }
						},
						.read_sensors = tempered_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 0,
								.temperature_low_byte_offset = 1
							}
						}
					}
				}
			},
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 0x59,
					.name = "HidTEMPer2 (experimental)",
					.open = tempered_type_hid_subtype_open,
					.read_sensors = tempered_type_hid_read_sensors,
					.get_sensor_count = tempered_type_hid_get_sensor_count,
					.get_temperature = tempered_type_hid_get_temperature
				},
				.sensor_group_count = 2,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{ // Internal sensor
						.query = {
							.length = 9,
							.data = (unsigned char[]){ 0, 0x54, 0, 0, 0, 0, 0, 0, 0 }
						},
						.read_sensors = tempered_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 0,
								.temperature_low_byte_offset = 1
							}
						}
					},
					{ // External sensor
						.query = {
							.length = 9,
							.data = (unsigned char[]){ 0, 0x53, 0, 0, 0, 0, 0, 0, 0 }
						},
						.read_sensors = tempered_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 0,
								.temperature_low_byte_offset = 1
							}
						}
					}
				}
			},
			/*
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 0x00, // TODO: find the ID
					.name = "HidTEMPerHUM (experimental)",
					.open = tempered_type_hid_subtype_open,
					.read_sensors = tempered_type_hid_read_sensors,
					.get_temperature = tempered_type_hid_get_temperature,
					.get_humidity = tempered_type_hid_get_humidity
				},
				.sensor_group_count = 1,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{
						.query = {
							.length = 9,
							.data = (unsigned char[]){ 0, 0x48, 0, 0, 0, 0, 0, 0, 0 }
						},
						.read_sensors = tempered_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_sht1x,
								.get_humidity = tempered_type_hid_get_humidity_sht1x,
								.temperature_high_byte_offset = 0,
								.temperature_low_byte_offset = 1
								.humidity_high_byte_offset = 2,
								.humidity_low_byte_offset = 3
							}
						}
					}
				}
			},
			*/
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 0x5b,
					.name = "HidTEMPerNTC (experimental)",
					.open = tempered_type_hid_subtype_open,
					.read_sensors = tempered_type_hid_read_sensors,
					.get_sensor_count = tempered_type_hid_get_sensor_count,
					.get_temperature = tempered_type_hid_get_temperature
				},
				.sensor_group_count = 2,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{ // Internal sensor
						.query = {
							.length = 9,
							.data = (unsigned char[]){ 0, 0x54, 0, 0, 0, 0, 0, 0, 0 }
						},
						.read_sensors = tempered_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 0,
								.temperature_low_byte_offset = 1
							}
						}
					},
					{ // External sensor
						.query = {
							.length = 9,
							.data = (unsigned char[]){ 0, 0x41, 0, 0, 0, 0, 0, 0, 0 }
						},
						.read_sensors = tempered_type_hid_read_sensor_group_ntc,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_ntc,
								.temperature_high_byte_offset = 0,
								.temperature_low_byte_offset = 1,
							}
						}
					}
				}
			},
			NULL // List terminator for subtypes
		}
	},
	{ .name=NULL } // List terminator for temper types
};

// Get the temper_type that matches the given USB device information
struct temper_type* temper_type_find(
	unsigned short vendor_id, unsigned short product_id, int interface_number
) {
	struct temper_type *type;
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

// Find the subtype of the given temper_type that has the given subtype ID.
struct temper_subtype* temper_type_find_subtype(
	struct temper_type const * type, unsigned char subtype_id
) {
	int i = 0;
	while ( type->subtypes[i] != NULL && type->subtypes[i]->id != subtype_id )
	{
		i++;
	}
	return type->subtypes[i];
}

/** Initialize the TEMPer types. */
bool temper_type_init( char **error )
{
	return tempered_type_hid_init( error );
}

/** Finalize the TEMPer types. */
bool temper_type_exit( char **error )
{
	return tempered_type_hid_exit( error );
}

/** Enumerate the known TEMPer devices. */
struct tempered_device_list* temper_type_enumerate( char **error )
{
	return tempered_type_hid_enumerate( error );
}

#ifndef TEMPERED_H
#define TEMPERED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/** This file contains the headers that comprise the public API of the TEMPered
 * library. The functions and types given here are the only ones that should be
 * used by the programs that use this library, as any others are considered
 * internal implementation details and are subject to change without notice.
 */


/** No such sensor, or it doesn't support any of the types we know. */
#define TEMPERED_SENSOR_TYPE_NONE        (0     )

/** The sensor supports reading the temperature. */
#define TEMPERED_SENSOR_TYPE_TEMPERATURE (1 << 0)

/** The sensor supports reading the relative humidity. */
#define TEMPERED_SENSOR_TYPE_HUMIDITY    (1 << 1)


/** This struct represents a linked list of enumerated TEMPer devices.
 * @see tempered_enumerate()
 */
struct tempered_device_list {
	/** Pointer to the next device in the list, or NULL if this is the last
	 * device in the list.
	 */
	struct tempered_device_list *next;
	
	/** Device path for this device.
	 */
	char *path;
	
	/** Name of the device type.
	 */
	char *type_name;
	
	/** USB Device Vendor ID for this device.
	 */
	unsigned short vendor_id;
	
	/** USB Device Product ID for this device.
	 */
	unsigned short product_id;
	
	/** USB Interface number for this device.
	 */
	int interface_number;
};

struct tempered_device_;

/** This type represents an opened TEMPer device.
 *
 * This is an opaque type.
 * @see tempered_open()
 */
typedef struct tempered_device_ tempered_device;

/** Initialize the TEMPered library.
 *
 * This function initializes the TEMPered library. Calling it is not strictly
 * necessary, as it will be called automatically when needed, but should be
 * called at the start of execution if there's a chance the library will be
 * used by multiple threads simultaneously.
 * @param error If an error occurs and this is not NULL, it will be set to the
 * error message. The returned string is dynamically allocated, and should be
 * freed when you're done with it.
 * @return true on success, false on error.
 */
bool tempered_init( char **error );

/** Finalize the TEMPered library.
 *
 * This function should be called at the end of execution to avoid memory leaks.
 * @param error If an error occurs and this is not NULL, it will be set to the
 * error message. The returned string is dynamically allocated, and should be
 * freed when you're done with it.
 * @return true on success, false on error.
 */
bool tempered_exit( char **error );

/** Enumerate the TEMPer devices.
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
 * If no devices were found, NULL is returned and the error remains unset.
 */
struct tempered_device_list* tempered_enumerate( char **error );

/** Free the memory used by the given device list.
 *
 * Once this method has been called with a list, the given list should not be
 * dereferenced.
 *
 * @param list The device list to be freed. Can be NULL to not free anything.
 */
void tempered_free_device_list( struct tempered_device_list *list );

/** Open a given device from the device list.
 *
 * The returned handle should be closed with tempered_close() when you are done
 * using the device.
 * @param list The device list entry that should be opened.
 * @param error If an error occurs and this is not NULL, it will be set to the
 * error message. The returned string is dynamically allocated, and should be
 * freed when you're done with it.
 * @return The opened device, or NULL on error.
 * @see tempered_close()
 * @see tempered_enumerate()
 */
tempered_device* tempered_open( struct tempered_device_list *list, char **error );

/** Close an open device.
 *
 * Once a device handle has been closed, it should no longer be used.
 * @param device The device handle to close.
 * @see tempered_open()
 */
void tempered_close( tempered_device *device );

/** Get the last error message for the given device.
 * @param device The device for which to get the last error message.
 * @return The last error message for the given device, or NULL if no error has
 * occurred on that device.
 *
 * The returned string must not be freed or modified, and is only valid as long
 * as the device handle is open and no further error has occurred on it.
 */
char* tempered_error( tempered_device *device );

/** Get the number of sensors supported by the given device.
 * @param device The device to get the sensor count of.
 * @return The number of sensors this device currently has.
 */
int tempered_get_sensor_count( tempered_device *device );

/** Get the sensor type of the given sensor.
 * @param device The device the sensor belongs to.
 * @param sensor The ID of the sensor to get the type of.
 * The ID is a number ( 0 <= ID < sensor_count ) that identifies the sensor.
 * @return A bitmask that identifies the type of sensor this is. This is made
 * up of the TEMPERED_SENSOR_TYPE_* constants.
 */
int tempered_get_sensor_type( tempered_device *device, int sensor );

/** Read the sensors of the given device.
 *
 * This should be called when you want to update the sensor values
 * (temperature, humidity) that is returned by the other methods.
 * @param device The device to read the sensors of.
 * @return Whether or not the sensors were successfully read.
 */
bool tempered_read_sensors( tempered_device *device );

/** Get the temperature from the given device.
 *
 * Note that to get up-to-date values you must first call tempered_read_sensors.
 * @param device The device to get the temperature from.
 * @param sensor The ID of the sensor to get the temperature of.
 * @param tempC A pointer to a float where the temperature will be stored.
 * @return Whether or not the temperature was successfully retrieved.
 */
bool tempered_get_temperature(
	tempered_device *device, int sensor, float *tempC
);

/** Get the relative humidity from the given device.
 *
 * Note that to get up-to-date values you must first call tempered_read_sensors.
 * @param device The device to get the humidity from.
 * @param sensor The ID of the sensor to get the relative humidity of.
 * @param rel_hum A pointer to a float where the humidity will be stored.
 * @return Whether or not the humidity was successfully retrieved.
 */
bool tempered_get_humidity(
	tempered_device *device, int sensor, float *rel_hum
);

/** Get the device path of the given device.
 * @param device The device to get the type name of.
 * @return The device path of the given device.
 *
 * The returned string must not be freed or modified,
 * and is only valid as long as the device handle is open.
 */
char const * tempered_get_device_path( tempered_device *device );

/** Get the type name of the given device.
 * @param device The device to get the type name of.
 * @return The type name of the given device, or NULL on error.
 *
 * The returned type name must not be freed or modified,
 * and is only valid as long as the device handle is open.
 */
char const * tempered_get_type_name( tempered_device *device );

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif

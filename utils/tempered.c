#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tempered.h>
#include <getopt.h>
#include <tempered-util.h>

struct my_options {
	bool enumerate;
	struct tempered_util__temp_scale const * temp_scale;
	int calibration_count;
	float * calibration_values;
	char ** devices;
};

void free_options( struct my_options *options )
{
	free( options->calibration_values );
	// Entries of options->devices are straight from argv, so don't free() them.
	free( options->devices );
	// options->temp_scale is not allocated on the heap.
	free( options );
}

void show_help()
{
	printf(
"Usage: tempered [options] [device-path...]\n"
"\n"
"Known options:\n"
"    -h\n"
"    --help                 Show this help text\n"
"    -e\n"
"    --enumerate            Enumerate the found devices without reading them.\n"
"    -s <scale>\n"
"    --scale <scale>        Set the temperature scale to show measurements in.\n"
"                           The <scale> can be the name or symbol of the scale,\n"
"                           or a unique prefix of the name.\n"
"                           Known scales: "
	);
	int pos = 14; // strlen( "Known scales: " );
	bool need_comma = false;
	struct tempered_util__temp_scale const * cur;
	for ( cur = tempered_util__known_temp_scales ; cur->name != NULL ; cur++ )
	{
		int len = strlen( cur->name );
		if ( pos + len + 1 > 50 ) // 50 is the width of the text area
		{
			printf(
				"%s\n                           ",
				( need_comma ? "," : "" )
			);
			need_comma = false;
			pos = 0;
		}
		printf( "%s%s", ( need_comma ? ", " : "" ), cur->name );
		pos += len + ( need_comma ? 2 : 0 );
		need_comma = true;
	}
	printf( "\n"
"    -c <cal>\n"
"    --calibrate-temp <cal> Calibrate the measured temperature using the given\n"
"                           calibration parameters. <cal> is a colon-separated\n"
"                           list of floats, where each one given represents the\n"
"                           factor for that power of the measured temperature,\n"
"                           starting at power zero. ( a+b*T+c*T^2+d*T^3 ... )\n"
	);
}

struct my_options* parse_options( int argc, char *argv[] )
{
	struct my_options options = {
		.enumerate = false,
		.temp_scale = NULL,
		.calibration_count = 0,
		.calibration_values = NULL,
		.devices = NULL,
	};
	char *temp_scale = "Celsius", *calibration_string = NULL;
	struct option const long_options[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "enumerate", no_argument, NULL, 'e' },
		{ "scale", required_argument, NULL, 's' },
		{ "calibrate-temp", required_argument, NULL, 'c' },
		{ NULL, 0, NULL, 0 }
	};
	char const * const short_options = "hes:c:";
	while ( true )
	{
		int opt = getopt_long( argc, argv, short_options, long_options, NULL );
		if ( opt == -1 )
		{
			break;
		}
		switch ( opt )
		{
			case 0:// This should never happen since all options have flag==NULL
			default:
			{
				fprintf( stderr, "Error: invalid option found." );
				return NULL;
			} break;
			case '?':
			{
				// getopt_long has already printed an error message.
				return NULL;
			} break;
			case 'h':
			{
				show_help();
				return NULL;
			} break;
			case 'e':
			{
				options.enumerate = true;
			} break;
			case 's':
			{
				temp_scale = optarg;
			} break;
			case 'c':
			{
				calibration_string = optarg;
			} break;
		}
	}
	options.temp_scale = tempered_util__find_temperature_scale( temp_scale );
	if ( options.temp_scale == NULL )
	{
		fprintf( stderr, "Temperature scale not found: %s\n", temp_scale );
		return NULL;
	}
	if ( calibration_string != NULL )
	{
		options.calibration_values = tempered_util__parse_calibration_string(
			calibration_string, &(options.calibration_count), true
		);
		if ( options.calibration_values == NULL )
		{
			// It has already printed an error message.
			return NULL;
		}
	}
	if ( optind < argc )
	{
		int count = argc - optind;
		char **devices = calloc( count + 1, sizeof( char* ) );
		memcpy( devices, &(argv[optind]), count * sizeof( char* ) );
		options.devices = devices;
	}
	struct my_options * heap_options = malloc( sizeof( struct my_options ) );
	memcpy( heap_options, &options, sizeof( struct my_options ) );
	return heap_options;
}

/** Get and print the sensor values for a given device and sensor. */
void print_device_sensor(
	tempered_device *device, int sensor, struct my_options *options
) {
	float tempC, rel_hum;
	int type = tempered_get_sensor_type( device, sensor );
	if ( type & TEMPERED_SENSOR_TYPE_TEMPERATURE )
	{
		if ( !tempered_get_temperature( device, sensor, &tempC ) )
		{
			fprintf(
				stderr, "%s %i: Failed to get the temperature: %s\n",
				tempered_get_device_path( device ), sensor,
				tempered_error( device )
			);
			type &= ~TEMPERED_SENSOR_TYPE_TEMPERATURE;
		}
		else if ( options->calibration_values != NULL )
		{
			tempC = tempered_util__calibrate_value(
				tempC, options->calibration_count, options->calibration_values
			);
		}
	}
	if ( type & TEMPERED_SENSOR_TYPE_HUMIDITY )
	{
		if ( !tempered_get_humidity( device, sensor, &rel_hum ) )
		{
			fprintf(
				stderr, "%s %i: Failed to get the humidity: %s\n",
				tempered_get_device_path( device ), sensor,
				tempered_error( device )
			);
			type &= ~TEMPERED_SENSOR_TYPE_HUMIDITY;
		}
	}
	if (
		( type & TEMPERED_SENSOR_TYPE_TEMPERATURE ) &&
		( type & TEMPERED_SENSOR_TYPE_HUMIDITY )
	) {
		printf(
			"%s %i: temperature %.2f %s"
				", relative humidity %.1f%%"
				", dew point %.1f %s\n",
			tempered_get_device_path( device ), sensor,
			options->temp_scale->from_celsius( tempC ),
			options->temp_scale->symbol,
			rel_hum,
			options->temp_scale->from_celsius(
				tempered_util__get_dew_point( tempC, rel_hum )
			),
			options->temp_scale->symbol
		);
	}
	else if ( type & TEMPERED_SENSOR_TYPE_TEMPERATURE )
	{
		printf(
			"%s %i: temperature %.2f %s\n",
			tempered_get_device_path( device ), sensor,
			options->temp_scale->from_celsius( tempC ),
			options->temp_scale->symbol
		);
	}
	else if ( type & TEMPERED_SENSOR_TYPE_HUMIDITY )
	{
		printf(
			"%s %i: relative humidity %.1f%%\n",
			tempered_get_device_path( device ), sensor,
			rel_hum
		);
	}
	else
	{
		printf(
			"%s %i: no sensor data available\n",
			tempered_get_device_path( device ), sensor
		);
	}
}

/** Print the sensor values for a given device. */
void print_device(
	struct tempered_device_list *dev, struct my_options *options
) {
	if ( options->enumerate )
	{
		printf(
			"%s : %s (USB IDs %04X:%04X)\n",
			dev->path, dev->type_name, dev->vendor_id, dev->product_id
		);
		return;
	}
	char *error = NULL;
	tempered_device *device = tempered_open( dev, &error );
	if ( device == NULL )
	{
		fprintf(
			stderr, "%s: Could not open device: %s\n",
			dev->path, error
		);
		free( error );
		return;
	}
	if ( !tempered_read_sensors( device ) )
	{
		fprintf(
			stderr, "%s: Failed to read the sensors: %s\n",
			tempered_get_device_path( device ),
			tempered_error( device )
		);
	}
	else
	{
		int sensor, sensors = tempered_get_sensor_count( device );
		for ( sensor = 0; sensor < sensors; sensor++ )
		{
			print_device_sensor( device, sensor, options );
		}
	}
	tempered_close( device );
}

int main( int argc, char *argv[] )
{
	struct my_options *options = parse_options( argc, argv );
	if ( options == NULL )
	{
		return 1;
	}
	char *error = NULL;
	if ( !tempered_init( &error ) )
	{
		fprintf( stderr, "Failed to initialize libtempered: %s\n", error );
		free( error );
		free_options( options );
		return 1;
	}
	
	struct tempered_device_list *list = tempered_enumerate( &error );
	if ( list == NULL )
	{
		fprintf( stderr, "Failed to enumerate devices: %s\n", error );
		free( error );
	}
	else
	{
		if ( options->devices != NULL )
		{
			// We have parameters, so only print those devices that are given.
			int i;
			for ( i = 0; options->devices[i] != NULL ; i++ )
			{
				bool found = false;
				struct tempered_device_list *dev;
				for ( dev = list ; dev != NULL ; dev = dev->next )
				{
					if ( strcmp( dev->path, options->devices[i] ) == 0 )
					{
						found = true;
						print_device( dev, options );
						break;
					}
				}
				if ( !found )
				{
					fprintf(
						stderr, "%s: TEMPered device not found or ignored.\n",
						options->devices[i]
					);
				}
			}
		}
		else
		{
			// We don't have any parameters, so print all the devices we found.
			struct tempered_device_list *dev;
			for ( dev = list ; dev != NULL ; dev = dev->next )
			{
				print_device( dev, options );
			}
		}
		tempered_free_device_list( list );
	}
	
	if ( !tempered_exit( &error ) )
	{
		fprintf( stderr, "%s\n", error );
		free( error );
		free_options( options );
		return 1;
	}
	return 0;
}

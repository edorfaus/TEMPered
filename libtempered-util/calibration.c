#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

float* tempered_util__parse_calibration_string(
	char const * string, int *found_count, bool print_errors
) {
	if ( string == NULL || found_count == NULL )
	{
		if ( found_count != NULL )
		{
			*found_count = -1;
		}
		if ( print_errors )
		{
			fprintf(
				stderr, "parse_calibration_string: Invalid parameters.\n"
			);
		}
		return NULL;
	}
	int count = 1, i;
	for ( i = 0 ; string[i] != '\0' ; i++ )
	{
		if ( string[i] == ':' )
		{
			count++;
		}
	}
	float values[count];
	char const *startptr = string;
	char *endptr;
	for ( i = 0 ; i < count ; i++ )
	{
		if ( *startptr == ':' || *startptr == '\0' )
		{
			values[i] = 0;
			startptr++;
			continue;
		}
		errno = 0;
		values[i] = strtof( startptr, &endptr );
		if ( errno != 0 )
		{
			*found_count = -2;
			if ( print_errors )
			{
				if ( errno == ERANGE )
				{
					fprintf( stderr, "Calibration: value is out of range.\n" );
				}
				else
				{
					fprintf(
						stderr, "Calibration: parsing value failed: %s\n",
						startptr
					);
				}
			}
			return NULL;
		}
		if ( *endptr != ':' && *endptr != '\0' )
		{
			*found_count = -3;
			if ( print_errors )
			{
				fprintf(
					stderr, "Calibration: parsing value failed: %s\n",
					startptr
				);
			}
			return NULL;
		}
		if ( !isfinite( values[i] ) )
		{
			*found_count = -4;
			if ( print_errors )
			{
				fprintf( stderr, "Calibration: values must be finite.\n" );
			}
			return NULL;
		}
		startptr = endptr;
		startptr++;
	}
	float * heap_values = malloc( ( count < 2 ? 2 : count ) * sizeof( float ) );
	if ( heap_values == NULL )
	{
		*found_count = -5;
		if ( print_errors )
		{
			fprintf( stderr, "Calibration: unable to allocate memory.\n" );
		}
		return NULL;
	}
	memcpy( heap_values, values, count * sizeof( float ) );
	if ( count < 2 )
	{
		heap_values[1] = 1;
	}
	*found_count = ( count < 2 ? 2 : count );
	return heap_values;
}

float tempered_util__calibrate_value(
	float base_value, int factor_count, float factors[]
) {
	float cur_power = 1;
	float cur_value = 0;
	int i;
	for ( i = 0 ; i < factor_count ; i++ )
	{
		cur_value += factors[i] * cur_power;
		cur_power *= base_value;
	}
	return cur_value;
}

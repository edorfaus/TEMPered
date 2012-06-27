#include <unistd.h>
#include <string.h>
#include <strings.h>
#include "tempered-util.h"

static float celsius_to_celsius( float temperature )
{
	// http://en.wikipedia.org/wiki/Celcius
	return temperature;
}

static float celsius_to_kelvin( float temperature )
{
	// http://en.wikipedia.org/wiki/Kelvin
	return temperature + 273.15;
}

static float celsius_to_fahrenheit( float temperature )
{
	// http://en.wikipedia.org/wiki/Fahrenheit
	return temperature * 9 / 5 + 32;
}

static float celsius_to_rankine( float temperature )
{
	// http://en.wikipedia.org/wiki/Rankine_scale
	return ( temperature + 273.15 ) * 9 / 5;
}

static float celsius_to_newton( float temperature )
{
	// The Newton temperature scale was devised by Isaac Newton around 1700.
	// http://en.wikipedia.org/wiki/Newton_scale
	return temperature * 33 / 100;
}

struct tempered_util__temp_scale const tempered_util__known_temp_scales[] = {
	{
		.name = "Celsius",
		.symbol = "°C",
		.from_celsius = celsius_to_celsius
	},
	{
		.name = "Kelvin",
		.symbol = "K",
		.from_celsius = celsius_to_kelvin
	},
	{
		.name = "Fahrenheit",
		.symbol = "°F",
		.from_celsius = celsius_to_fahrenheit
	},
	{
		.name = "Rankine",
		.symbol = "°R",
		.from_celsius = celsius_to_rankine
	},
	{
		.name = "Newton",
		.symbol = "°N",
		.from_celsius = celsius_to_newton
	},
	{ .name = NULL } // List terminator
};

struct tempered_util__temp_scale const * tempered_util__find_temperature_scale(
	char const * const name
) {
	if ( name == NULL || name[0] == '\0' )
	{
		return NULL;
	}
	struct tempered_util__temp_scale const *cur;
	for ( cur = tempered_util__known_temp_scales ; cur->name != NULL ; cur++ )
	{
		if ( strcasecmp( cur->name, name ) == 0 )
		{
			return cur;
		}
	}
	for ( cur = tempered_util__known_temp_scales ; cur->name != NULL ; cur++ )
	{
		if ( strcmp( cur->symbol, name ) == 0 )
		{
			return cur;
		}
	}
	size_t name_len = strlen( name );
	struct tempered_util__temp_scale const *found = NULL;
	for ( cur = tempered_util__known_temp_scales ; cur->name != NULL ; cur++ )
	{
		if ( strncasecmp( cur->name, name, name_len ) == 0 )
		{
			if ( found != NULL )
			{
				found = NULL;
				break;
			}
			found = cur;
		}
	}
	return found;
}

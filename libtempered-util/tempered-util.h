#ifndef TEMPERED_UTIL_H
#define TEMPERED_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/* temp-scale.c start */

/** Description of a known temperature scale. */
struct tempered_util__temp_scale {
	/** The name of the temperature scale, e.g. "Celsius". */
	char const * const name;
	
	/** The symbol of the temperature scale, e.g. "°C" */
	char const * const symbol;
	
	/** The function to use to convert to this scale from degrees Celsius. */
	float (* const from_celsius)( float );
};

/** The array of known temperature scales.
 * This list is terminated by an element having a name that is NULL.
 */
extern struct tempered_util__temp_scale const tempered_util__known_temp_scales[];

/** Find a temperature scale with a name or symbol that matches the given name.
 * This matches case-insensitively, and also matches on unique name prefixes.
 * @param name The name to find a temperature scale that matches.
 * @return The matching temperature scale, or NULL if no such scale was found.
 */
struct tempered_util__temp_scale const * tempered_util__find_temperature_scale(
	char const * const name
);

/* temp-scale.c end */

/* dew-point.c start */

/** Calculate the dew point for the given temperature and relative humidity.
 * @param tempC The temperature in degrees Celsius.
 * @param rel_hum The relative humidity, in %RH.
 * @return The dew point for the given temperature and relative humidity.
 */
float tempered_util__get_dew_point( float tempC, float rel_hum );

/* dew-point.c end */

#ifdef __cplusplus
}
#endif

#endif

#ifndef TEMPERED_UTIL_H
#define TEMPERED_UTIL_H

#include <stdbool.h>

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

/* calibration.c start */

/** Parse a calibration string into an array of floats.
 *
 * For convenience, if the given string only contains one value, the second is
 * defaulted to 1 so that the calibration doesn't end up ignoring the sensor.
 * @param string The string to parse. This should be a colon-separated list of
 * floats with finite numeric values.
 * @param found_count A pointer to an int where we store the count of values.
 * @param print_errors Whether or not to print error messages to stderr.
 * @return The array of floats (the size is returned in found_count), or NULL
 * on error.
 */
float* tempered_util__parse_calibration_string(
	char const * string, int *found_count, bool print_errors
);

/** Calibrate a value based on a power formula using the given factors.
 *
 * This will basically calculate the formula f0 + f1 * v + f2 * v^2 + ...
 * where v is the given base value and f0, f1 etc. are the given factors.
 *
 * Hence, the given number of factors should be at least 2, or the returned
 * value will not be based on the given base value at all.
 * @param base_value The base value to be calibrated.
 * @param factor_count The number of factors in the factors parameter.
 * @param factors The array of factors to be applied in the calibration.
 * @return The calibrated value.
 */
float tempered_util__calibrate_value(
	float base_value, int factor_count, float factors[]
);

/* calibration.c end */

#ifdef __cplusplus
}
#endif

#endif

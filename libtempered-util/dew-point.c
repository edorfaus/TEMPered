#include <math.h>

/** Calculate the dew point for the given temperature and relative humidity. */
float tempered_util__get_dew_point( float tempC, float rel_hum )
{
	// This is based on the Sensirion SHT1x datasheet, with some extra reading
	// on Wikipedia.
	double Tn = 243.12;
	double m = 17.62;
	if ( tempC < 0 )
	{
		Tn = 272.62;
		m = 22.46;
	}
	double gamma = log( rel_hum / 100 ) + m * tempC / ( Tn + tempC );
	double dew_point = Tn * gamma / ( m - gamma );
	return dew_point;
}

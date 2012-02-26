#ifndef TEMPER_TYPE_HID_H
#define TEMPER_TYPE_HID_H

#include "tempered.h"

/** Method for opening HID devices. */
bool temper_type_hid_open( tempered_device* device );

/** Method for closing HID devices. */
void temper_type_hid_close( tempered_device* device );

/** Method for getting the temperature from HID devices. */
bool temper_type_hid_get_temperature( tempered_device* device, float* tempC );

#endif

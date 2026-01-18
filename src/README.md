# Testing Adafruit LvGL Glue with V2 TSC2007 FeatherWings

V2 FeatherWings have replaced the discontinued STMPE811 touchscreen controller with the TSC2007, the original of the [Glue library](https://github.com/adafruit/Adafruit_LvGL_Glue) only supports the STMPE811.

The two touchscreen include files cannot be included at the same time (thanks to the definitions of TSPoint and associated objects clashing) so we choose which one with `#ifdef`s.

This builds and runs the included example with INFO level logging to USB serial. Touch events are recorded as happening there so touchscreen control appears to work on both models.

Testing further than the included example has not been done.

NOTES:

* The PlatformIO build targets are for the combinations of V1/V2 ESP32 Feather and V1/V2 FeatherWing.
* There are defines to set if the ESP32 device you are using has PSRAM or not
* The glue libraries [only support LvGL v8](https://github.com/adafruit/Adafruit_LvGL_Glue/issues/26)
* There is an `#ifdef` for The Touchscreen library if you are using that
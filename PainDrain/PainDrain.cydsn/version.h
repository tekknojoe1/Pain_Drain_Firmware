#ifndef VERSION_H
#define VERSION_H

/* Single source of truth for the firmware version. Bump these per release/OTA:
 * an OTA only switches slots when the new image's version is STRICTLY higher than
 * the running one. The integer version used by the chooser + DIS is
 * MAJOR*10000 + MINOR*100 + PATCH (see main_cm4.c APP_VERSION, build_slot.py). */
#define FIRMWARE_VERSION_MAJOR  1
#define FIRMWARE_VERSION_MINOR  0
#define FIRMWARE_VERSION_PATCH  2

/* String form derived from the numbers above so the two can never drift apart. */
#define FW_VERSION_STR_(x)  #x
#define FW_VERSION_STR(x)   FW_VERSION_STR_(x)
#define FIRMWARE_VERSION_STR  FW_VERSION_STR(FIRMWARE_VERSION_MAJOR) "." \
                              FW_VERSION_STR(FIRMWARE_VERSION_MINOR) "." \
                              FW_VERSION_STR(FIRMWARE_VERSION_PATCH)

#endif /* VERSION_H */

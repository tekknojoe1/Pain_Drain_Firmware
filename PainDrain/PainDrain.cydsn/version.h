#ifndef VERSION_H
#define VERSION_H

/* Single source of truth for the firmware version. Bump these per release/OTA:
 * an OTA only switches slots when the new image's version is STRICTLY higher than
 * the running one. The integer version used by the chooser + slot selection is
 * MAJOR*10000 + MINOR*100 + PATCH (see main_cm4.c APP_VERSION, build_slot.py) --
 * BUILD is NOT part of it, so a release MUST bump MAJOR/MINOR/PATCH. BUILD is a
 * build counter carried in the display/DIS string for traceability only. */
#define FIRMWARE_VERSION_MAJOR  1
#define FIRMWARE_VERSION_MINOR  1
#define FIRMWARE_VERSION_PATCH  0
#define FIRMWARE_VERSION_BUILD  6

/* String form "MAJOR.MINOR.PATCH.BUILD" derived from the numbers above so they
 * can never drift apart (published in the DIS Firmware Revision String). */
#define FW_VERSION_STR_(x)  #x
#define FW_VERSION_STR(x)   FW_VERSION_STR_(x)
#define FIRMWARE_VERSION_STR  FW_VERSION_STR(FIRMWARE_VERSION_MAJOR) "." \
                              FW_VERSION_STR(FIRMWARE_VERSION_MINOR) "." \
                              FW_VERSION_STR(FIRMWARE_VERSION_PATCH) "." \
                              FW_VERSION_STR(FIRMWARE_VERSION_BUILD)

#endif /* VERSION_H */

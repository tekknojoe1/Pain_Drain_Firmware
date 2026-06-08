#ifndef APP1_VERSION_H
#define APP1_VERSION_H

#define APP1_VERSION_MAJOR 1
#define APP1_VERSION_MINOR 0
#define APP1_VERSION_PATCH 0
#define APP1_VERSION_STR   "1.0.0"

/* Packed uint32 for .cy_app_header: major in bits 23:16, minor in 15:8, patch in 7:0 */
#define APP1_VERSION_PACKED \
    (((uint32_t)(APP1_VERSION_MAJOR) << 16u) | \
     ((uint32_t)(APP1_VERSION_MINOR) <<  8u) | \
      (uint32_t)(APP1_VERSION_PATCH))

#endif /* APP1_VERSION_H */

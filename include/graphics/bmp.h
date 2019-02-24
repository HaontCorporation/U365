#ifndef BMP_H
#define BMP_H

#include <graphics/bmp.h>

#include <stdio.h>
#include <graphics/surface.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BMP_Header BMP_Header;

struct BMP_Header
{
    // Constant file signature
    uint16_t sign;
    // File size, check
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t dataOffset;

    // Structure size
    uint32_t strSize;
    
    // Image definition
    uint32_t w;
    uint32_t h;

    // Number of images in file - must be 1
    uint16_t planes;
    // Color depth
    uint16_t bpp;
    // Compression (unsupported!)
    uint32_t compression;
    
    uint32_t dataSize;
    
    // DPI
    uint32_t dpiX;
    uint32_t dpiY;

    // Indexed colors fields, unused
    uint32_t colorsUsed;
    uint32_t colorsImportant;
} __attribute__((packed));

surface* bmp_read (FILE*);
uint8_t  bmp_write(FILE*, const surface*);

#ifdef __cplusplus
}
#endif

#endif // BMP_H

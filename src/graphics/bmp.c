#include "graphics/bmp.h"

#include "memory.h"

uint8_t pmask(uint32_t raw)
{
    uint8_t i;
    for(i = 0; i < 32; i += 8)
    {
        if(((raw >> i) & 1) && ((raw >> i) & 0xFF) == 0xFF)
        {
            return i / 8;
        }
    }
    return -1;
}

surface* bmp_read(FILE* stream)
{
    BMP_Header header;
    if(!fread(&header, sizeof(BMP_Header), 1, stream))
    {
        //perr("[BMP]: Failed to read BMP header\n");
        return 0;
    }
    if(header.sign != 0x4d42 && header.sign != 0x4349 && header.sign != 0x5450)
    {
        //perr("[BMP]: Invalid bmp signature\n");
        return 0;
    }
//    if(fseek(stream, header.fileSize - 1, SEEK_SET))
//    {
//        perr("[BMP]: Invalid file size\n");
//        return 0;
//    }
    if(header.reserved || header.planes != 1 || (header.compression && header.compression != 3))
    {
        perr("[BMP]: Unsupported header flags\n");
        printf("%i, %i, %i\n", header.reserved, header.planes, header.compression);
        return 0;
    }

    surface* result = surface_new();

    result->flags = SURFACE_EDITABLE | SURFACE_RESIZABLE | SURFACE_READABLE;
    result->del   = &surface_default_destructor;
    result->bpp   = header.bpp / 8;
    switch(header.bpp)
    {
        case 8:
            result->rb = 0;
            result->gb = 0;
            result->bb = 0;
            break;
        case 32:
            result->flags |= SURFACE_ALPHA;
            result->ab = 3;
	/* fall through */
        case 24:
            result->rb = 2;
            result->gb = 1;
            result->bb = 0;
            break;
        default:
            perr("[BMP]: Unsupported color depth\n");
            free(result);
            
    }
    
    // Channel masks
    if(header.compression == 3)
    {
        fseek(stream, 0x36, SEEK_SET);
        uint32_t rawmask;
        // Red
        fread(&rawmask, 4, 1, stream);
        result->rb = pmask(rawmask);
        // Green
        fread(&rawmask, 4, 1, stream);
        result->gb = pmask(rawmask);
        // Blue
        fread(&rawmask, 4, 1, stream);
        result->bb = pmask(rawmask);
        // Alpha
        fread(&rawmask, 4, 1, stream);
        result->ab = pmask(rawmask);
    }
    
    result->w = header.w;
    result->h = header.h;
    result->bpp = header.bpp / 8;
    int32_t pitch = result->w * result->bpp;
    int8_t t = pitch % 4;
    if(t)
        pitch += 4 - t;

    result->data = (void**)(malloc(header.h));

    size_t i;
    fseek(stream, header.dataOffset, SEEK_SET);
    for(i = 0; i < result->h; ++i)
    {
        result->data[result->h - i - 1] = malloc(pitch);
        if(!fread(result->data[result->h - i - 1], pitch, 1, stream))
        {
            perr("[BMP]: Image is corrupted, aborting\n");
            surface_delete(result);
            return 0;
        }
    }
    return result;
}

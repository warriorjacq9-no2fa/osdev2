#ifndef BOOT_H
#define BOOT_H
#include <efi.h>

typedef struct {
    UINT64 framebuffer;
    UINT32 width;
    UINT32 height;
    UINT32 pitch;
    UINT32 pixel_format;
} framebuffer_info_t;

typedef struct {
    UINT64 base;
    UINT32 baud;
    UINT8  type;   // 0 = unknown, 1 = MMIO, 2 = I/O port
} serial_info_t;

typedef struct {
    EFI_MEMORY_DESCRIPTOR *memory_map;
    UINTN  memory_map_size;
    UINTN  memory_map_descriptor_size;
    UINT32 memory_map_version;
} memory_map_info_t;

typedef struct {
    framebuffer_info_t fb;
    serial_info_t      serial;
    memory_map_info_t  mmap;
} boot_info_t;

#endif
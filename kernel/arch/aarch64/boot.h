#include <stdint.h>
#ifndef BOOT_H
#define BOOT_H

typedef struct {
    uint64_t framebuffer;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t pixel_format;
} framebuffer_info_t;

typedef struct {
    uint64_t    base;
    uint32_t    baud;
    uint8_t     type;   // 0 = unknown, 1 = MMIO, 2 = I/O port
    uint16_t    model; // See https://learn.microsoft.com/en-us/windows-hardware/drivers/bringup/acpi-debug-port-table#table-3-debug-port-types-and-subtypes
} serial_info_t;

typedef struct {
    uint32_t    type;
    uint64_t   *phys_start;
    uint64_t   *virt_start;
    uint64_t    num_pages;
    uint64_t    attribute;
} memory_map_t;

typedef struct {
    memory_map_t   *memory_map;
    uint64_t        memory_map_size;
    uint64_t        memory_map_descriptor_size;
    uint32_t        memory_map_version;
} memory_map_info_t;

typedef struct {
    framebuffer_info_t fb;
    serial_info_t      serial;
    memory_map_info_t  mmap;
} boot_info_t;

#endif
#ifndef ACPI_H
#define ACPI_H
#include <efi.h>
#include <efilib.h>
#include <boot.h>

// ACPI table header (common to all ACPI tables)
typedef struct {
    CHAR8  Signature[4];
    UINT32 Length;
    UINT8  Revision;
    UINT8  Checksum;
    CHAR8  OEMID[6];
    CHAR8  OEMTableID[8];
    UINT32 OEMRevision;
    UINT32 CreatorID;
    UINT32 CreatorRevision;
} __attribute__((packed)) ACPI_TABLE_HEADER;

// RSDP (Root System Description Pointer)
typedef struct {
    CHAR8  Signature[8];
    UINT8  Checksum;
    CHAR8  OEMID[6];
    UINT8  Revision;
    UINT32 RsdtAddress;
} __attribute__((packed)) ACPI_RSDP_1_0;

typedef struct {
    ACPI_RSDP_1_0 Rsdp1;
    UINT32 Length;
    UINT64 XsdtAddress;
    UINT8  ExtendedChecksum;
    UINT8  Reserved[3];
} __attribute__((packed)) ACPI_RSDP_2_0;

// RSDT/XSDT
typedef struct {
    ACPI_TABLE_HEADER Header;
    UINT32 Entry[];
} __attribute__((packed)) ACPI_RSDT;

typedef struct {
    ACPI_TABLE_HEADER Header;
    UINT64 Entry[];
} __attribute__((packed)) ACPI_XSDT;

// Generic Address Structure (used in SPCR)
typedef struct {
    UINT8  AddressSpaceId;    // 0 = System Memory, 1 = System I/O
    UINT8  RegisterBitWidth;
    UINT8  RegisterBitOffset;
    UINT8  AccessSize;
    UINT64 Address;
} __attribute__((packed)) ACPI_GENERIC_ADDRESS;

// SPCR (Serial Port Console Redirection) table
typedef struct {
    ACPI_TABLE_HEADER Header;
    UINT8  InterfaceType;
    UINT8  Reserved1[3];
    ACPI_GENERIC_ADDRESS BaseAddress;
    UINT8  InterruptType;
    UINT8  Irq;
    UINT32 GlobalSystemInterrupt;
    UINT8  BaudRate;
    UINT8  Parity;
    UINT8  StopBits;
    UINT8  FlowControl;
    UINT8  TerminalType;
    UINT8  Reserved2;
    UINT16 PciDeviceId;
    UINT16 PciVendorId;
    UINT8  PciBusNumber;
    UINT8  PciDeviceNumber;
    UINT8  PciFunctionNumber;
    UINT32 PciFlags;
    UINT8  PciSegment;
    UINT32 Reserved3;
} __attribute__((packed)) ACPI_SPCR_TABLE;

ACPI_TABLE_HEADER *FindAcpiTableBySignature(EFI_SYSTEM_TABLE *SystemTable, const CHAR8 *Signature);

#endif
#include <efi.h>
#include <efilib.h>
#include <boot.h>
#include <acpi.h>

VOID *FindAcpiTable(EFI_SYSTEM_TABLE *SystemTable) {
    EFI_GUID Acpi20TableGuid = ACPI_20_TABLE_GUID;
    EFI_GUID Acpi10TableGuid = ACPI_TABLE_GUID;
    EFI_CONFIGURATION_TABLE *ConfigTable = SystemTable->ConfigurationTable;
    
    // Try ACPI 2.0+ first
    for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; i++) {
        if (CompareMem(&ConfigTable[i].VendorGuid, &Acpi20TableGuid, sizeof(EFI_GUID)) == 0) {
            Print(L"Found ACPI 2.0 table: %p\n", ConfigTable[i].VendorTable);
            return ConfigTable[i].VendorTable;
        }
    }
    
    // Fall back to ACPI 1.0
    for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; i++) {
        if (CompareMem(&ConfigTable[i].VendorGuid, &Acpi10TableGuid, sizeof(EFI_GUID)) == 0) {
            Print(L"Found ACPI 1.0 table: %p\n", ConfigTable[i].VendorTable);
            return ConfigTable[i].VendorTable;
        }
    }
    
    return NULL;
}

ACPI_TABLE_HEADER *FindAcpiTableBySignature(EFI_SYSTEM_TABLE *SystemTable, const CHAR8 *Signature) {
    Print(L"Finding RSDP\n");
    VOID *Rsdp = FindAcpiTable(SystemTable);
    Print(L"Found RSDP: %p\n", Rsdp);
    
    if (!Rsdp) return NULL;
    
    ACPI_RSDP_1_0 *Rsdp1 = (ACPI_RSDP_1_0 *)Rsdp;
    
    if (CompareMem(Rsdp1->Signature, "RSD PTR ", 8) != 0) {
        Print(L"ERROR: Invalid RSDP signature\n");
        Print(L"Expected: 'RSD PTR ', got: '%.8a'\n", Rsdp1->Signature);
        return NULL;
    }
    
    Print(L"RSDP Revision: %u\n", Rsdp1->Revision);
    Print(L"RSDT Address: 0x%x\n", Rsdp1->RsdtAddress);
    
    ACPI_TABLE_HEADER *Header;
    
    Print(L"Looking for SDT's\n");
    
    // Use XSDT if available (ACPI 2.0+)
    if (Rsdp1->Revision >= 2) {
        ACPI_RSDP_2_0 *Rsdp2 = (ACPI_RSDP_2_0 *)Rsdp;
        
        Print(L"XSDT Address: 0x%lx\n", Rsdp2->XsdtAddress);
        
        // Validate the XSDT address
        if (Rsdp2->XsdtAddress == 0 || Rsdp2->XsdtAddress < 0x1000) {
            Print(L"ERROR: Invalid XSDT address, falling back to RSDT\n");
            goto use_rsdt;
        }
        
        ACPI_XSDT *Xsdt = (ACPI_XSDT *)(UINTN)Rsdp2->XsdtAddress;
        
        // Verify XSDT signature
        if (CompareMem(Xsdt->Header.Signature, "XSDT", 4) != 0) {
            Print(L"ERROR: Invalid XSDT signature: %.4a, falling back to RSDT\n", Xsdt->Header.Signature);
            goto use_rsdt;
        }
        
        Print(L"Found XSDT: %p", Xsdt);
        UINTN Entries = (Xsdt->Header.Length - sizeof(ACPI_TABLE_HEADER)) / sizeof(UINT64);
        Print(L", entries: %u\n", Entries);
        
        for (UINTN i = 0; i < Entries; i++) {
            if (Xsdt->Entry[i] == 0) continue;
            
            Header = (ACPI_TABLE_HEADER *)(UINTN)Xsdt->Entry[i];
            Print(L"Entry %u: %.4a at %p\n", i, Header->Signature, Header);
            
            if (CompareMem(Header->Signature, Signature, 4) == 0) {
                Print(L"Found ACPI table: %p\n", Header);
                return Header;
            }
        }
    } else {
use_rsdt:
        // Use RSDT (ACPI 1.0)
        Print(L"RSDT Address: 0x%x\n", Rsdp1->RsdtAddress);
        
        if (Rsdp1->RsdtAddress == 0 || Rsdp1->RsdtAddress < 0x1000) {
            Print(L"ERROR: Invalid RSDT address\n");
            return NULL;
        }
        
        ACPI_RSDT *Rsdt = (ACPI_RSDT *)(UINTN)Rsdp1->RsdtAddress;
        
        // Verify RSDT signature
        if (CompareMem(Rsdt->Header.Signature, "RSDT", 4) != 0) {
            Print(L"ERROR: Invalid RSDT signature: %.4a\n", Rsdt->Header.Signature);
            return NULL;
        }
        
        UINTN Entries = (Rsdt->Header.Length - sizeof(ACPI_TABLE_HEADER)) / sizeof(UINT32);
        Print(L"Found RSDT: %p, entries: %u\n", Rsdt, Entries);
        
        for (UINTN i = 0; i < Entries; i++) {
            if (Rsdt->Entry[i] == 0) continue;
            
            Header = (ACPI_TABLE_HEADER *)(UINTN)Rsdt->Entry[i];
            Print(L"Entry %u: %.4a at %p\n", i, Header->Signature, Header);
            
            if (CompareMem(Header->Signature, Signature, 4) == 0) {
                Print(L"Found ACPI table: %p\n", Header);
                return Header;
            }
        }
    }
    
    return NULL;
}
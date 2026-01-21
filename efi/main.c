#include <efi.h>
#include <efilib.h>
#include <efiapi.h>
#include <boot.h>
#include <acpi.h>
#include <elf.h>
#include <stdlib.h>

#define KERNEL_PATH L"\\kernel.elf"

serial_info_t find_uart(EFI_SYSTEM_TABLE *SystemTable) {
    serial_info_t result;
    result.base = 0;
    result.baud = 0;
    result.type = 0;
    
    // Try to find SPCR table
    Print(L"Finding SPCR\n");
    ACPI_SPCR_TABLE *Spcr = (ACPI_SPCR_TABLE *)FindAcpiTableBySignature(SystemTable, "SPCR");
    
    if (Spcr) {
        Print(L"Found SPCR");
        // Extract base address from SPCR
        result.base = Spcr->BaseAddress.Address;
        
        // Determine type from AddressSpaceId
        switch (Spcr->BaseAddress.AddressSpaceId) {
            case 0: // System Memory (MMIO)
                result.type = 1;
                break;
            case 1: // System I/O
                result.type = 2;
                break;
            default:
                result.type = 0;
                break;
        }
        switch (Spcr->BaudRate) {
            case 3:
                result.baud = 9600;
                break;
            case 4:
                result.baud = 19200;
                break;
            case 6:
                result.baud = 57600;
                break;
            case 7:
                result.baud = 115200;
                break;
        }

        result.model = Spcr->InterfaceType;
        Print(L", base %p, type %u, baud %u\n", result.base, result.type, result.baud);
        return result;
    }
    
    Print(L"SPCR not found");
    // Fallback: Try legacy x86 COM1 port
    #if defined(__x86_64__) || defined(__i386__)
    Print(L", falling back to COM1\n");
    result.base = 0x3F8;  // COM1
    result.type = 2;       // I/O port
    #else
    Print(L"\n");
    #endif
    
    return result;
}

void print_info(boot_info_t *info) {
    Print(L"Boot info:\n");
    if(info->fb.framebuffer) {
        Print(L"  Framebuffer info:\n");
        Print(L"    Address: %p\n", info->fb.framebuffer);
        Print(L"    Dimensions: %ux%u, %u pitch",
            info->fb.width, info->fb.height, info->fb.pitch);
        Print(L"    Pixel format: %u\n\n", info->fb.pixel_format);
    }
    if(info->serial.base) {
        Print(L"  Serial info:\n");
        Print(L"    Address: %p (%s)\n", info->serial.base, 
            (info->serial.type ? 
                (info->serial.type == 1 ? L"MMIO" : L"Port") : 
                L"????"));
        Print(L"    Baud: %u\n\n", info->serial.baud);
    }
    if(info->mmap.memory_map) {
        int count = info->mmap.memory_map_size / info->mmap.memory_map_descriptor_size;
        Print(L"  Memory map:\n");
        Print(L"    %u entries", count);
        if(count > 8) {
            count = 8;
            Print(L" (truncated to 8)");
        }
        Print(L", version %u\n",
            info->mmap.memory_map_version);
        for(int i = 0; i < count; i++) {
            Print(L"    Entry %d:\n", i);
            EFI_MEMORY_DESCRIPTOR *desc =
                (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)info->mmap.memory_map + i * info->mmap.memory_map_descriptor_size);
            Print(L"      Attribute %u, type %u\n", desc->Attribute, desc->Type);
            Print(L"      Physical address: %p\n", desc->PhysicalStart);
            Print(L"      Virtual address: %p\n", desc->VirtualStart);
            Print(L"      %u pages (%u padding)\n\n", desc->NumberOfPages, desc->Pad);
        }
    }
}

EFI_STATUS load_elf(EFI_FILE_HANDLE file, EFI_SYSTEM_TABLE *st, EFI_PHYSICAL_ADDRESS *entry_point, EFI_PHYSICAL_ADDRESS *base) {
    EFI_STATUS status;
    EFI_FILE_INFO *file_info;
    UINTN file_info_size;
    VOID *elf_data = NULL;
    UINTN elf_size;
    Elf64_Ehdr *ehdr;
    Elf64_Phdr *phdr;
    UINT64 base_addr = 0;
    UINT64 min_vaddr = (UINT64)-1;
    UINT64 max_vaddr = 0;
    UINTN total_size = 0;
    VOID *load_base = NULL;
    BOOLEAN is_pie;
    
    // Get file size
    file_info_size = SIZE_OF_EFI_FILE_INFO + 256;
    status = uefi_call_wrapper(st->BootServices->AllocatePool, 3,
                               EfiLoaderData,
                               file_info_size,
                               (VOID **)&file_info);
    if (EFI_ERROR(status)) {
        Print(L"Failed to allocate pool for file info: %r\n", status);
        return status;
    }
    
    status = uefi_call_wrapper(file->GetInfo, 4,
                               file,
                               &gEfiFileInfoGuid,
                               &file_info_size,
                               file_info);
    if (EFI_ERROR(status)) {
        Print(L"Failed to get file info: %r\n", status);
        uefi_call_wrapper(st->BootServices->FreePool, 1, file_info);
        return status;
    }
    
    elf_size = file_info->FileSize;
    uefi_call_wrapper(st->BootServices->FreePool, 1, file_info);
    
    // Allocate buffer for ELF file
    status = uefi_call_wrapper(st->BootServices->AllocatePool, 3,
                               EfiLoaderData,
                               elf_size,
                               &elf_data);
    if (EFI_ERROR(status)) {
        Print(L"Failed to allocate buffer for ELF: %r\n", status);
        return status;
    }
    
    // Read entire file
    status = uefi_call_wrapper(file->Read, 3, file, &elf_size, elf_data);
    if (EFI_ERROR(status)) {
        Print(L"Failed to read ELF file: %r\n", status);
        uefi_call_wrapper(st->BootServices->FreePool, 1, elf_data);
        return status;
    }
    
    ehdr = (Elf64_Ehdr *)elf_data;
    
    // Verify ELF magic
    if (ehdr->e_ident[0] != EI_MAG0 || ehdr->e_ident[1] != EI_MAG1 ||
        ehdr->e_ident[2] != EI_MAG2 || ehdr->e_ident[3] != EI_MAG3) {
        Print(L"Invalid ELF magic\n");
        uefi_call_wrapper(st->BootServices->FreePool, 1, elf_data);
        return EFI_INVALID_PARAMETER;
    }
    
    // Check if 64-bit
    if (ehdr->e_ident[4] != 2) {
        Print(L"Not a 64-bit ELF\n");
        uefi_call_wrapper(st->BootServices->FreePool, 1, elf_data);
        return EFI_UNSUPPORTED;
    }
    
    // Determine if PIE (ET_DYN) or non-PIE (ET_EXEC)
    is_pie = (ehdr->e_type == ET_DYN);
    
    if (ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN) {
        Print(L"Unsupported ELF type: %d\n", ehdr->e_type);
        uefi_call_wrapper(st->BootServices->FreePool, 1, elf_data);
        return EFI_UNSUPPORTED;
    }
    
    // First pass: determine memory requirements
    for (UINT16 i = 0; i < ehdr->e_phnum; i++) {
        phdr = (Elf64_Phdr *)((UINT8 *)elf_data + ehdr->e_phoff + i * ehdr->e_phentsize);
        
        if (phdr->p_type == PT_LOAD) {
            if (phdr->p_vaddr < min_vaddr) {
                min_vaddr = phdr->p_vaddr;
            }
            if (phdr->p_vaddr + phdr->p_memsz > max_vaddr) {
                max_vaddr = phdr->p_vaddr + phdr->p_memsz;
            }
        }
    }
    
    total_size = max_vaddr - min_vaddr;
    
    if (is_pie) {
        // For PIE, allocate memory anywhere
        UINTN pages = EFI_SIZE_TO_PAGES(total_size);
        EFI_PHYSICAL_ADDRESS addr = 0;
        
        status = uefi_call_wrapper(st->BootServices->AllocatePages, 4,
                                   AllocateAnyPages,
                                   EfiLoaderCode,
                                   pages,
                                   &addr);
        if (EFI_ERROR(status)) {
            Print(L"Failed to allocate memory for PIE: %r\n", status);
            uefi_call_wrapper(st->BootServices->FreePool, 1, elf_data);
            return status;
        }
        
        load_base = (VOID *)addr;
        
        // Clear the allocated memory
        SetMem(load_base, total_size, 0);
        
        base_addr = addr - min_vaddr;
    } else {
        // For non-PIE, load at specific addresses
        base_addr = 0;
        load_base = (VOID *)min_vaddr;
    }
    
    // Second pass: load segments
    for (UINT16 i = 0; i < ehdr->e_phnum; i++) {
        phdr = (Elf64_Phdr *)((UINT8 *)elf_data + ehdr->e_phoff + i * ehdr->e_phentsize);
        
        if (phdr->p_type == PT_LOAD) {
            VOID *dest = (VOID *)(base_addr + phdr->p_vaddr);
            VOID *src = (VOID *)((UINT8 *)elf_data + phdr->p_offset);
            
            if (!is_pie) {
                // For non-PIE, allocate at specific address
                UINTN pages = EFI_SIZE_TO_PAGES(phdr->p_memsz);
                EFI_PHYSICAL_ADDRESS addr = phdr->p_vaddr;
                
                status = uefi_call_wrapper(st->BootServices->AllocatePages, 4,
                                          AllocateAddress,
                                          EfiLoaderCode,
                                          pages,
                                          &addr);
                if (EFI_ERROR(status)) {
                    Print(L"Failed to allocate at 0x%lx: %r\n", phdr->p_vaddr, status);
                    uefi_call_wrapper(st->BootServices->FreePool, 1, elf_data);
                    return status;
                }
                
                // Clear the segment
                SetMem(dest, phdr->p_memsz, 0);
            }
            
            // Copy file contents
            if (phdr->p_filesz > 0) {
                CopyMem(dest, src, phdr->p_filesz);
            }
            
            // Zero out BSS (p_memsz > p_filesz)
            if (phdr->p_memsz > phdr->p_filesz) {
                SetMem((UINT8 *)dest + phdr->p_filesz, 
                      phdr->p_memsz - phdr->p_filesz, 0);
            }
        }
    }
    
    // Process relocations for PIE
    if (is_pie) {
        Elf64_Dyn *dyn = NULL;
        UINT64 rela_addr = 0;
        UINT64 rela_size = 0;
        UINT64 rela_ent = 0;
        
        // Find PT_DYNAMIC segment
        for (UINT16 i = 0; i < ehdr->e_phnum; i++) {
            phdr = (Elf64_Phdr *)((UINT8 *)elf_data + ehdr->e_phoff + i * ehdr->e_phentsize);
            
            if (phdr->p_type == PT_DYNAMIC) {
                dyn = (Elf64_Dyn *)(base_addr + phdr->p_vaddr);
                break;
            }
        }
        
        if (dyn) {
            // Parse dynamic section to find relocation info
            for (Elf64_Dyn *d = dyn; d->d_tag != DT_NULL; d++) {
                if (d->d_tag == DT_RELA) {
                    rela_addr = d->d_un.d_ptr;
                } else if (d->d_tag == DT_RELASZ) {
                    rela_size = d->d_un.d_val;
                } else if (d->d_tag == DT_RELAENT) {
                    rela_ent = d->d_un.d_val;
                }
            }
            
            // Apply relocations
            if (rela_addr && rela_size && rela_ent) {
                Elf64_Rela *rela = (Elf64_Rela *)(base_addr + rela_addr);
                UINTN rela_count = rela_size / rela_ent;
                
                for (UINTN i = 0; i < rela_count; i++) {
                    UINT32 type = rela[i].r_info & 0xffffffff;
                    
                    if (type == R_AARCH64_RELATIVE) {
                        UINT64 *reloc_addr = (UINT64 *)(base_addr + rela[i].r_offset);
                        *reloc_addr = base_addr + rela[i].r_addend;
                    }
                }
                
                Print(L"Applied %d relocations\n", rela_count);
            }
        }
    }
    
    // Calculate entry point
    *entry_point = base_addr + ehdr->e_entry;
    *base = base_addr;
    
    Print(L"ELF loaded successfully\n");
    Print(L"Type: %s\n", is_pie ? L"PIE (ET_DYN)" : L"non-PIE (ET_EXEC)");
    Print(L"Load base: 0x%lx\n", is_pie ? (UINT64)load_base : min_vaddr);
    Print(L"Entry point: 0x%lx\n", *entry_point);
    
    // Free the ELF file buffer
    uefi_call_wrapper(st->BootServices->FreePool, 1, elf_data);
    
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *st)
{   
    InitializeLib(image, st);

    EFI_STATUS status;
    EFI_FILE_IO_INTERFACE *fs;
    EFI_FILE_HANDLE root, kernel;

    // Open filesystem
    status = uefi_call_wrapper(
        st->BootServices->LocateProtocol,
        3,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        (void **)&fs
    );
    if(EFI_ERROR(status)) {
        Print(L"Filesystem error: %r\n", status);
        return status;
    }

    fs->OpenVolume(fs, &root);
        status = root->Open(
        root,
        &kernel,
        KERNEL_PATH,
        EFI_FILE_MODE_READ,
        0
    );

    if (EFI_ERROR(status)) {
        if (status == EFI_NOT_FOUND) {
            Print(L"Kernel not found: %s\n", KERNEL_PATH);
        } else {
            Print(L"Failed to open kernel: %r\n", status);
        }
        return status;
    }

    // Read ELF header
    EFI_PHYSICAL_ADDRESS elf_entry;
    EFI_PHYSICAL_ADDRESS base;
    status = load_elf(kernel, st, &elf_entry, &base);

    if(EFI_ERROR(status)) {
        return (EFI_STATUS)elf_entry;
    }

    boot_info_t *boot_info;
    st->BootServices->AllocatePool(
        EfiLoaderData,
        sizeof(boot_info_t),
        (void **)&boot_info
    );
    SetMem(boot_info, sizeof(*boot_info), 0);

    // --- Graphics (optional) --- 
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
    status = uefi_call_wrapper(
        st->BootServices->LocateProtocol,
        3,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        (void **)&gop
    );

    if (!EFI_ERROR(status)) {
        boot_info->fb.framebuffer =
            gop->Mode->FrameBufferBase;
        boot_info->fb.width =
            gop->Mode->Info->HorizontalResolution;
        boot_info->fb.height =
            gop->Mode->Info->VerticalResolution;
        boot_info->fb.pitch =
            gop->Mode->Info->PixelsPerScanLine;
        boot_info->fb.pixel_format =
            gop->Mode->Info->PixelFormat;
    }

    // --- Serial --- 
    serial_info_t uart = find_uart(st);
    boot_info->serial = uart;

    // --- Memory map --- 
    UINTN mmap_size = 0, map_key, desc_size;
    UINT32 desc_ver;

    st->BootServices->GetMemoryMap(
        &mmap_size, NULL, &map_key, &desc_size, &desc_ver);

    mmap_size += desc_size * 8;

    EFI_MEMORY_DESCRIPTOR *mmap;
    st->BootServices->AllocatePool(
        EfiLoaderData,
        mmap_size,
        (void **)&mmap
    );

    st->BootServices->GetMemoryMap(
        &mmap_size,
        mmap,
        &map_key,
        &desc_size,
        &desc_ver
    );

    boot_info->mmap.memory_map = mmap;
    boot_info->mmap.memory_map_size = mmap_size;
    boot_info->mmap.memory_map_descriptor_size = desc_size;
    boot_info->mmap.memory_map_version = desc_ver;

    print_info(boot_info);

    void (*entry)(boot_info_t *, UINTN) =
        (void (*)(boot_info_t *, UINTN))elf_entry;

    Print(L"Exiting boot services, jumping to kernel at %p, with boot_info at %p...\n", entry, boot_info);

    // Exit boot services 
    status = uefi_call_wrapper(
        st->BootServices->ExitBootServices,
        2,
        image,
        map_key
    );
    if(EFI_ERROR(status)) {
        Print(L"Error exiting: %r\n", status);
        return status;
    }
    // Jump to kernel 
    entry(boot_info, base);
    while(1);
    return EFI_SUCCESS;
}

#include <efi.h>
#include <efilib.h>
#include <efiapi.h>
#include <boot.h>
#include <acpi.h>

#define KERNEL_PATH L"\\kernel.elf"
#define ELF_MAGIC 0x464C457F
#define PT_LOAD   1

typedef struct {
    UINT32 magic;
    UINT8  class_;
    UINT8  data;
    UINT8  version;
    UINT8  osabi;
    UINT8  abiversion;
    UINT8  pad[7];
} Elf64_Ident;

typedef struct {
    Elf64_Ident ident;
    UINT16 type;
    UINT16 machine;
    UINT32 version;
    UINT64 entry;
    UINT64 phoff;
    UINT64 shoff;
    UINT32 flags;
    UINT16 ehsize;
    UINT16 phentsize;
    UINT16 phnum;
    UINT16 shentsize;
    UINT16 shnum;
    UINT16 shstrndx;
} Elf64_Ehdr;

typedef struct {
    UINT32 type;
    UINT32 flags;
    UINT64 offset;
    UINT64 vaddr;
    UINT64 paddr;
    UINT64 filesz;
    UINT64 memsz;
    UINT64 align;
} Elf64_Phdr;

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
            EFI_MEMORY_DESCRIPTOR desc = info->mmap.memory_map[i];
            Print(L"      Attribute %u, type %u\n", desc.Attribute, desc.Type);
            Print(L"      Physical address: %p\n", desc.PhysicalStart);
            Print(L"      Virtual address: %p\n", desc.VirtualStart);
            Print(L"      %u pages (%u padding)\n\n", desc.NumberOfPages, desc.Pad);
        }
    }
}

EFI_PHYSICAL_ADDRESS load_kernel(EFI_HANDLE image, EFI_SYSTEM_TABLE *st) {

    EFI_STATUS status;
    EFI_FILE_IO_INTERFACE *fs;
    EFI_FILE_HANDLE root, kernel;

    /* Open filesystem */
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
    root->Open(root, &kernel, KERNEL_PATH, EFI_FILE_MODE_READ, 0);

    /* Read ELF header */
    Elf64_Ehdr ehdr;
    UINTN size = sizeof(ehdr);
    kernel->Read(kernel, &size, &ehdr);

    if (ehdr.ident.magic != ELF_MAGIC) {
        Print(L"Bad ELF");
        return EFI_LOAD_ERROR;
    }

    EFI_PHYSICAL_ADDRESS addr;
    /* Load segments */
    for (UINT16 i = 0; i < ehdr.phnum; i++) {
        Elf64_Phdr ph;
        UINTN psz = sizeof(ph);

        kernel->SetPosition(kernel, ehdr.phoff + i * ehdr.phentsize);
        kernel->Read(kernel, &psz, &ph);

        if (ph.type != PT_LOAD)
            continue;

        UINTN pages = EFI_SIZE_TO_PAGES(ph.memsz);
        if(pages == 0) {
            Print(L"Bad ELF: PHDR %u memsz is zero", i);
        }
        Print(L"Loaded PHDR %u: memsz = %lx pages = %lx\n", 
            i, ph.memsz, pages);

        status = uefi_call_wrapper(
            st->BootServices->AllocatePages,
            4,
            AllocateAnyPages,
            EfiLoaderCode,
            pages,
            &addr
        );
        if(EFI_ERROR(status)) {
            Print(L"Error allocating: %r\n", status);
            return status;
        }

        kernel->SetPosition(kernel, ph.offset);
        UINTN fsz = ph.filesz;
        kernel->Read(kernel, &fsz, (void *)addr);

        if (ph.memsz > ph.filesz)
            SetMem((void *)(addr + ph.filesz),
                   ph.memsz - ph.filesz,
                   0);
    }
    Print(L"Loaded kernel ELF at %p, entry at %p\n", addr, addr + ehdr.entry);
    return addr + ehdr.entry;
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
    EFI_PHYSICAL_ADDRESS elf_entry = load_kernel(image, st);

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

    void (*entry)(boot_info_t *) =
        (void (*)(boot_info_t *))elf_entry;

    Print(L"Exiting boot services, jumping to kernel at %p, with boot_info at %p...\n", entry, &boot_info);

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
    entry(boot_info);
    while(1);
    return EFI_SUCCESS;
}

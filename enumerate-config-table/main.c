#include <efi.h>
#include <efilib.h>

#define PACKED
typedef EFI_GUID GUID;
#include <Acpi.h>
#define EFI_ACPI_TABLE_GUID ACPI_TABLE_GUID
#define EFI_ACPI_20_TABLE_GUID ACPI_20_TABLE_GUID

typedef EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER RSDP1_t;
typedef EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER RSDP2_t;

const EFI_GUID gEfiAcpiTableGuid = EFI_ACPI_TABLE_GUID;
const EFI_GUID gEfiAcpi20TableGuid = EFI_ACPI_20_TABLE_GUID;

EFI_STATUS searchEfiConfigurationTable( const EFI_GUID          *guid_p,
                                        EFI_CONFIGURATION_TABLE **entry_pp)
{
    EFI_STATUS ret;
    EFI_CONFIGURATION_TABLE *cfg_table_p    = ST->ConfigurationTable;
    EFI_CONFIGURATION_TABLE *entry_p;
    UINTN i, count                          = ST->NumberOfTableEntries;
    for(i=0, ret = EFI_NOT_FOUND; i<count; i++){
        entry_p = &(cfg_table_p[i]);
        if(0 == CompareGuid((EFI_GUID*)guid_p, &(entry_p->VendorGuid))){
            ret = EFI_SUCCESS;
            *entry_pp = entry_p;
            break;
        }
    }
    return ret;
}

EFI_STATUS getRSDP1fromCfgTable(RSDP1_t **RSDP_pp)
{
    EFI_STATUS ret;
    EFI_CONFIGURATION_TABLE *entry_p;
    ret = searchEfiConfigurationTable(  &gEfiAcpiTableGuid,
                                        &entry_p);
    if(!EFI_ERROR(ret))
        *RSDP_pp = (RSDP1_t*)entry_p->VendorTable;
    return ret;
}

EFI_STATUS getRSDP2fromCfgTable(RSDP2_t **RSDP_pp)
{
    EFI_STATUS ret;
    EFI_CONFIGURATION_TABLE *entry_p;
    ret = searchEfiConfigurationTable(  &gEfiAcpi20TableGuid,
                                        &entry_p);
    if(!EFI_ERROR(ret))    
        *RSDP_pp = (RSDP2_t*)entry_p->VendorTable;
    return ret;
}

UINTN getRSDPsize(VOID)
{
    EFI_STATUS ret;
    UINTN size;
    RSDP2_t *RSDP2_p;
    RSDP1_t *RSDP1_p;
    ret = getRSDP2fromCfgTable(&RSDP2_p);
    if(!EFI_ERROR(ret))
        size = sizeof(RSDP2_t);
    else{
        if(ret != EFI_NOT_FOUND){
            Print(  L"getRSDPsize: getRSDP2fromCfgTable "\
                    L"returned error value %r\n",
                    ret);
            size = 0;
        }else{
            ret = getRSDP1fromCfgTable(&RSDP1_p);
            if(!EFI_ERROR(ret))
                size = sizeof(RSDP1_t);
            else{
                if(ret != EFI_NOT_FOUND){
                    Print(  L"getRSDPsize: getRSDPfromCfgTable "\
                            L"returned error value %r\n",
                            ret);
                }
                size = 0;
            }
        }
    }
    return size;
}

EFI_STATUS EFIAPI efi_main( EFI_HANDLE          ImageHandle,
                            EFI_SYSTEM_TABLE    *SystemTable)
{
    UINTN size;
    InitializeLib(ImageHandle, SystemTable);
    Print(L"Hello, world!\n");
    size = getRSDPsize();
    Print(L"RSDP1 size: %d\n", sizeof(RSDP1_t));
    Print(L"RSDP2 size: %d\n", sizeof(RSDP2_t));
    Print(L"current RSDP size: %d\n", size);
    return EFI_SUCCESS;
}


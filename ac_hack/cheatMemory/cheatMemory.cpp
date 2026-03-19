#include "Windows.h"
#include "cheatMemory.h"
#include "../globals/globals.h"

namespace Memory {
    void unpatchAll() {
        patchNoSpread(false);
    }

    void patchNoSpread(bool notActive) {
        patch(
            Globals::Addresses::baseAddress + Globals::Addresses::Offssets::noSpread,
            std::vector<BYTE>{ 0x0F, 0x8E, 0x99, 0x00, 0x00, 0x00},
            std::vector<BYTE>{ 0xE9, 0x9A, 0x00, 0x00, 0x00, 0x90 },
            notActive);
    }

    void patchNoSpread() {
        patch(
            Globals::Addresses::baseAddress + Globals::Addresses::Offssets::noSpread,
            std::vector<BYTE>{ 0x0F, 0x8E, 0x99, 0x00, 0x00, 0x00},
            std::vector<BYTE>{ 0xE9, 0x9A, 0x00, 0x00, 0x00, 0x90 },
            Globals::Settings::LittleFeatures::noSpreadOn);
    }

    void patch(uintptr_t address, const std::vector<BYTE>& oldBytes, const std::vector<BYTE>& newBytes, bool bIsActive) {
        const std::vector<BYTE>& bytesToWrite = bIsActive ? newBytes : oldBytes;
        size_t size = bytesToWrite.size();

        DWORD oldProtect;
        VirtualProtect((void*)address, size, PAGE_EXECUTE_READWRITE, &oldProtect);

        memcpy((void*)address, bytesToWrite.data(), size);

        VirtualProtect((void*)address, size, oldProtect, &oldProtect);
    }

    //to write smth to data section
    void patchds(uintptr_t address, void* value, size_t size) { 
        DWORD oldProtect;
        VirtualProtect((void*)address, size, PAGE_READWRITE, &oldProtect);
        memcpy((void*)address, value, size);
        VirtualProtect((void*)address, size, oldProtect, &oldProtect);
    }

    //exception filter function to parse exception details
    int ExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS* ep) {
        PEXCEPTION_RECORD record = ep->ExceptionRecord;

        printf("Exception code: 0x%X\n", code);
        printf("Faulting instruction address: %p\n", record->ExceptionAddress);

        switch (code) {
        case EXCEPTION_ACCESS_VIOLATION: {
            printf("EXCEPTION_ACCESS_VIOLATION\n");

            //for Access Violation, the ExceptionInformation array contains details:
            // [0] - operation type (0 = read, 1 = write, 8 = DEP violation)
            // [1] - the memory address that caused the fault

            ULONG_PTR operation = record->ExceptionInformation[0];
            ULONG_PTR badAddress = record->ExceptionInformation[1];

            if (operation == 0) {
                printf("Attempted to READ from invalid address: 0x%IX\n", badAddress);
            }
            else if (operation == 1) {
                printf("Attempted to WRITE to invalid address: 0x%IX\n", badAddress);
            }
            else if (operation == 8) {
                printf("Data Execution Prevention (DEP) violation at address: 0x%IX\n", badAddress);
            }

            //if the address is close to zero (e.g., 0x00000004), 
            //it's highly likely a null pointer dereference
            if (badAddress < 0x1000) {
                printf("PROBABLE CAUSE: Null Pointer Dereference!\n");
            }
            break;
        }
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            printf("Integer division by zero!\n");
            break;
        case EXCEPTION_STACK_OVERFLOW:
            printf("Stack overflow!\n");
            break;
            //add other exception codes as needed
        }

        //tell the program to execute the __except block
        return EXCEPTION_EXECUTE_HANDLER;
    }
}

#pragma once
#include <vector>
namespace Memory {
    void unpatchAll();
    void patchNoSpread(bool);
    void patchNoSpread();
    void patch(uintptr_t address, const std::vector<BYTE>& oldBytes, const std::vector<BYTE>& newBytes, bool bIsActive);
    void patchds(uintptr_t address, void* value, size_t size); 
    int ExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS* ep);
}
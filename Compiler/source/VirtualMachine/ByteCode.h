#pragma once

#include <Defines.h>
#include <Compiler/API.h>
#include <VirtualMachine/FunctionDeclaration.h>
#include <string>
#include <optional>
#include <vector>
#include <unordered_map>

struct COMPILER_API ByteCode
{
    std::vector<u8> data;
    std::unordered_map<std::string, FunctionDeclaration> functions;

    inline u8 readUInt8(u16& ip) const;
    inline u16 readUInt16(u16& ip) const;
    inline i32 readInt32(u16& ip) const;

    inline void writeUInt8(u8 value);
    inline void writeUInt16(u16 value);
    inline void writeUInt16(u16 value, u16 address);
    inline void writeInt32(i32 value);

    void setFunctionDeclaration(const FunctionDeclaration& declaration);
    std::optional<FunctionDeclaration> getFunctionDeclaration(const std::string& name);
};

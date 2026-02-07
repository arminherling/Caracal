#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>
#include <Caracal/VirtualMachine/FunctionDeclaration.h>
#include <string>
#include <optional>
#include <vector>
#include <unordered_map>

struct CARACAL_API ByteCode
{
    std::vector<u8> data;
    std::unordered_map<std::string, FunctionDeclaration> functions;

    u8 readUInt8(u16& ip) const;
    u16 readUInt16(u16& ip) const;
    i32 readInt32(u16& ip) const;

    void writeUInt8(u8 value);
    void writeUInt16(u16 value);
    void writeUInt16(u16 value, u16 address);
    void writeInt32(i32 value);

    void setFunctionDeclaration(const FunctionDeclaration& declaration);
    std::optional<FunctionDeclaration> getFunctionDeclaration(const std::string& name);
};

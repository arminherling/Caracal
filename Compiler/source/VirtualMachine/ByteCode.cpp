#include "ByteCode.h"

inline u8 ByteCode::readUInt8(u16& ip) const
{
    return data[ip++];
}

inline u16 ByteCode::readUInt16(u16& ip) const
{
    return (u16)(data[ip++] | (data[ip++] << 8));
}

inline i32 ByteCode::readInt32(u16& ip) const
{
    return (i32)(data[ip++] | (data[ip++] << 8) | (data[ip++] << 16) | (data[ip++] << 24));
}

inline void ByteCode::writeUInt8(u8 value)
{
    data.emplace_back(value);
}

inline void ByteCode::writeUInt16(u16 value)
{
    data.emplace_back((value >> 0) & 0xFF);
    data.emplace_back((value >> 8) & 0xFF);
}

inline void ByteCode::writeUInt16(u16 value, u16 address)
{
    data[address] = ((value >> 0) & 0xFF);
    data[address+1] = ((value >> 8) & 0xFF);
}

inline void ByteCode::writeInt32(i32 value)
{
    data.emplace_back((value >> 0) & 0xFF);
    data.emplace_back((value >> 8) & 0xFF);
    data.emplace_back((value >> 16) & 0xFF);
    data.emplace_back((value >> 24) & 0xFF);
}

void ByteCode::setFunctionDeclaration(const FunctionDeclaration& declaration)
{
    functions.insert({ declaration.name, declaration });
}

std::optional<FunctionDeclaration> ByteCode::getFunctionDeclaration(const std::string& name)
{
    if (auto search = functions.find(name); search != functions.end())
        return search->second;
    else
        return std::nullopt;
}

#pragma once

#include <Defines.h>
#include <Compiler/API.h>
#include <Text/SourceText.h>

struct COMPILER_API SourceLocation
{
    i32 startIndex = -1;
    i32 endIndex = -1;
    i32 startColumn = -1;
    i32 endColumn = -1;
    i32 startLine = -1;
    i32 endLine = -1;
};

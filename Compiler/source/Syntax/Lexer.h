#pragma once

#include <Compiler/API.h>
#include <Compiler/DiagnosticsBag.h>
#include <Syntax/TokenBuffer.h>
#include <Text/SourceText.h>

namespace Caracal 
{
    COMPILER_API TokenBuffer Lex(const SourceTextSharedPtr& source, DiagnosticsBag& diagnostics) noexcept;
}

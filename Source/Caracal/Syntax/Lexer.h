#pragma once

#include <Caracal/API.h>
#include <Caracal/Debug/DiagnosticsBag.h>
#include <Caracal/Syntax/TokenBuffer.h>
#include <Caracal/Text/SourceText.h>

namespace Caracal 
{
    CARACAL_API TokenBuffer lex(const SourceTextSharedPtr& source, DiagnosticsBag& diagnostics) noexcept;
}

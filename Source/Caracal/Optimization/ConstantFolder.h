#pragma once

#include <Caracal/API.h>
#include <Caracal/CompilationContext.h>
#include <Caracal/Diagnostics/DiagnosticsBag.h>
#include <Caracal/Semantic/SemanticContext.h>
#include <Caracal/Syntax/ParseTree.h>

#include <vector>

namespace Caracal
{
    CARACAL_API bool foldConstants(
        const CompilationContext& compilationContext,
        DiagnosticsBag& diagnostics,
        bool isPreludePass = false) noexcept;
}

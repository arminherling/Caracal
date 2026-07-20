#pragma once

#include <Caracal/API.h>
#include <Caracal/Diagnostics/DiagnosticsBag.h>
#include <Caracal/Semantic/SemanticContext.h>
#include <Caracal/Syntax/ParseTree.h>

#include <vector>

namespace Caracal
{
    CARACAL_API bool foldConstants(
        const std::vector<ParseTreeUPtr>& parseTrees,
        const SemanticContext& module,
        DiagnosticsBag& diagnostics) noexcept;
}

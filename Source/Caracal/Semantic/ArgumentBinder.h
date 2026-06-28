#pragma once

#include <Caracal/Semantic/Parameter.h>

#include <string>
#include <vector>

namespace Caracal
{
    class FunctionCallExpression;
    class Expression;
    class DiagnosticsBag;
    class TokenBuffer;

    struct ArgumentBindingResult
    {
        std::vector<const Expression*> ordered; 
        std::vector<const Expression*> variadic;
        bool ok = true;
    };

    ArgumentBindingResult bindCallArguments(
        const std::vector<Parameter>& parameters,
        size_t parameterOffset,
        bool isVariadic,
        const FunctionCallExpression& call,
        const std::string& functionName,
        DiagnosticsBag& diagnostics,
        const TokenBuffer& tokens);
}

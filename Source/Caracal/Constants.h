#pragma once

namespace Caracal
{
    // name of the implicit this parameter for methods, which is not visible in 
    // Caracal source code but is used in the semantic context and IR generation
    // the leading dot cant appear in a Caracal identifier
    inline constexpr const char* ImplicitThisName = ".this";
    inline constexpr const char* GlobalInitializerName = ".global_init";

    inline constexpr const char* EntryPointFunctionName = "caracalMain";
    inline constexpr const char* UserMainFunctionName = "main";
    inline constexpr const char* CRuntimeEntrySymbolName = "main";
    inline constexpr const char* UserMainSymbolName = "caracal.userMain";
}

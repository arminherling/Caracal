#pragma once

#include <Compiler/API.h>
#include <Syntax/Node.h>

class COMPILER_API Statement : public Node
{
public:
    Statement(NodeKind kind, const Type& type);
};

using StatementUPtr = std::unique_ptr<Statement>;

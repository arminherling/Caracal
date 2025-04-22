#pragma once

#include <Defines.h>
#include <Compiler/API.h>
#include <Compiler/DiagnosticsBag.h>
#include <Syntax/Token.h>
#include <Text/SourceText.h>
#include <Text/SourceLocation.h>

#include <vector>

class COMPILER_API TokenBuffer
{
public:
    explicit TokenBuffer(const SourceTextSharedPtr& source);

    Token addToken(const Token& token) noexcept;
    i32 addLexeme(QStringView lexeme) noexcept;
    i32 addSourceLocation(const SourceLocation& sourceLocation) noexcept;

    i32 size() const noexcept;
    const Token& operator[](i32 position) const noexcept;

    const SourceLocation& getSourceLocation(const Token& token) const noexcept;
    QStringView getLexeme(const Token& token) const noexcept;

private:
    SourceTextSharedPtr source;
    std::vector<Token> tokens;
    std::vector<QStringView> lexemes;
    std::vector<SourceLocation> sourceLocations;
};

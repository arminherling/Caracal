#pragma once

#include <Defines.h>
#include <Compiler/API.h>
#include <Syntax/Token.h>
#include <Text/SourceText.h>
#include <Text/SourceLocation.h>

#include <vector>

namespace Caracal
{
    class COMPILER_API TokenBuffer
    {
    public:
        explicit TokenBuffer(const SourceTextSharedPtr& source);

        void addToken(const Token& token) noexcept;
        [[nodiscard]] i32 addLexeme(QStringView lexeme) noexcept;
        [[nodiscard]] i32 addSourceLocation(const SourceLocation& sourceLocation) noexcept;

        [[nodiscard]] i32 size() const noexcept;
        [[nodiscard]] const Token& getToken(i32 position) const noexcept;
        [[nodiscard]] const Token& getLastToken() const noexcept;
        [[nodiscard]] const SourceLocation& getSourceLocation(const Token& token) const noexcept;
        [[nodiscard]] QStringView getLexeme(const Token& token) const noexcept;

    private:
        SourceTextSharedPtr source;
        std::vector<Token> tokens;
        std::vector<QStringView> lexemes;
        std::vector<SourceLocation> sourceLocations;
    };
}

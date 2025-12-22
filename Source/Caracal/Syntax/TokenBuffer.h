#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Text/SourceText.h>
#include <Caracal/Text/SourceLocation.h>

#include <vector>

namespace Caracal
{
    class CARACAL_API TokenBuffer
    {
    public:
        explicit TokenBuffer(const SourceTextSharedPtr& source);

        void addToken(const Token& token) noexcept;
        [[nodiscard]] i32 addLexeme(std::string_view lexeme) noexcept;
        [[nodiscard]] i32 addTrivia(std::string_view trivia) noexcept;
        [[nodiscard]] i32 addSourceLocation(const SourceLocation& sourceLocation) noexcept;

        [[nodiscard]] i32 size() const noexcept;
        [[nodiscard]] const Token& getToken(i32 position) const noexcept;
        [[nodiscard]] const Token& getLastToken() const noexcept;
        [[nodiscard]] std::string_view getLexeme(const Token& token) const noexcept;
        [[nodiscard]] std::string_view getTrivia(const Token& token) const noexcept;
        [[nodiscard]] const SourceLocation& getSourceLocation(const Token& token) const noexcept;

    private:
        SourceTextSharedPtr source;
        std::vector<Token> tokens;
        std::vector<std::string_view> lexemes;
        std::vector<std::string_view> trivias;
        std::vector<SourceLocation> sourceLocations;
    };
}

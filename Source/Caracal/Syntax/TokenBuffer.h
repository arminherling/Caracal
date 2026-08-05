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
        explicit TokenBuffer(const SourceTextSharedPtr& source, u16 fileId = 0, i32 firstTokenStart = 0);

        inline void addToken(TokenKind kind, const SourceLocation& sourceLocation) noexcept
        {
            m_kinds.push_back(kind);
            m_sourceLocations.push_back(sourceLocation);
        }

        [[nodiscard]] const SourceTextSharedPtr& source() const noexcept;
        [[nodiscard]] u16 fileId() const noexcept;
        [[nodiscard]] inline i32 size() const noexcept
        {
            return m_kinds.size();
        }

        [[nodiscard]] inline Token getToken(i32 position) const noexcept
        {
            return { .kind = m_kinds.at(position), .fileId = m_fileId, .index = position };
        }
        [[nodiscard]] Token getLastToken() const noexcept;
        [[nodiscard]] std::string_view getLexeme(const Token& token) const noexcept;
        [[nodiscard]] std::string_view getTrivia(const Token& token) const noexcept;
        [[nodiscard]] const SourceLocation& getSourceLocation(const Token& token) const noexcept;

    private:
        SourceTextSharedPtr m_source;
        u16 m_fileId;
        i32 m_firstTokenStart;
        std::vector<TokenKind> m_kinds;
        std::vector<SourceLocation> m_sourceLocations;
    };
}

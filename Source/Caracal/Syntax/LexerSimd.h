#pragma once

#include <Caracal/Defines.h>

#include <bit>

#if defined(_M_X64) || defined(__x86_64__)
    #define CARACAL_LEXER_SSE2 1
    #include <emmintrin.h>
#endif

// the functions rely on at least SourceText::TailPaddingSize readable NUL bytes 
// at the end of text, so that 16 bytes vector loads dont fail
namespace Caracal::LexerScan
{
    [[nodiscard]] inline auto isLetter(char c) noexcept
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    [[nodiscard]] inline auto isNumber(char c) noexcept
    {
        return (c >= '0' && c <= '9');
    }

    [[nodiscard]] inline auto isUnderscoreOrNumber(char c) noexcept
    {
        return (c == '_') || isNumber(c);
    }

    [[nodiscard]] inline auto isUnderscoreOrLetter(char c) noexcept
    {
        return (c == '_') || isLetter(c);
    }

    [[nodiscard]] inline auto isUnderscoreOrLetterOrNumber(char c) noexcept
    {
        return (c == '_') || isLetter(c) || isNumber(c);
    }

    [[nodiscard]] inline auto isWhitespace(char c) noexcept
    {
        return (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n');
    }

    namespace Scalar
    {
        // count of leading identifier characters ([A-Za-z0-9_])
        [[nodiscard]] inline i32 identifierRunLength(const char* text) noexcept
        {
            i32 length = 0;
            while (isUnderscoreOrLetterOrNumber(text[length]))
                length++;

            return length;
        }

        // count of leading digits
        [[nodiscard]] inline i32 digitRunLength(const char* text) noexcept
        {
            i32 length = 0;
            while (isNumber(text[length]))
                length++;

            return length;
        }

        // count of leading whitespace characters (space, tab, CR, LF)
        [[nodiscard]] inline i32 whitespaceRunLength(const char* text) noexcept
        {
            i32 length = 0;
            while (isWhitespace(text[length]))
                length++;

            return length;
        }

        // offset of the first occurrence of any needle
        // must include '\0' so the scan terminates at the end of the text
        template<char... Needles>
        [[nodiscard]] inline i32 findAnyOf(const char* text) noexcept
        {
            static_assert(((Needles == '\0') || ...), "needles must include the NUL terminator");

            i32 offset = 0;
            while (((text[offset] != Needles) && ...))
                offset++;

            return offset;
        }

        // offset of the '*' of the first "*/", or of the terminating NUL if there is none
        [[nodiscard]] inline i32 findStarSlashOrEOF(const char* text) noexcept
        {
            i32 offset = 0;
            while (text[offset] != '\0')
            {
                if (text[offset] == '*' && text[offset + 1] == '/')
                    break;

                offset++;
            }

            return offset;
        }
    }

#if defined(CARACAL_LEXER_SSE2)
    namespace Sse2
    {
        namespace Detail
        {
            [[nodiscard]] inline __m128i loadBytes(const char* text) noexcept
            {
                // unaligned 16 byte load, SourceText::TailPaddingSize makes this safe
                return _mm_loadu_si128(reinterpret_cast<const __m128i*>(text));
            }

            // 0xFF for bytes inside [low, high] treated as unsigned, 0x00 otherwise
            [[nodiscard]] inline __m128i inRange(__m128i bytes, char low, char high) noexcept
            {
                // shift the range down to start at zero: in-range bytes wrap to 0..(high - low),
                // everything else wraps to a larger value
                const auto shifted = _mm_sub_epi8(bytes, _mm_set1_epi8(low));

                // saturating unsigned subtract of the range width leaves exactly 0 for
                // in-range bytes and a non-zero remainder for everything else
                const auto excess = _mm_subs_epu8(shifted, _mm_set1_epi8(static_cast<char>(high - low)));

                // compare against zero to turn "no excess" into an all-ones lane mask
                return _mm_cmpeq_epi8(excess, _mm_setzero_si128());
            }

            // 0xFF for identifier bytes ([A-Za-z0-9_]), 0x00 otherwise
            [[nodiscard]] inline __m128i identifierBytes(__m128i bytes) noexcept
            {
                // OR-ing 0x20 folds upper case onto lower case ('A' 0x41 -> 'a' 0x61); only
                // letters land in [a-z] afterwards, no other byte maps into that range
                const auto lowerCased = _mm_or_si128(bytes, _mm_set1_epi8(0x20));
                const auto letters = inRange(lowerCased, 'a', 'z');
                const auto digits = inRange(bytes, '0', '9');
                const auto underscores = _mm_cmpeq_epi8(bytes, _mm_set1_epi8('_'));

                // a byte is an identifier char if any of the three classes matched it
                return _mm_or_si128(_mm_or_si128(letters, digits), underscores);
            }
        }

        [[nodiscard]] inline i32 identifierRunLength(const char* text) noexcept
        {
            i32 length = 0;
            while (true)
            {
                // classify 16 bytes at once, then pack the 16 lane masks into the low
                // 16 bits of a general-purpose register (bit n = byte n matched)
                const auto matches = Detail::identifierBytes(Detail::loadBytes(text + length));
                const auto mask = static_cast<u32>(_mm_movemask_epi8(matches));

                // anything but all-ones means the run ends inside this chunk: invert to get
                // the non-matching bytes and count trailing zeros to find the first one
                if (mask != 0xFFFF)
                    return length + std::countr_zero(~mask & 0xFFFF);

                length += 16;
            }
        }

        [[nodiscard]] inline i32 digitRunLength(const char* text) noexcept
        {
            i32 length = 0;
            while (true)
            {
                // same pattern as identifierRunLength with a single-range classification
                const auto matches = Detail::inRange(Detail::loadBytes(text + length), '0', '9');
                const auto mask = static_cast<u32>(_mm_movemask_epi8(matches));
                if (mask != 0xFFFF)
                    return length + std::countr_zero(~mask & 0xFFFF);

                length += 16;
            }
        }

        [[nodiscard]] inline i32 whitespaceRunLength(const char* text) noexcept
        {
            i32 length = 0;
            while (true)
            {
                // four exact-match masks instead of an inRange over 0x09..0x0D, because
                // \v and \f are NOT whitespace to the lexer (they are illegal characters)
                const auto bytes = Detail::loadBytes(text + length);
                const auto spaces = _mm_cmpeq_epi8(bytes, _mm_set1_epi8(' '));
                const auto tabs = _mm_cmpeq_epi8(bytes, _mm_set1_epi8('\t'));
                const auto carriageReturns = _mm_cmpeq_epi8(bytes, _mm_set1_epi8('\r'));
                const auto lineFeeds = _mm_cmpeq_epi8(bytes, _mm_set1_epi8('\n'));
                const auto matches = _mm_or_si128(_mm_or_si128(spaces, tabs), _mm_or_si128(carriageReturns, lineFeeds));

                // pack lane masks into bits and find the first non-whitespace byte
                const auto mask = static_cast<u32>(_mm_movemask_epi8(matches));
                if (mask != 0xFFFF)
                    return length + std::countr_zero(~mask & 0xFFFF);

                length += 16;
            }
        }

        template<char... Needles>
        [[nodiscard]] inline i32 findAnyOf(const char* text) noexcept
        {
            static_assert(((Needles == '\0') || ...), "needles must include the NUL terminator");

            i32 offset = 0;
            while (true)
            {
                // one exact-match mask per needle, OR-ed together by the fold expression
                // a set bit in the packed mask then means we found a needle
                const auto bytes = Detail::loadBytes(text + offset);
                auto matches = _mm_setzero_si128();
                ((matches = _mm_or_si128(matches, _mm_cmpeq_epi8(bytes, _mm_set1_epi8(Needles)))), ...);

                // the first set bit is the first needle occurrence, the NUL needle
                // guarantees this loop terminates at the end of the text
                const auto mask = static_cast<u32>(_mm_movemask_epi8(matches));
                if (mask != 0)
                    return offset + std::countr_zero(mask);

                offset += 16;
            }
        }

        [[nodiscard]] inline i32 findStarSlashOrEOF(const char* text) noexcept
        {
            i32 offset = 0;
            while (true)
            {
                // candidates are '*' bytes (possible comment close) and NUL (end of text)
                const auto bytes = Detail::loadBytes(text + offset);
                const auto stars = _mm_cmpeq_epi8(bytes, _mm_set1_epi8('*'));
                const auto nuls = _mm_cmpeq_epi8(bytes, _mm_setzero_si128());
                auto mask = static_cast<u32>(_mm_movemask_epi8(_mm_or_si128(stars, nuls)));

                // walk the candidate bits lowest-first
                // clearing the lowest set bit with mask & (mask - 1) steps to the next candidate in this chunk
                while (mask != 0)
                {
                    // reading one byte past a candidate is safe because of the padding
                    const auto candidate = offset + std::countr_zero(mask);
                    if (text[candidate] == '\0' || text[candidate + 1] == '/')
                        return candidate;

                    mask &= mask - 1;
                }

                offset += 16;
            }
        }
    }

    using Sse2::identifierRunLength;
    using Sse2::digitRunLength;
    using Sse2::whitespaceRunLength;
    using Sse2::findAnyOf;
    using Sse2::findStarSlashOrEOF;
#else
    using Scalar::identifierRunLength;
    using Scalar::digitRunLength;
    using Scalar::whitespaceRunLength;
    using Scalar::findAnyOf;
    using Scalar::findStarSlashOrEOF;
#endif
}

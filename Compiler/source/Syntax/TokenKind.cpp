#include <Syntax/TokenKind.h>
#include <unordered_map>

QString Stringify(TokenKind kind, bool /*quoteStrings*/)
{
    static const std::unordered_map<TokenKind, QString> kindToString{
        { TokenKind::Unknown,           QStringLiteral("Unknown") },
        { TokenKind::Error,             QStringLiteral("Error") },
        { TokenKind::Plus,              QStringLiteral("Plus") },
        { TokenKind::Minus,             QStringLiteral("Minus") },
        { TokenKind::Star,              QStringLiteral("Star") },
        { TokenKind::Slash,             QStringLiteral("Slash") },
        { TokenKind::Dot,               QStringLiteral("Dot") },
        { TokenKind::Colon,             QStringLiteral("Colon") },
        { TokenKind::Semicolon,         QStringLiteral("Semicolon") },
        { TokenKind::Comma,             QStringLiteral("Comma") },
        { TokenKind::Equal,             QStringLiteral("Equal") },
        { TokenKind::EqualEqual,        QStringLiteral("EqualEqual") },
        { TokenKind::Bang,              QStringLiteral("Bang") },
        { TokenKind::BangEqual,         QStringLiteral("BangEqual") },
        { TokenKind::LessThan,          QStringLiteral("LessThan") },
        { TokenKind::LessThanEqual,     QStringLiteral("LessThanEqual") },
        { TokenKind::GreaterThan,       QStringLiteral("GreaterThan") },
        { TokenKind::GreaterThanEqual,  QStringLiteral("GreaterThanEqual") },
        { TokenKind::LessThan,          QStringLiteral("LessThan") },
        { TokenKind::GreaterThan,       QStringLiteral("GreaterThan") },
        { TokenKind::Underscore,        QStringLiteral("Underscore") },
        { TokenKind::OpenParenthesis,   QStringLiteral("OpenParenthesis") },
        { TokenKind::CloseParenthesis,  QStringLiteral("CloseParenthesis") },
        { TokenKind::OpenBracket,       QStringLiteral("OpenBracket") },
        { TokenKind::CloseBracket,      QStringLiteral("CloseBracket") },
        { TokenKind::Identifier,        QStringLiteral("Identifier") },
        { TokenKind::Number,            QStringLiteral("Number") },
        { TokenKind::String,            QStringLiteral("String") },
        { TokenKind::DefKeyword,        QStringLiteral("DefKeyword") },
        { TokenKind::EnumKeyword,       QStringLiteral("EnumKeyword") },
        { TokenKind::TypeKeyword,       QStringLiteral("TypeKeyword") },
        { TokenKind::IfKeyword,         QStringLiteral("IfKeyword") },
        { TokenKind::ElseKeyword,       QStringLiteral("ElseKeyword") },
        { TokenKind::WhileKeyword,      QStringLiteral("WhileKeyword") },
        { TokenKind::BreakKeyword,      QStringLiteral("BreakKeyword") },
        { TokenKind::SkipKeyword,       QStringLiteral("SkipKeyword") },
        { TokenKind::ReturnKeyword,     QStringLiteral("ReturnKeyword") },
        { TokenKind::TrueKeyword,       QStringLiteral("TrueKeyword") },
        { TokenKind::FalseKeyword,      QStringLiteral("FalseKeyword") },
        { TokenKind::AndKeyword,        QStringLiteral("AndKeyword") },
        { TokenKind::OrKeyword,         QStringLiteral("OrKeyword") },
        { TokenKind::RefKeyword,        QStringLiteral("RefKeyword") },
        { TokenKind::CppKeyword,        QStringLiteral("CppKeyword") },
        { TokenKind::EndOfFile,         QStringLiteral("EndOfFile") },
    };

    const auto it = kindToString.find(kind);
    if (it != kindToString.end())
        return it->second;

    TODO("String for TokenKind value was not defined yet");
    return QString();
}

i32 unaryOperatorPrecedence(TokenKind kind)
{
    switch (kind)
    {
        case TokenKind::RefKeyword:
            return 7;
        case TokenKind::Bang:
        case TokenKind::Minus:
            return 6;
        default:
            return 0;
    }
}

i32 binaryOperatorPrecedence(TokenKind kind)
{
    switch (kind)
    {
        case TokenKind::Dot:
            return 5;
        case TokenKind::Star:
        case TokenKind::Slash:
            return 4;
        case TokenKind::Plus:
        case TokenKind::Minus:
            return 3;
        case TokenKind::EqualEqual:
        case TokenKind::BangEqual:
        case TokenKind::LessThan:
        case TokenKind::LessThanEqual:
        case TokenKind::GreaterThan:
        case TokenKind::GreaterThanEqual:
            return 2;
        case TokenKind::AndKeyword:
        case TokenKind::OrKeyword:
            return 1;
        default:
            return 0;
    }
}

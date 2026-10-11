#include "Tokenizer.hpp"
#include <cctype>
#include <sstream>
#include <iomanip>

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Number:     return "Number";
        case TokenType::Plus:       return "Plus (+)";
        case TokenType::Minus:      return "Minus (-)";
        case TokenType::Multiply:   return "Multiply (*)";
        case TokenType::Divide:     return "Divide (/)";
        case TokenType::LeftParen:  return "LeftParen '('";
        case TokenType::RightParen: return "RightParen ')'";
        case TokenType::EndOfFile:  return "EndOfFile";
        default:                    return "Unknown";
    }
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << tokenTypeToString(type);
    if (type == TokenType::Number) {
        oss << ", val=" << value;
    }
    oss << ", lexeme='" << lexeme << "', pos=" << position << ")";
    return oss.str();
}

Tokenizer::Tokenizer(std::string input)
    : m_input(std::move(input)), m_cursor(0) {}

char Tokenizer::peek() const noexcept {
    if (isAtEnd()) {
        return '\0';
    }
    return m_input[m_cursor];
}

char Tokenizer::peekNext() const noexcept {
    if (m_cursor + 1 >= m_input.size()) {
        return '\0';
    }
    return m_input[m_cursor + 1];
}

char Tokenizer::advance() noexcept {
    if (isAtEnd()) {
        return '\0';
    }
    return m_input[m_cursor++];
}

bool Tokenizer::isAtEnd() const noexcept {
    return m_cursor >= m_input.size();
}

void Tokenizer::skipWhitespace() noexcept {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else {
            break;
        }
    }
}

Token Tokenizer::scanNumber() {
    size_t startPos = m_cursor;
    bool hasDecimalPoint = false;

    // Check if number starts with leading decimal point e.g. .5
    if (peek() == '.') {
        hasDecimalPoint = true;
        advance();
    }

    while (!isAtEnd()) {
        char c = peek();
        if (std::isdigit(static_cast<unsigned char>(c))) {
            advance();
        } else if (c == '.') {
            if (hasDecimalPoint) {
                // Second decimal point is an error in numeric literal e.g. 1.2.3
                size_t errorPos = m_cursor + 1; // 1-based
                throw LexerException("Malformed number with multiple decimal points at position " +
                                     std::to_string(errorPos), errorPos);
            }
            hasDecimalPoint = true;
            advance();
        } else {
            break;
        }
    }

    std::string lexeme = m_input.substr(startPos, m_cursor - startPos);

    // If lexeme is just ".", that is an error
    if (lexeme == ".") {
        size_t errorPos = startPos + 1;
        throw LexerException("Unexpected solitary decimal point '.' at position " +
                             std::to_string(errorPos), errorPos);
    }

    // Convert string to double
    double val = 0.0;
    try {
        val = std::stod(lexeme);
    } catch (const std::exception&) {
        size_t errorPos = startPos + 1;
        throw LexerException("Failed to parse number literal '" + lexeme + "' at position " +
                             std::to_string(errorPos), errorPos);
    }

    return Token(TokenType::Number, val, lexeme, startPos + 1); // 1-based position
}

std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        skipWhitespace();

        if (isAtEnd()) {
            tokens.emplace_back(TokenType::EndOfFile, 0.0, "", m_cursor + 1);
            break;
        }

        size_t tokenPos = m_cursor + 1; // 1-based column position
        char c = peek();

        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            tokens.push_back(scanNumber());
            continue;
        }

        advance(); // Consume the character
        switch (c) {
            case '+':
                tokens.emplace_back(TokenType::Plus, 0.0, "+", tokenPos);
                break;
            case '-':
                tokens.emplace_back(TokenType::Minus, 0.0, "-", tokenPos);
                break;
            case '*':
                tokens.emplace_back(TokenType::Multiply, 0.0, "*", tokenPos);
                break;
            case '/':
                tokens.emplace_back(TokenType::Divide, 0.0, "/", tokenPos);
                break;
            case '(':
                tokens.emplace_back(TokenType::LeftParen, 0.0, "(", tokenPos);
                break;
            case ')':
                tokens.emplace_back(TokenType::RightParen, 0.0, ")", tokenPos);
                break;
            default: {
                std::string msg = "Unexpected character '";
                msg += c;
                msg += "' at position " + std::to_string(tokenPos);
                throw LexerException(msg, tokenPos);
            }
        }
    }

    return tokens;
}

#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <cstddef>

/**
 * @brief Represents the kind of lexical token scanned from the input expression.
 */
enum class TokenType {
    Number,      // Numeric literal (integer or floating-point, e.g., 42, 3.14)
    Plus,        // Operator '+'
    Minus,       // Operator '-'
    Multiply,    // Operator '*'
    Divide,      // Operator '/'
    LeftParen,   // Parenthesis '('
    RightParen,  // Parenthesis ')'
    EndOfFile    // End of input stream (EOF)
};

/**
 * @brief Converts a TokenType enum to a human-readable string for debugging and error reporting.
 */
std::string tokenTypeToString(TokenType type);

/**
 * @brief Represents a single token emitted by the lexical analysis phase.
 */
struct Token {
    TokenType type;       // Type of the token
    double value;         // Numeric value if type == TokenType::Number; otherwise 0.0
    std::string lexeme;   // Raw text representation from the source string
    size_t position;      // 1-based column position in the input string

    Token(TokenType t, double val, std::string text, size_t pos)
        : type(t), value(val), lexeme(std::move(text)), position(pos) {}

    std::string toString() const;
};

/**
 * @brief Exception thrown when lexical analysis encounters an illegal character.
 */
class LexerException : public std::runtime_error {
public:
    LexerException(const std::string& message, size_t position)
        : std::runtime_error(message), m_position(position) {}

    size_t getPosition() const noexcept { return m_position; }

private:
    size_t m_position;
};

/**
 * @brief Tokenizer (Lexer) that transforms a raw arithmetic string into a sequence of Tokens.
 */
class Tokenizer {
public:
    explicit Tokenizer(std::string input);

    /**
     * @brief Scans and returns all tokens from the input string up to and including EndOfFile.
     * @throws LexerException if an unrecognized character or malformed number is encountered.
     */
    std::vector<Token> tokenize();

private:
    char peek() const noexcept;
    char peekNext() const noexcept;
    char advance() noexcept;
    bool isAtEnd() const noexcept;
    void skipWhitespace() noexcept;
    Token scanNumber();

    std::string m_input;
    size_t m_cursor{0}; // Current byte index (0-based) in m_input
};

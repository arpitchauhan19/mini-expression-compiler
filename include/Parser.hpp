#pragma once

#include "Tokenizer.hpp"
#include "AST.hpp"
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>

/**
 * @brief Exception thrown during parsing when syntax or grammatical structure is invalid.
 */
class ParseException : public std::runtime_error {
public:
    ParseException(const std::string& message, size_t position, std::string lexeme = "")
        : std::runtime_error(message), m_position(position), m_lexeme(std::move(lexeme)) {}

    size_t getPosition() const noexcept { return m_position; }
    const std::string& getLexeme() const noexcept { return m_lexeme; }

    /**
     * @brief Formats an error banner showing the offending line and a caret pointing to the error column.
     */
    std::string formatErrorWithContext(const std::string& sourceLine) const;

private:
    size_t m_position;
    std::string m_lexeme;
};

/**
 * @brief Recursive-descent parser that transforms a sequence of Tokens into an Abstract Syntax Tree (AST).
 *
 * Grammar:
 *   Expression ::= Term ( ( '+' | '-' ) Term )*
 *   Term       ::= Factor ( ( '*' | '/' ) Factor )*
 *   Factor     ::= ( '+' | '-' ) Factor | Primary
 *   Primary    ::= NUMBER | '(' Expression ')'
 */
class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    /**
     * @brief Parses the token stream and returns the root of the AST.
     * @throws ParseException if the syntax is invalid or tokens remain unconsumed.
     */
    std::unique_ptr<ASTNode> parse();

private:
    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    const Token& consume(TokenType type, const std::string& errorMessage);

    // Recursive-descent grammar methods
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parsePrimary();

    std::vector<Token> m_tokens;
    size_t m_cursor{0};
};

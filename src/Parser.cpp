#include "Parser.hpp"
#include <sstream>

std::string ParseException::formatErrorWithContext(const std::string& sourceLine) const {
    std::ostringstream oss;
    oss << "Syntax Error: " << what() << "\n";
    oss << "  " << sourceLine << "\n";
    oss << "  ";
    if (m_position > 1) {
        oss << std::string(m_position - 1, ' ');
    }
    oss << "^\n";
    return oss.str();
}

Parser::Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens)), m_cursor(0) {}

const Token& Parser::peek() const {
    if (m_cursor >= m_tokens.size()) {
        return m_tokens.back(); // EndOfFile token
    }
    return m_tokens[m_cursor];
}

const Token& Parser::previous() const {
    if (m_cursor == 0) {
        return m_tokens[0];
    }
    return m_tokens[m_cursor - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

const Token& Parser::advance() {
    if (!isAtEnd()) {
        m_cursor++;
    }
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd() && type != TokenType::EndOfFile) {
        return false;
    }
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

const Token& Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) {
        return advance();
    }
    const Token& current = peek();
    throw ParseException(errorMessage, current.position, current.lexeme);
}

std::unique_ptr<ASTNode> Parser::parse() {
    if (m_tokens.empty() || m_tokens[0].type == TokenType::EndOfFile) {
        throw ParseException("Empty expression: no tokens to evaluate", 1);
    }

    auto root = parseExpression();

    if (!isAtEnd()) {
        const Token& trailing = peek();
        throw ParseException("Unexpected trailing token '" + trailing.lexeme +
                             "' at position " + std::to_string(trailing.position),
                             trailing.position, trailing.lexeme);
    }

    return root;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto node = parseTerm();

    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        TokenType opType = previous().type;
        char opChar = (opType == TokenType::Plus) ? '+' : '-';
        size_t opPos = previous().position;

        if (isAtEnd()) {
            throw ParseException(std::string("Expected operand after operator '") +
                                 opChar + "' at position " + std::to_string(opPos),
                                 opPos);
        }

        auto right = parseTerm();
        node = std::make_unique<BinaryOpNode>(opChar, std::move(node), std::move(right));
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto node = parseFactor();

    while (match(TokenType::Multiply) || match(TokenType::Divide)) {
        TokenType opType = previous().type;
        char opChar = (opType == TokenType::Multiply) ? '*' : '/';
        size_t opPos = previous().position;

        if (isAtEnd()) {
            throw ParseException(std::string("Expected operand after operator '") +
                                 opChar + "' at position " + std::to_string(opPos),
                                 opPos);
        }

        auto right = parseFactor();
        node = std::make_unique<BinaryOpNode>(opChar, std::move(node), std::move(right));
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    // Unary operator '+' or '-'
    if (match(TokenType::Plus)) {
        return std::make_unique<UnaryOpNode>('+', parseFactor());
    }
    if (match(TokenType::Minus)) {
        return std::make_unique<UnaryOpNode>('-', parseFactor());
    }

    return parsePrimary();
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    // Numeric literal
    if (match(TokenType::Number)) {
        return std::make_unique<NumberNode>(previous().value);
    }

    // Parenthesized sub-expression
    if (match(TokenType::LeftParen)) {
        size_t openParenPos = previous().position;

        // Detect empty parentheses ()
        if (check(TokenType::RightParen)) {
            size_t closePos = peek().position;
            throw ParseException("Empty parentheses '()' are not allowed at position " +
                                 std::to_string(closePos), closePos, ")");
        }

        auto expr = parseExpression();

        if (!match(TokenType::RightParen)) {
            const Token& errToken = peek();
            throw ParseException("Missing closing parenthesis ')' matching '(' from position " +
                                 std::to_string(openParenPos),
                                 errToken.position, errToken.lexeme);
        }

        return expr;
    }

    // Unmatched closing parenthesis
    if (match(TokenType::RightParen)) {
        throw ParseException("Unexpected closing parenthesis ')' without opening '(' at position " +
                             std::to_string(previous().position),
                             previous().position, ")");
    }

    // End of file when a factor was expected
    if (isAtEnd()) {
        throw ParseException("Unexpected end of expression: expected number or '(' at position " +
                             std::to_string(peek().position),
                             peek().position);
    }

    // Any other unexpected token
    const Token& unexpected = peek();
    throw ParseException("Unexpected token '" + unexpected.lexeme +
                         "' at position " + std::to_string(unexpected.position) +
                         ": expected number or '('",
                         unexpected.position, unexpected.lexeme);
}

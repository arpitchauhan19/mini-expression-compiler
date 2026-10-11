#pragma once

#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "AST.hpp"
#include "Bytecode.hpp"
#include <string>
#include <vector>
#include <memory>

/**
 * @brief Encapsulates the entire result of compiling and evaluating an expression.
 */
struct CompilationResult {
    bool success{false};
    double value{0.0};
    std::vector<Token> tokens;
    std::unique_ptr<ASTNode> ast;
    std::vector<Instruction> bytecode;
    std::string errorMessage;
    size_t errorPosition{0};
};

/**
 * @brief High-level facade orchestrating tokenization, parsing, AST generation,
 *        bytecode compilation, and dual-mode evaluation (Tree-walk & Stack VM).
 */
class Compiler {
public:
    /**
     * @brief Performs full end-to-end compilation and evaluation with both Tree-walk and Stack VM.
     */
    static CompilationResult compile(const std::string& expression);

    /**
     * @brief Convenient helper that evaluates an expression and returns the double result.
     * @throws LexerException, ParseException, or RuntimeError on failure.
     */
    static double evaluate(const std::string& expression);

    /**
     * @brief Generates bytecode for an expression string.
     */
    static std::vector<Instruction> compileToBytecode(const std::string& expression);
};

#pragma once

#include "Bytecode.hpp"
#include <memory>
#include <string>
#include <ostream>
#include <vector>

/**
 * @brief Base class for all nodes in the Abstract Syntax Tree (AST).
 */
class ASTNode {
public:
    virtual ~ASTNode() = default;

    /**
     * @brief Evaluates the expression directly via recursive tree-walk.
     * @throws RuntimeError on division by zero or invalid operation.
     */
    virtual double evaluate() const = 0;

    /**
     * @brief Returns a prefix S-expression representation (e.g. "(+ 10 (* 2 3))").
     */
    virtual std::string toSExpression() const = 0;

    /**
     * @brief Prints the subtree as a formatted ASCII tree.
     */
    virtual void printTree(std::ostream& os, const std::string& prefix = "", bool isTail = true) const = 0;

    /**
     * @brief Emits stack machine bytecode via post-order AST traversal.
     */
    virtual void emitBytecode(std::vector<Instruction>& program) const = 0;
};

/**
 * @brief Represents a numeric literal leaf node (e.g. 42 or 3.14).
 */
class NumberNode : public ASTNode {
public:
    explicit NumberNode(double value);
    double getValue() const noexcept { return m_value; }

    double evaluate() const override;
    std::string toSExpression() const override;
    void printTree(std::ostream& os, const std::string& prefix = "", bool isTail = true) const override;
    void emitBytecode(std::vector<Instruction>& program) const override;

private:
    double m_value;
};

/**
 * @brief Represents a binary operation node (+, -, *, /) with left and right subtrees.
 */
class BinaryOpNode : public ASTNode {
public:
    BinaryOpNode(char op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);

    char getOp() const noexcept { return m_op; }
    const ASTNode* getLeft() const noexcept { return m_left.get(); }
    const ASTNode* getRight() const noexcept { return m_right.get(); }

    double evaluate() const override;
    std::string toSExpression() const override;
    void printTree(std::ostream& os, const std::string& prefix = "", bool isTail = true) const override;
    void emitBytecode(std::vector<Instruction>& program) const override;

private:
    char m_op;
    std::unique_ptr<ASTNode> m_left;
    std::unique_ptr<ASTNode> m_right;
};

/**
 * @brief Represents a unary operation node (+ or -) with a single child operand.
 */
class UnaryOpNode : public ASTNode {
public:
    UnaryOpNode(char op, std::unique_ptr<ASTNode> operand);

    char getOp() const noexcept { return m_op; }
    const ASTNode* getOperand() const noexcept { return m_operand.get(); }

    double evaluate() const override;
    std::string toSExpression() const override;
    void printTree(std::ostream& os, const std::string& prefix = "", bool isTail = true) const override;
    void emitBytecode(std::vector<Instruction>& program) const override;

private:
    char m_op;
    std::unique_ptr<ASTNode> m_operand;
};

#include "AST.hpp"
#include <sstream>
#include <cmath>

static std::string formatDouble(double val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

// -------------------------------------------------------------
// NumberNode Implementation
// -------------------------------------------------------------

NumberNode::NumberNode(double value)
    : m_value(value) {}

double NumberNode::evaluate() const {
    return m_value;
}

std::string NumberNode::toSExpression() const {
    return formatDouble(m_value);
}

void NumberNode::printTree(std::ostream& os, const std::string& prefix, bool isTail) const {
    os << prefix << (isTail ? "└── " : "├── ")
       << "Number(" << formatDouble(m_value) << ")\n";
}

void NumberNode::emitBytecode(std::vector<Instruction>& program) const {
    program.emplace_back(OpCode::Push, m_value);
}

// -------------------------------------------------------------
// BinaryOpNode Implementation
// -------------------------------------------------------------

BinaryOpNode::BinaryOpNode(char op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
    : m_op(op), m_left(std::move(left)), m_right(std::move(right)) {}

double BinaryOpNode::evaluate() const {
    double l = m_left->evaluate();
    double r = m_right->evaluate();

    switch (m_op) {
        case '+':
            return l + r;
        case '-':
            return l - r;
        case '*':
            return l * r;
        case '/':
            if (r == 0.0) {
                throw RuntimeError("Runtime error: Division by zero");
            }
            return l / r;
        default:
            throw RuntimeError(std::string("Unknown binary operator '") + m_op + "'");
    }
}

std::string BinaryOpNode::toSExpression() const {
    return "(" + std::string(1, m_op) + " " +
           m_left->toSExpression() + " " +
           m_right->toSExpression() + ")";
}

void BinaryOpNode::printTree(std::ostream& os, const std::string& prefix, bool isTail) const {
    os << prefix << (isTail ? "└── " : "├── ")
       << "BinaryOp(" << m_op << ")\n";

    std::string childPrefix = prefix + (isTail ? "    " : "│   ");
    if (m_left) {
        m_left->printTree(os, childPrefix, false);
    }
    if (m_right) {
        m_right->printTree(os, childPrefix, true);
    }
}

void BinaryOpNode::emitBytecode(std::vector<Instruction>& program) const {
    // Post-order: emit left child, emit right child, then emit operator instruction
    m_left->emitBytecode(program);
    m_right->emitBytecode(program);

    switch (m_op) {
        case '+':
            program.emplace_back(OpCode::Add);
            break;
        case '-':
            program.emplace_back(OpCode::Subtract);
            break;
        case '*':
            program.emplace_back(OpCode::Multiply);
            break;
        case '/':
            program.emplace_back(OpCode::Divide);
            break;
        default:
            throw RuntimeError(std::string("Unknown operator for bytecode emission: ") + m_op);
    }
}

// -------------------------------------------------------------
// UnaryOpNode Implementation
// -------------------------------------------------------------

UnaryOpNode::UnaryOpNode(char op, std::unique_ptr<ASTNode> operand)
    : m_op(op), m_operand(std::move(operand)) {}

double UnaryOpNode::evaluate() const {
    double val = m_operand->evaluate();
    if (m_op == '-') {
        return -val;
    }
    if (m_op == '+') {
        return val;
    }
    throw RuntimeError(std::string("Unknown unary operator '") + m_op + "'");
}

std::string UnaryOpNode::toSExpression() const {
    return "(" + std::string(1, m_op) + " " + m_operand->toSExpression() + ")";
}

void UnaryOpNode::printTree(std::ostream& os, const std::string& prefix, bool isTail) const {
    os << prefix << (isTail ? "└── " : "├── ")
       << "UnaryOp(" << m_op << ")\n";

    std::string childPrefix = prefix + (isTail ? "    " : "│   ");
    if (m_operand) {
        m_operand->printTree(os, childPrefix, true);
    }
}

void UnaryOpNode::emitBytecode(std::vector<Instruction>& program) const {
    m_operand->emitBytecode(program);
    if (m_op == '-') {
        program.emplace_back(OpCode::Negate);
    }
    // Unary '+' is a no-op in terms of stack execution
}

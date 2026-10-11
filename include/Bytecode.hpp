#pragma once

#include <string>
#include <vector>
#include <stdexcept>

/**
 * @brief Opcodes for the stack-based virtual machine.
 */
enum class OpCode {
    Push,       // Pushes a constant operand onto the evaluation stack
    Add,        // Pops 2 values, pushes (b + a)
    Subtract,   // Pops 2 values, pushes (left - right)
    Multiply,   // Pops 2 values, pushes (left * right)
    Divide,     // Pops 2 values, pushes (left / right)
    Negate      // Pops 1 value, pushes (-value)
};

/**
 * @brief Represents a single bytecode instruction.
 */
struct Instruction {
    OpCode op;
    double operand{0.0}; // Used only for OpCode::Push

    Instruction(OpCode operation, double val = 0.0)
        : op(operation), operand(val) {}

    std::string toString() const;
};

/**
 * @brief Exception thrown during bytecode execution (e.g. division by zero, stack underflow).
 */
class RuntimeError : public std::runtime_error {
public:
    explicit RuntimeError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief A stack-based virtual machine that executes linear bytecode instructions.
 */
class StackMachine {
public:
    StackMachine() = default;

    /**
     * @brief Executes a sequence of bytecode instructions and returns the final top-of-stack result.
     * @throws RuntimeError on division by zero or invalid stack state.
     */
    double execute(const std::vector<Instruction>& program);

    /**
     * @brief Disassembles a bytecode program into readable assembly-like text.
     */
    static std::string disassemble(const std::vector<Instruction>& program);
};

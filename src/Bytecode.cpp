#include "Bytecode.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

std::string Instruction::toString() const {
    std::ostringstream oss;
    switch (op) {
        case OpCode::Push:
            oss << "PUSH " << operand;
            break;
        case OpCode::Add:
            oss << "ADD";
            break;
        case OpCode::Subtract:
            oss << "SUB";
            break;
        case OpCode::Multiply:
            oss << "MUL";
            break;
        case OpCode::Divide:
            oss << "DIV";
            break;
        case OpCode::Negate:
            oss << "NEG";
            break;
    }
    return oss.str();
}

double StackMachine::execute(const std::vector<Instruction>& program) {
    if (program.empty()) {
        throw RuntimeError("Cannot execute empty bytecode program");
    }

    std::vector<double> stack;

    for (const auto& instr : program) {
        switch (instr.op) {
            case OpCode::Push: {
                stack.push_back(instr.operand);
                break;
            }
            case OpCode::Add: {
                if (stack.size() < 2) {
                    throw RuntimeError("Stack underflow during ADD");
                }
                double rhs = stack.back();
                stack.pop_back();
                double lhs = stack.back();
                stack.pop_back();
                stack.push_back(lhs + rhs);
                break;
            }
            case OpCode::Subtract: {
                if (stack.size() < 2) {
                    throw RuntimeError("Stack underflow during SUB");
                }
                double rhs = stack.back();
                stack.pop_back();
                double lhs = stack.back();
                stack.pop_back();
                stack.push_back(lhs - rhs);
                break;
            }
            case OpCode::Multiply: {
                if (stack.size() < 2) {
                    throw RuntimeError("Stack underflow during MUL");
                }
                double rhs = stack.back();
                stack.pop_back();
                double lhs = stack.back();
                stack.pop_back();
                stack.push_back(lhs * rhs);
                break;
            }
            case OpCode::Divide: {
                if (stack.size() < 2) {
                    throw RuntimeError("Stack underflow during DIV");
                }
                double rhs = stack.back();
                stack.pop_back();
                double lhs = stack.back();
                stack.pop_back();
                if (rhs == 0.0) {
                    throw RuntimeError("Runtime error: Division by zero");
                }
                stack.push_back(lhs / rhs);
                break;
            }
            case OpCode::Negate: {
                if (stack.empty()) {
                    throw RuntimeError("Stack underflow during NEG");
                }
                double val = stack.back();
                stack.pop_back();
                stack.push_back(-val);
                break;
            }
        }
    }

    if (stack.size() != 1) {
        throw RuntimeError("Invalid execution state: stack contains " +
                           std::to_string(stack.size()) + " values upon program completion");
    }

    return stack.back();
}

std::string StackMachine::disassemble(const std::vector<Instruction>& program) {
    std::ostringstream oss;
    oss << "; Bytecode Listing (" << program.size() << " instructions)\n";
    for (size_t i = 0; i < program.size(); ++i) {
        oss << "  " << std::setw(3) << std::right << i << ":  "
            << program[i].toString() << "\n";
    }
    return oss.str();
}

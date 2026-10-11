#include "Compiler.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

static void printHelp(const char* progName) {
    std::cout << "Mini Expression Compiler (Modern C++17)\n"
              << "========================================\n"
              << "Usage:\n"
              << "  " << progName << "                       Launch interactive REPL\n"
              << "  " << progName << " \"<expr>\"               Evaluate an expression directly\n"
              << "  " << progName << " --tokens \"<expr>\"     Display tokenization stream\n"
              << "  " << progName << " --ast \"<expr>\"        Display Abstract Syntax Tree\n"
              << "  " << progName << " --bytecode \"<expr>\"   Display compiled stack bytecode\n"
              << "  " << progName << " --all \"<expr>\"        Display full compilation pipeline\n"
              << "  " << progName << " --help                 Show this help message\n\n"
              << "Examples:\n"
              << "  " << progName << " \"10 + 2 * 3\"\n"
              << "  " << progName << " --all \"(10 + 2) * -3\"\n";
}

static void printErrorWithCaret(const std::string& input, size_t position, const std::string& message) {
    std::cerr << "\n" << message << "\n";
    std::cerr << "  " << input << "\n";
    if (position > 0 && position <= input.length() + 1) {
        std::cerr << "  " << std::string(position - 1, ' ') << "^\n";
    }
}

static void processExpression(const std::string& input, bool showTokens, bool showAST, bool showBytecode) {
    auto result = Compiler::compile(input);

    if (showTokens && !result.tokens.empty()) {
        std::cout << "\n[1. Tokenization Stream]\n";
        for (const auto& token : result.tokens) {
            std::cout << "  " << token.toString() << "\n";
        }
    }

    if (showAST && result.ast) {
        std::cout << "\n[2. Abstract Syntax Tree]\n";
        std::cout << "  S-Expression: " << result.ast->toSExpression() << "\n";
        std::cout << "  Tree Hierarchy:\n";
        result.ast->printTree(std::cout, "  ");
    }

    if (showBytecode && !result.bytecode.empty()) {
        std::cout << "\n[3. Stack-Machine Bytecode]\n";
        std::cout << StackMachine::disassemble(result.bytecode);
    }

    if (result.success) {
        std::cout << "\nResult: " << result.value << "\n";
    } else {
        printErrorWithCaret(input, result.errorPosition, result.errorMessage);
    }
}

static void runRepl() {
    std::cout << "========================================================\n"
              << " Mini Expression Compiler REPL (C++17)\n"
              << " Built for Compiler Verification & Educational Exploration\n"
              << "========================================================\n"
              << "Commands:\n"
              << "  :tokens    Toggle token stream display\n"
              << "  :ast       Toggle AST tree display\n"
              << "  :bytecode  Toggle bytecode disassembly display\n"
              << "  :all       Toggle all inspection views\n"
              << "  :help      Show help\n"
              << "  :exit      Exit REPL\n"
              << "--------------------------------------------------------\n";

    bool showTokens = false;
    bool showAST = false;
    bool showBytecode = false;

    std::string line;
    while (true) {
        std::cout << "\ncompiler> ";
        if (!std::getline(std::cin, line)) {
            break; // EOF
        }

        if (line.empty()) {
            continue;
        }

        if (line == ":exit" || line == ":quit" || line == "exit" || line == "quit") {
            std::cout << "Exiting. Goodbye!\n";
            break;
        }

        if (line == ":tokens") {
            showTokens = !showTokens;
            std::cout << "Tokens display: " << (showTokens ? "ON" : "OFF") << "\n";
            continue;
        }

        if (line == ":ast") {
            showAST = !showAST;
            std::cout << "AST display: " << (showAST ? "ON" : "OFF") << "\n";
            continue;
        }

        if (line == ":bytecode") {
            showBytecode = !showBytecode;
            std::cout << "Bytecode display: " << (showBytecode ? "ON" : "OFF") << "\n";
            continue;
        }

        if (line == ":all") {
            bool newState = !(showTokens && showAST && showBytecode);
            showTokens = showAST = showBytecode = newState;
            std::cout << "All inspection views: " << (newState ? "ON" : "OFF") << "\n";
            continue;
        }

        if (line == ":help") {
            std::cout << "Enter any valid arithmetic expression, e.g. '10 + 2 * 3' or '-(4.5 / 1.5)'.\n"
                      << "Inspection commands: :tokens, :ast, :bytecode, :all, :exit.\n";
            continue;
        }

        processExpression(line, showTokens, showAST, showBytecode);
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        runRepl();
        return 0;
    }

    std::string arg1 = argv[1];
    if (arg1 == "--help" || arg1 == "-h") {
        printHelp(argv[0]);
        return 0;
    }

    bool showTokens = false;
    bool showAST = false;
    bool showBytecode = false;
    std::string expr;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--tokens") {
            showTokens = true;
        } else if (arg == "--ast") {
            showAST = true;
        } else if (arg == "--bytecode") {
            showBytecode = true;
        } else if (arg == "--all") {
            showTokens = showAST = showBytecode = true;
        } else {
            expr = arg;
        }
    }

    if (expr.empty()) {
        std::cerr << "Error: No expression provided. Run with --help for usage.\n";
        return 1;
    }

    auto result = Compiler::compile(expr);
    if (showTokens || showAST || showBytecode) {
        processExpression(expr, showTokens, showAST, showBytecode);
    } else {
        if (result.success) {
            std::cout << result.value << "\n";
        } else {
            printErrorWithCaret(expr, result.errorPosition, result.errorMessage);
            return 1;
        }
    }

    return result.success ? 0 : 1;
}

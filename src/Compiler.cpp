#include "Compiler.hpp"
#include <cmath>

CompilationResult Compiler::compile(const std::string& expression) {
    CompilationResult result;
    try {
        // Phase 1: Lexical Analysis (Tokenization)
        Tokenizer tokenizer(expression);
        result.tokens = tokenizer.tokenize();

        // Phase 2: Syntactic Analysis (Parsing to AST)
        Parser parser(result.tokens);
        result.ast = parser.parse();

        // Phase 3: Direct Tree-Walk Evaluation
        double treeResult = result.ast->evaluate();

        // Phase 4: Intermediate Code Generation (Stack Bytecode)
        result.ast->emitBytecode(result.bytecode);

        // Phase 5: Stack-Based Virtual Machine Execution
        StackMachine vm;
        double vmResult = vm.execute(result.bytecode);

        // Compiler Verification: Differential verification between AST and Bytecode VM
        if (std::abs(treeResult - vmResult) > 1e-9) {
            throw RuntimeError("Verification failure: AST tree-walk (" + std::to_string(treeResult) +
                               ") and Stack VM (" + std::to_string(vmResult) + ") diverged!");
        }

        result.value = treeResult;
        result.success = true;
    } catch (const LexerException& e) {
        result.success = false;
        result.errorMessage = std::string("Lexical Error: ") + e.what();
        result.errorPosition = e.getPosition();
    } catch (const ParseException& e) {
        result.success = false;
        result.errorMessage = std::string("Syntax Error: ") + e.what();
        result.errorPosition = e.getPosition();
    } catch (const RuntimeError& e) {
        result.success = false;
        result.errorMessage = e.what();
        result.errorPosition = 0;
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = std::string("Error: ") + e.what();
        result.errorPosition = 0;
    }
    return result;
}

double Compiler::evaluate(const std::string& expression) {
    Tokenizer tokenizer(expression);
    auto tokens = tokenizer.tokenize();
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    return ast->evaluate();
}

std::vector<Instruction> Compiler::compileToBytecode(const std::string& expression) {
    Tokenizer tokenizer(expression);
    auto tokens = tokenizer.tokenize();
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    std::vector<Instruction> bytecode;
    ast->emitBytecode(bytecode);
    return bytecode;
}

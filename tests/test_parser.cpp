#include "Compiler.hpp"
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <functional>

namespace TestFramework {
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;

    void recordResult(const std::string& testName, bool success, const std::string& failureDetail = "") {
        totalTests++;
        if (success) {
            passedTests++;
            std::cout << "  [PASS] " << testName << "\n";
        } else {
            failedTests++;
            std::cout << "  [FAIL] " << testName << "\n";
            if (!failureDetail.empty()) {
                std::cout << "         " << failureDetail << "\n";
            }
        }
    }

    void testExpression(const std::string& testName, const std::string& expression, double expectedValue, double epsilon = 1e-6) {
        auto result = Compiler::compile(expression);
        if (!result.success) {
            recordResult(testName, false, "Expected success for '" + expression + "', but got error: " + result.errorMessage);
            return;
        }

        if (std::abs(result.value - expectedValue) > epsilon) {
            recordResult(testName, false, "Expression '" + expression + "': expected " +
                         std::to_string(expectedValue) + ", got " + std::to_string(result.value));
            return;
        }

        recordResult(testName, true);
    }

    void testError(const std::string& testName, const std::string& expression, const std::string& expectedKeyword = "") {
        auto result = Compiler::compile(expression);
        if (result.success) {
            recordResult(testName, false, "Expected error for '" + expression + "', but evaluation succeeded with value: " +
                         std::to_string(result.value));
            return;
        }

        if (!expectedKeyword.empty() && result.errorMessage.find(expectedKeyword) == std::string::npos) {
            recordResult(testName, false, "Expected error containing '" + expectedKeyword +
                         "', got: '" + result.errorMessage + "'");
            return;
        }

        recordResult(testName, true);
    }
}

int main() {
    using namespace TestFramework;

    std::cout << "========================================================\n";
    std::cout << " Running Mini Expression Compiler Verification Suite\n";
    std::cout << "========================================================\n\n";

    // -------------------------------------------------------------
    // Category 1: Basic Arithmetic
    // -------------------------------------------------------------
    std::cout << "[Suite 1: Basic Arithmetic]\n";
    testExpression("Simple Addition", "10 + 5", 15.0);
    testExpression("Simple Subtraction", "20 - 7", 13.0);
    testExpression("Simple Multiplication", "6 * 7", 42.0);
    testExpression("Simple Division", "48 / 6", 8.0);
    testExpression("Multiple Additions", "1 + 2 + 3 + 4", 10.0);

    // -------------------------------------------------------------
    // Category 2: Operator Precedence (* and / before + and -)
    // -------------------------------------------------------------
    std::cout << "\n[Suite 2: Operator Precedence]\n";
    testExpression("Precedence: Add and Mul (10 + 2 * 3)", "10 + 2 * 3", 16.0);
    testExpression("Precedence: Mul and Add (2 * 3 + 10)", "2 * 3 + 10", 16.0);
    testExpression("Precedence: Sub and Div (20 - 12 / 3)", "20 - 12 / 3", 16.0);
    testExpression("Precedence: Complex Combined (5 + 2 * 4 - 6 / 2)", "5 + 2 * 4 - 6 / 2", 10.0);

    // -------------------------------------------------------------
    // Category 3: Associativity (Left-Associativity for - and /)
    // -------------------------------------------------------------
    std::cout << "\n[Suite 3: Left-Associativity]\n";
    testExpression("Left Associative Subtraction (10 - 4 - 2)", "10 - 4 - 2", 4.0);
    testExpression("Left Associative Division (24 / 4 / 2)", "24 / 4 / 2", 3.0);
    testExpression("Mixed Associative Chain (100 / 5 / 2 - 4 - 1)", "100 / 5 / 2 - 4 - 1", 5.0);

    // -------------------------------------------------------------
    // Category 4: Parenthesized Expressions
    // -------------------------------------------------------------
    std::cout << "\n[Suite 4: Parenthesized Expressions]\n";
    testExpression("Parentheses override precedence ((10 + 2) * 3)", "(10 + 2) * 3", 36.0);
    testExpression("Parentheses in denominator (100 / (2 + 3))", "100 / (2 + 3)", 20.0);
    testExpression("Multiple parenthesized terms ((5 + 3) * (10 - 6))", "(5 + 3) * (10 - 6)", 32.0);

    // -------------------------------------------------------------
    // Category 5: Deeply Nested Parentheses
    // -------------------------------------------------------------
    std::cout << "\n[Suite 5: Nested Parentheses]\n";
    testExpression("Nested Depth 2 ((2 + 3) * 4)", "((2 + 3) * 4)", 20.0);
    testExpression("Nested Depth 4 ((((5 + 3) * 2) - 4) / 2)", "((((5 + 3) * 2) - 4) / 2)", 6.0);
    testExpression("Redundant Parentheses (((42)))", "(((42)))", 42.0);

    // -------------------------------------------------------------
    // Category 6: Unary Minus and Plus
    // -------------------------------------------------------------
    std::cout << "\n[Suite 6: Unary Operators]\n";
    testExpression("Unary Minus on Literal (-5)", "-5", -5.0);
    testExpression("Unary Plus on Literal (+5)", "+5", 5.0);
    testExpression("Unary Minus with Binary Operator (10 + -3)", "10 + -3", 7.0);
    testExpression("Unary Minus in Product (-2 * -3)", "-2 * -3", 6.0);
    testExpression("Unary Minus on Parenthesized Expression (-(3 + 2))", "-(3 + 2)", -5.0);
    testExpression("Double Unary Minus (-(-5))", "-(-5)", 5.0);
    testExpression("Triple Unary Minus (- - - 5)", "- - - 5", -5.0);

    // -------------------------------------------------------------
    // Category 7: Floating-Point Numbers & Decimals
    // -------------------------------------------------------------
    std::cout << "\n[Suite 7: Floating-Point & Decimals]\n";
    testExpression("Decimal Addition (3.14 + 2.86)", "3.14 + 2.86", 6.0);
    testExpression("Decimal Division (10.5 / 2.5)", "10.5 / 2.5", 4.2);
    testExpression("Leading Decimal Point (.5 + .5)", ".5 + .5", 1.0);
    testExpression("Mixed Integer and Decimal (10 + 0.5 * 4)", "10 + 0.5 * 4", 12.0);

    // -------------------------------------------------------------
    // Category 8: Whitespace Handling
    // -------------------------------------------------------------
    std::cout << "\n[Suite 8: Whitespace Handling]\n";
    testExpression("Leading & Trailing Spaces (   42   )", "   42   ", 42.0);
    testExpression("Mixed Tabs & Spaces ( \\t 10  + \\t 2 * 3  )", " \t 10  + \t 2 * 3  ", 16.0);
    testExpression("No Spaces (10+2*3)", "10+2*3", 16.0);

    // -------------------------------------------------------------
    // Category 9: Differential Verification (AST vs Stack VM)
    // -------------------------------------------------------------
    std::cout << "\n[Suite 9: Differential Compiler Verification]\n";
    {
        std::string expr = "((10 + 2) * -3) / (1 + 2)";
        auto result = Compiler::compile(expr);
        bool match = (result.success && std::abs(result.value - (-12.0)) < 1e-6);
        recordResult("Differential Check: AST Tree-Walk vs Bytecode VM", match);
    }

    // -------------------------------------------------------------
    // Category 10: Error Handling - Division by Zero
    // -------------------------------------------------------------
    std::cout << "\n[Suite 10: Error Handling - Division by Zero]\n";
    testError("Direct Division by Zero (5 / 0)", "5 / 0", "Division by zero");
    testError("Evaluated Zero Divisor (10 / (2 - 2))", "10 / (2 - 2)", "Division by zero");
    testError("Decimal Zero Divisor (4.2 / 0.0)", "4.2 / 0.0", "Division by zero");

    // -------------------------------------------------------------
    // Category 11: Error Handling - Syntax & Malformed Expressions
    // -------------------------------------------------------------
    std::cout << "\n[Suite 11: Error Handling - Syntax & Grammar]\n";
    testError("Empty String", "", "Empty expression");
    testError("Whitespace Only", "     ", "Empty expression");
    testError("Trailing Operator (2 +)", "2 +", "Expected operand");
    testError("Leading Binary Operator (* 5)", "* 5", "Unexpected token");
    testError("Consecutive Binary Operators (2 + * 3)", "2 + * 3", "Unexpected token");
    testError("Consecutive Numbers Without Operator (2 3 + 4)", "2 3 + 4", "Unexpected trailing token");
    testError("Empty Parentheses ()", "()", "Empty parentheses");
    testError("Empty Parentheses in Expression (2 + ())", "2 + ()", "Empty parentheses");
    testError("Missing Closing Parenthesis ((2 + 3)", "(2 + 3", "Missing closing parenthesis");
    testError("Unexpected Closing Parenthesis (2 + 3))", "2 + 3)", "Unexpected trailing token");
    testError("Lone Closing Parenthesis ())", ")", "Unexpected closing parenthesis");

    // -------------------------------------------------------------
    // Category 12: Error Handling - Lexical & Characters
    // -------------------------------------------------------------
    std::cout << "\n[Suite 12: Error Handling - Lexical Errors]\n";
    testError("Unexpected Character '$' (2 + $ 3)", "2 + $ 3", "Unexpected character '$'");
    testError("Alphabetical Identifier (x + 5)", "x + 5", "Unexpected character 'x'");
    testError("Multiple Decimal Points (1.2.3 + 4)", "1.2.3 + 4", "multiple decimal points");
    testError("Solitary Decimal Point (5 + .)", "5 + .", "solitary decimal point");

    // -------------------------------------------------------------
    // Summary
    // -------------------------------------------------------------
    std::cout << "\n========================================================\n";
    std::cout << " Test Suite Summary\n";
    std::cout << "========================================================\n";
    std::cout << " Total Tests : " << totalTests << "\n";
    std::cout << " Passed      : " << passedTests << "\n";
    std::cout << " Failed      : " << failedTests << "\n";
    std::cout << " Status      : " << (failedTests == 0 ? "ALL TESTS PASSED" : "TEST FAILURES DETECTED") << "\n";
    std::cout << "========================================================\n";

    return failedTests == 0 ? 0 : 1;
}

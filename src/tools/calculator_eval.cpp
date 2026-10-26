// src/tools/calculator_eval.cpp — YOU IMPLEMENT (spec §4.6).
//
// calc_evaluate: evaluate a well-formed token list with two stacks
// (one for numbers, one for operators and '('). See spec §4.6 and the
// worked trace in Appendix C. std::stack or std::vector are both fine.

#include "tools/calculator.h"
#include <stack>

static int precedence(CalcToken::Kind k){ // This is a helper function that will return the precendence of an operator to use in comparisons for the calc_evaluate function.
    if(k == CalcToken::Kind::Plus) return 1;
    if(k == CalcToken::Kind::Minus) return 1;
    if(k == CalcToken::Kind::Times) return 2;
    if(k == CalcToken::Kind::Divide) return 2;

    return 0;
}

static bool apply(std::stack<double>& values, std::stack<CalcToken::Kind>& ops, std::string* error) {
    
    if(ops.empty() || values.size() < 2){ // If there are no operators or if the amount of values is less than 2, an operation is not possible.
        *error = "malformed expression";
        return false; 
    } 

    CalcToken::Kind op = ops.top();
    ops.pop();
    double right_operand = values.top();
    values.pop();
    double left_operand = values.top();
    values.pop();

    double applied_val = 0;

    switch(op) {
        case CalcToken::Kind::Plus: applied_val = left_operand + right_operand; break;
        case CalcToken::Kind::Minus: applied_val = left_operand - right_operand; break;
        case CalcToken::Kind::Times: applied_val = left_operand * right_operand; break;
        case CalcToken::Kind::Divide: 
            if(right_operand == 0) {
                *error = "division by zero";
                return false; 
            }
            applied_val = left_operand / right_operand; 
        break;
        default: 
            *error = "unexpected token on operator stack";
            return false;
    }

    values.push(applied_val);
    return true;
}

bool calc_evaluate(const std::vector<CalcToken>& tokens, double* out,
                   std::string* error) {
    std::stack<double> values;
    std::stack<CalcToken::Kind> ops;
    
}

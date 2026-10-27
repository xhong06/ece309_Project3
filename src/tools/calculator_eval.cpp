// src/tools/calculator_eval.cpp — YOU IMPLEMENT (spec §4.6).
//
// calc_evaluate: evaluate a well-formed token list with two stacks
// (one for numbers, one for operators and '('). See spec §4.6 and the
// worked trace in Appendix C. std::stack or std::vector are both fine.

#include "tools/calculator.h"
#include <stack>
#include <string>
#include <vector>

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
    
    // Iterate through all tokens
    for(std::size_t i = 0; i < tokens.size(); i++) {
        // If the token is a Number,
        if(tokens[i].kind == CalcToken::Kind::Number) {
            values.push(tokens[i].value); // Add the number's value to the value's stack.
        }
        // If the token is a Left Parentheses,
        else if(tokens[i].kind == CalcToken::Kind::LParen) {
            ops.push(tokens[i].kind); // Then push the Left Parentheses to the tokens list.
        }
        // If the token is a Right Parentheses,
        else if(tokens[i].kind == CalcToken::Kind::RParen) {
            while(!ops.empty() && (ops.top() != CalcToken::Kind::LParen)) {
                // Then apply the operators in the ops stack until you reach the Left Parentheses on the ops stack.
                if(!(apply(values, ops, error))){ 
                    return false;
                }
            }
            // If the ops stack is empty, then return before trying to pop.
            if(ops.empty()) { 
                *error = "malformed expression";
                return false;
            } 
            // Pop the Left Parentheses.
            ops.pop();
        }
        // If the token is a +,-,*,/
        else{
            while(!ops.empty() && ops.top() != CalcToken::Kind::LParen && (precedence(ops.top()) >= precedence(tokens[i].kind))) {
                // Then apply the operator on the top of the ops stack if that operator is of greater precedence than the operator about to be added.
                if(!(apply(values, ops, error))){
                    return false;
                }
            }
            // Push the token operator that you compared onto the stack to be evaluated in a future cycle.
            ops.push(tokens[i].kind);
        }
    }

    // After all tokens have been parsed,
    // While there are still operators in the ops stack,
    while(!ops.empty()) {
        // apply them until there are no more operators left.
        if(!(apply(values, ops, error))){
            return false;
        }
    }

    // Ensure that all the operators shrank the number of values down to 1.
    if(values.size() != 1) {
        *error = "malformed expression";
        return false;
    } else {
        // Return the final value through *out.
        *out = values.top();
        return true;
    }
}

#include "ExprToken.h"

#include <cctype>
#include <cmath>
#include <string>

ExprTokenType token_type(const char* val)
{
    if (std::isdigit(*val)) return ExprTokenType::ET_VAL;
    switch (*val)
    {
    case '+': return ExprTokenType::ET_PLUS;
    case '-': return ExprTokenType::ET_U_MINUS;
    case '*': return ExprTokenType::ET_MULT;
    case '/': return ExprTokenType::ET_DIV;
    case '(': return ExprTokenType::ET_OPEN_BRACKET;
    case ')': return ExprTokenType::ET_CLOSE_BRACKET;
    case '^': return ExprTokenType::ET_EXP;
    }
    std::string str = val;
    if (str.find("sin") == 0) return ExprTokenType::ET_SIN;
    if (str.find("cos") == 0) return ExprTokenType::ET_COS;
    if (str.find("log") == 0) return ExprTokenType::ET_LOG;
    return ExprTokenType::ET_EMPTY;
}

int ExprToken::defPriority()
{
    switch (type)
    {
    case ExprTokenType::ET_PLUS: return 1;
    case ExprTokenType::ET_B_MINUS: return 1;
    case ExprTokenType::ET_MULT: return 2;
    case ExprTokenType::ET_DIV: return 2;
    case ExprTokenType::ET_EXP: return 3;
    case ExprTokenType::ET_SIN: return 4;
    case ExprTokenType::ET_COS: return 4;
    case ExprTokenType::ET_LOG: return 4;
    case ExprTokenType::ET_U_MINUS: return 4;
    }
    return 0;
}

ExprTokenType ExprToken::defOpType()
{
    switch (type)
    {
    case ExprTokenType::ET_PLUS:
    case ExprTokenType::ET_B_MINUS:
    case ExprTokenType::ET_MULT:
    case ExprTokenType::ET_DIV:
    case ExprTokenType::ET_EXP:
        return ExprTokenType::ET_BINARY;
    case ExprTokenType::ET_SIN:
    case ExprTokenType::ET_COS:
    case ExprTokenType::ET_LOG:
    case ExprTokenType::ET_U_MINUS:
        return ExprTokenType::ET_UNARY;
    case ExprTokenType::ET_VAL:
    case ExprTokenType::ET_VARIABLE:
        return ExprTokenType::ET_NON_OP;
    }
    return ExprTokenType::ET_EMPTY;
}

float ExprToken::execute(float num1, float num2)
{
    switch (type)
    {
    case ExprTokenType::ET_PLUS: return num1 + num2;
    case ExprTokenType::ET_B_MINUS: return num1 - num2;
    case ExprTokenType::ET_MULT: return num1 * num2;
    case ExprTokenType::ET_DIV: return num1 / num2;
    case ExprTokenType::ET_EXP: return std::pow(num1, num2);
    case ExprTokenType::ET_SIN: return std::sin(num1);
    case ExprTokenType::ET_COS: return std::cos(num1);
    case ExprTokenType::ET_LOG: return std::log(num1);
    }
    return 0.0f;
}
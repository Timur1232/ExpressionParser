#pragma once

#include <string>

enum class ExprTokenType
{
    ET_VAL = 0,
    ET_VARIABLE,

    ET_PLUS,
    ET_B_MINUS,
    ET_MULT,
    ET_DIV,
    ET_OPEN_BRACKET,
    ET_CLOSE_BRACKET,
    ET_U_MINUS,
    ET_EXP,

    ET_SIN,
    ET_COS,
    ET_LOG,

    ET_BINARY,
    ET_UNARY,
    ET_NON_OP,

    ET_EMPTY = -1
};

struct ExprToken
{
    float val = 0.0f;
    ExprTokenType type;
    std::string name;

    ExprToken(float val)
        : val(val), type(ExprTokenType::ET_VAL), name("")
    {
    }
    ExprToken(char type)
        : val(0.0f), type((ExprTokenType)type), name("")
    {
    }
    ExprToken(ExprTokenType type)
        : val(0.0f), type(type), name("")
    {
    }
    ExprToken(const std::string& name)
        : val(0.0f), type(ExprTokenType::ET_VARIABLE), name(name)
    {
    }

    int defPriority();
    ExprTokenType defOpType();
    float execute(float num1, float num2);
};

ExprTokenType token_type(const char* val);
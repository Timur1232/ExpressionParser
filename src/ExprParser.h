#pragma once

#include "ExprToken.h"

#include <vector>
#include <string>
#include <unordered_map>

class ExprParser
{
public:
    ExprParser() = default;
    ExprParser(const ExprParser&) = default;
    ExprParser(ExprParser&&) noexcept;
    ExprParser(const char* expression, int& err);
    ExprParser(const std::string& expression, int& err);

    int parseExpr(const char* expression);
    int parseExpr(const std::string& expression);
    const std::vector<ExprToken>& getInfix() const;
    const std::vector<ExprToken>& getPostfix() const;

    bool hasVars() const;
    const std::unordered_map<std::string, float>& getVars() const;
    std::unordered_map<std::string, float>::iterator getVarIter();
    std::unordered_map<std::string, float>::const_iterator getVarIter() const;

    void setVar(const std::string& name, float val);
    float& var(const std::string& name);
    float var(const std::string& name) const;

    float calcExpr();
    float getAnswer() const;

private:
    void scanInfix(const char* expr);
    int checkExpr(const char* expr);
    int checkExpr();
    void convertToPostfix();

private:
    std::vector<ExprToken> m_InfixNotation;
    std::vector<ExprToken> m_PostfixNotation;
    std::unordered_map<std::string, float> m_Variables;
    float m_Answer = 0.0f;
};

//ExprParser parse_expr(const char* expr, float& result);
//float parse_expr_fast(const char* expr, ...);

void print_expr(const std::vector<ExprToken>& expt);
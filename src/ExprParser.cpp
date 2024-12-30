#include "ExprParser.h"

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cmath>

static int parse_float(std::string::const_iterator iter, const std::string::const_iterator& iterEnd, float& dest);
static bool is_white_space(char ch);

ExprParser::ExprParser(ExprParser&& other) noexcept
    : m_InfixNotation(std::move(other.m_InfixNotation)), m_PostfixNotation(std::move(other.m_PostfixNotation)),
      m_Variables(std::move(other.m_Variables)), m_Answer(other.m_Answer)
{
}

ExprParser::ExprParser(const char* expression, int& err)
{
    err = parseExpr(expression);
}

ExprParser::ExprParser(const std::string& expression, int& err)
    : ExprParser(expression.c_str(), err)
{
}

int ExprParser::parseExpr(const char* expression)
{
    if (checkExpr(expression)) return -1;
    scanInfix(expression);
    if (checkExpr()) return -1;
    convertToPostfix();
    return 0;
}

int ExprParser::parseExpr(const std::string& expression)
{
    return parseExpr(expression.c_str());
}

const std::vector<ExprToken>& ExprParser::getInfix() const
{
    return m_InfixNotation;
}

const std::vector<ExprToken>& ExprParser::getPostfix() const
{
    return m_PostfixNotation;
}

bool ExprParser::hasVars() const
{
    return !m_Variables.empty();
}

const std::unordered_map<std::string, float>& ExprParser::getVars() const
{
    return m_Variables;
}

std::unordered_map<std::string, float>::iterator ExprParser::getVarIter()
{
    return m_Variables.begin();
}

std::unordered_map<std::string, float>::const_iterator ExprParser::getVarIter() const
{
    return m_Variables.begin();
}

void ExprParser::setVar(const std::string& name, float val)
{
    m_Variables[name] = val;
}

float& ExprParser::var(const std::string& name)
{
    return m_Variables[name];
}

float ExprParser::var(const std::string& name) const
{
    return m_Variables.at(name);
}

float ExprParser::calcExpr()
{
    std::vector<ExprToken> stack;
    for (auto& token : m_PostfixNotation)
    {
        if (token.type == ExprTokenType::ET_VAL || token.type == ExprTokenType::ET_VARIABLE)
        {
            stack.emplace_back(token);
        }
        else if (token.type == ExprTokenType::ET_U_MINUS)
        {
            stack.back().val *= -1;
        }
        else if (token.type == ExprTokenType::ET_SIN || token.type == ExprTokenType::ET_COS || token.type == ExprTokenType::ET_LOG)
        {
            float num1;
            if (stack.back().type == ExprTokenType::ET_VARIABLE)
            {
                num1 = m_Variables[stack.back().name];
            }
            else
            {
                num1 = stack.back().val;
            }
            stack.pop_back();
            stack.emplace_back(token.execute(num1, 0.0f));
        }
        else
        {
            float num2, num1;
            if (stack.back().type == ExprTokenType::ET_VARIABLE)
            {
                num2 = m_Variables[stack.back().name];
            }
            else
            {
                num2 = stack.back().val;
            }
            stack.pop_back();
            if (stack.back().type == ExprTokenType::ET_VARIABLE)
            {
                num1 = m_Variables[stack.back().name];
            }
            else
            {
                num1 = stack.back().val;
            }
            stack.pop_back();
            stack.emplace_back(token.execute(num1, num2));
        }
    }
    m_Answer = stack.back().val;
    return m_Answer;
}

float ExprParser::getAnswer() const
{
    return m_Answer;
}

void ExprParser::scanInfix(const char* expr)
{
    m_InfixNotation.clear();
    float value = 0.0f;
    int shift = 0;
    std::string strExpr = expr;
    auto iter = strExpr.begin();
    while (iter != strExpr.end())
    {
        if (is_white_space(*iter))
        {
            ++iter;
            continue;
        }
        if (std::isdigit(*iter))
        {
            shift = parse_float(iter, strExpr.end(), value);
            if (shift < 0) break;
            iter += shift;
            m_InfixNotation.emplace_back(value);
            continue;
        }
        else if (std::isalpha(*iter))
        {
            ExprTokenType type = token_type(&(*iter));
            if (type == ExprTokenType::ET_EMPTY)
            {
                std::string::iterator varIter = iter;
                while (varIter != strExpr.end() && !is_white_space(*varIter) && token_type(&(*varIter)) == ExprTokenType::ET_EMPTY)
                {
                    ++varIter;
                }
                std::string varName(iter, varIter);
                m_Variables.insert({ varName, 0.0f });
                m_InfixNotation.emplace_back(varName);
                iter = varIter;
                continue;
            }
            else
            {
                iter += 2;
                m_InfixNotation.emplace_back(type);
            }
        }
        else if (token_type(&(*iter)) != ExprTokenType::ET_VAL)
        {
            if ((*iter == '-') && ((iter != strExpr.begin())) && ((m_InfixNotation.back().type == ExprTokenType::ET_VAL) || (m_InfixNotation.back().type == ExprTokenType::ET_VARIABLE) || (m_InfixNotation.back().type == ExprTokenType::ET_CLOSE_BRACKET)))
            {
                m_InfixNotation.emplace_back(ExprTokenType::ET_B_MINUS);
            }
            else
            {
                m_InfixNotation.emplace_back(token_type(&(*iter)));
            }
        }
        ++iter;
    }
}

int ExprParser::checkExpr(const char* expr)
{
    int open = 0, close = 0;
    for (int i = 0; expr[i] != '\0'; i++)
    {
        if (expr[i] == '(') open++;
        if (expr[i] == ')') close++;
    }
    return -(open != close);
}

int ExprParser::checkExpr()
{
    auto iter = m_InfixNotation.begin();
    if (iter->defOpType() == ExprTokenType::ET_BINARY)
        return -1;
    ++iter;
    while (iter != (m_InfixNotation.end() - 1))
    {
        if ((iter->defOpType() == ExprTokenType::ET_BINARY) && (((iter - 1)->defOpType() == ExprTokenType::ET_BINARY) ||
            (iter + 1)->defOpType() == ExprTokenType::ET_BINARY))
            return -1;
        if ((iter->defOpType() == ExprTokenType::ET_UNARY) && ((iter + 1)->defOpType() == ExprTokenType::ET_BINARY))
            return -1;
        if ((iter->defOpType() == ExprTokenType::ET_NON_OP) && (((iter - 1)->defOpType() == ExprTokenType::ET_NON_OP) || ((iter + 1)->defOpType() == ExprTokenType::ET_NON_OP)))
            return -1;
        ++iter;
    }
    if (iter->defOpType() == ExprTokenType::ET_BINARY)
        return -1;
    return 0;
}

void ExprParser::convertToPostfix()
{
    m_PostfixNotation.clear();
    std::vector<ExprToken> stack;
    for (auto& token : m_InfixNotation)
    {
        if (token.type != ExprTokenType::ET_VAL && token.type != ExprTokenType::ET_VARIABLE)
        {
            if (token.type == ExprTokenType::ET_CLOSE_BRACKET)
            {
                while (!stack.empty() && (stack.back().type != ExprTokenType::ET_OPEN_BRACKET))
                {
                    m_PostfixNotation.emplace_back(stack.back());
                    stack.pop_back();
                }
                stack.pop_back();
                continue;
            }
            else if (!stack.empty() && (stack.back().type != ExprTokenType::ET_OPEN_BRACKET) && (token.type != ExprTokenType::ET_OPEN_BRACKET) && (token.defPriority() < stack.back().defPriority()))
            {
                m_PostfixNotation.emplace_back(stack.back());
                stack.pop_back();
            }
            stack.emplace_back(token.type);
        }
        else
        {
            m_PostfixNotation.emplace_back(token);
        }
    }
    while (!stack.empty())
    {
        m_PostfixNotation.emplace_back(stack.back());
        stack.pop_back();
    }
}

//ExprParser parse_expr(const char* expr, float& result)
//{
//    return ExprParser();
//}
//
//float parse_expr_fast(const char* expr, ...)
//{
//    return 0.0f;
//}

void print_expr(const std::vector<ExprToken>& expr)
{
    for (auto i : expr)
    {
        if (i.type == ExprTokenType::ET_VAL)
            std::cout << i.val << ' ';
        else
        {
            const char* ch = "";
            switch (i.type)
            {
            case ExprTokenType::ET_PLUS: ch = "+"; break;
            case ExprTokenType::ET_U_MINUS: ch = "-u"; break;
            case ExprTokenType::ET_B_MINUS: ch = "-b"; break;
            case ExprTokenType::ET_MULT: ch = "*"; break;
            case ExprTokenType::ET_DIV: ch = "/"; break;
            case ExprTokenType::ET_OPEN_BRACKET: ch = "("; break;
            case ExprTokenType::ET_CLOSE_BRACKET: ch = ")"; break;
            case ExprTokenType::ET_EXP: ch = "^"; break;
            case ExprTokenType::ET_SIN: ch = "sin"; break;
            case ExprTokenType::ET_COS: ch = "cos"; break;
            case ExprTokenType::ET_LOG: ch = "log"; break;
            case ExprTokenType::ET_VARIABLE: ch = i.name.c_str(); break;
            }
            std::cout << ch << ' ';
        }
    }
    std::cout << std::endl;
}

int parse_float(std::string::const_iterator iter, const std::string::const_iterator& iterEnd, float& dest)
{
    dest = 0.0f;

    float decimalMult = 0.1f;
    bool period = false;
    int shift = -1;
    // Основной цикл ввода
    while (true)
    {
        shift++;
        if (*iter == '\n' || *iter == ' ')
        {
            break;
        }
        if (*iter == '.' || *iter == ',')
        {
            if (period) {
                return -1;
            }
            period = true;
            ++iter;
            continue;
        }

        if (*iter >= '0' && *iter <= '9')
        {
            // Дробная часть
            if (period)	
            {
                dest += (float)(*iter - '0') * decimalMult;
                decimalMult *= 0.1f;
            }
            // Целая часть
            else
            {
                dest *= 10.0f;
                dest += *iter - '0';
            }
        }
        else
        {
            break;
        }
        ++iter;
        if (iter == iterEnd)
        {
            shift++;
            break;
        }
    }

    return shift;
}

static bool is_white_space(char ch)
{
    return ch == ' ' || ch == '\n' || ch == '\t';
}





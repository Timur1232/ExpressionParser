#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <stack>
#include <cmath>

enum class ExprTokenType
{
    ET_VAL = 0,
    ET_PLUS = 1,
    ET_B_MINUS = 2,
    ET_MULT = 3,
    ET_DIV = 4,
    ET_OPEN_BRACKET = 5,
    ET_CLOSE_BRACKET = 6,
    ET_U_MINUS = 7,
    ET_EXP = 8,

    ET_EMPTY = -1
};

struct ExprToken
{
    float val = 0.0f;
    ExprTokenType type;

    ExprToken(float val)
        : val(val), type(ExprTokenType::ET_VAL)
    {
    }
    ExprToken(char type)
        : val(0.0f), type((ExprTokenType)type)
    {
    }
    ExprToken(ExprTokenType type)
        : val(0.0f), type(type)
    {
    }
};

static float parse_expr(const char* expr);
static int parse_float(std::string::const_iterator iter, const std::string::const_iterator& iterEnd, float& dest);
static void print_expr(const std::vector<ExprToken>& expt);
static ExprTokenType token_type(char val);
static int token_priority(ExprTokenType token);
static float execute(float num1, float num2, ExprTokenType operation);
static bool is_white_space(char ch);

int main()
{
	std::string input;
	std::cout << "Enter expression -> ";
	std::getline(std::cin, input);
	std::cout << "\nAnswer: " << parse_expr(input.c_str()) << '\n';
	std::cin.get();
}

float parse_expr(const char* expr)
{
	std::vector<ExprToken> infixExpr;

	// Преобразование строки в токены
	float value = 0.0f;
	int shift = 0;
	std::string strExpr = expr;
	auto iter = strExpr.begin();
	while (iter != strExpr.end())
	{
		if (std::isdigit(*iter))
		{
			shift = parse_float(iter, strExpr.end(), value);
			if (shift < 0) break;
			iter += shift;
			infixExpr.emplace_back(value);
			continue;
		}
		else if (!is_white_space(*iter) && token_type(*iter) != ExprTokenType::ET_VAL)
		{
			if ((*iter == '-') && ((iter == strExpr.begin()) || (infixExpr.back().type == ExprTokenType::ET_VAL) || (infixExpr.back().type == ExprTokenType::ET_CLOSE_BRACKET)))
			{
				infixExpr.emplace_back(ExprTokenType::ET_B_MINUS);
			}
			else
			{
				infixExpr.emplace_back(token_type(*iter));
			}
		}
		else if (std::isalpha(*iter))
		{
			std::cerr << "Error" << std::endl;
		}
		++iter;
	}
	strExpr.clear();
	std::cout << "\nInfix notation:\n";
	print_expr(infixExpr);

	std::vector<ExprToken> stack;
	std::vector<ExprToken> postfixExpr;
	for (auto& token : infixExpr)
	{
		if (token.type != ExprTokenType::ET_VAL)
		{
			if (token.type == ExprTokenType::ET_CLOSE_BRACKET)
			{
				while (!stack.empty() && (stack.back().type != ExprTokenType::ET_OPEN_BRACKET))
				{
					postfixExpr.emplace_back(stack.back());
					stack.pop_back();
				}
				if (!stack.empty() && (stack.back().type == ExprTokenType::ET_OPEN_BRACKET)) stack.pop_back();
				continue;
			}
			else if (!stack.empty() && (stack.back().type != ExprTokenType::ET_OPEN_BRACKET) && (token.type != ExprTokenType::ET_OPEN_BRACKET) && (token_priority(token.type) > token_priority(stack.back().type)))
			{
				postfixExpr.emplace_back(stack.back());
				stack.pop_back();
			}
			stack.emplace_back(token.type);
			//print_expr(stack);
		}
		else
		{
			postfixExpr.emplace_back(token);
		}
	}
	while (!stack.empty())
	{
		postfixExpr.emplace_back(stack.back());
		stack.pop_back();
	}
	infixExpr.clear();
	std::cout << "\nPostfix notation:\n";
	print_expr(postfixExpr);

	for (auto& token : postfixExpr)
	{
		if (token.type == ExprTokenType::ET_VAL)
		{
			stack.emplace_back(token);
		}
		else if (token.type == ExprTokenType::ET_U_MINUS)
		{
			stack.back().val *= -1;
		}
		else
		{
			float num2 = stack.back().val;
			stack.pop_back();
			float num1 = stack.back().val;
			stack.pop_back();
			stack.emplace_back(execute(num1, num2, token.type));
		}
	}

	return stack.back().val;
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

static ExprTokenType token_type(char val)
{
	if (std::isdigit(val)) return ExprTokenType::ET_VAL;
	switch (val)
	{
	case '+': return ExprTokenType::ET_PLUS;
	case '-': return ExprTokenType::ET_U_MINUS;
	case '*': return ExprTokenType::ET_MULT;
	case '/': return ExprTokenType::ET_DIV;
	case '(': return ExprTokenType::ET_OPEN_BRACKET;
	case ')': return ExprTokenType::ET_CLOSE_BRACKET;
	case '^': return ExprTokenType::ET_EXP;
	}
	return ExprTokenType::ET_EMPTY;
}

static int token_priority(ExprTokenType token)
{
	switch (token)
	{
	case ExprTokenType::ET_PLUS: return 4;
	case ExprTokenType::ET_B_MINUS: return 4;
	case ExprTokenType::ET_MULT: return 3;
	case ExprTokenType::ET_DIV: return 3;
	case ExprTokenType::ET_EXP: return 2;
	case ExprTokenType::ET_U_MINUS: return 1;
	}
}

static float execute(float num1, float num2, ExprTokenType operation)
{
	switch (operation)
	{
	case ExprTokenType::ET_PLUS: return num1 + num2;
	case ExprTokenType::ET_B_MINUS: return num1 - num2;
	case ExprTokenType::ET_MULT: return num1 * num2;
	case ExprTokenType::ET_DIV: return num1 / num2;
	case ExprTokenType::ET_EXP: return std::pow(num1, num2);
	}
}

static bool is_white_space(char ch)
{
	return ch == ' ' || ch == '\n' || ch == '\t';
}

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
			}
			std::cout << ch << ' ';
		}
	}
	std::cout << std::endl;
}

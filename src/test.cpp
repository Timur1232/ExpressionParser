#include "ExprParser.h"
#include <iostream>
#include <string>

int main()
{
	std::string input;
	std::cout << "Enter expression:\n";
	std::getline(std::cin, input);
	ExprParser expr;
	if (expr.parseExpr(input))
	{
		std::cout << "\nError" << std::endl;
	}
	else
	{
		if (expr.hasVars())
		{
			std::cout << '\n';
			auto iter = expr.getVarIter();
			while (iter != expr.getVars().end())
			{
				std::cout << "Enter variable " << iter->first << ": ";
				std::cin >> iter->second;
				++iter;
			}
			std::cout << '\n';
		}
		std::cout << "Infix notation:\n";
		print_expr(expr.getInfix());
		std::cout << "\nPostfix notation:\n";
		print_expr(expr.getPostfix());
		std::cout << "\nAnswer: " << expr.calcExpr() << std::endl;
	}
	std::cin.get();
	std::cin.get();
}
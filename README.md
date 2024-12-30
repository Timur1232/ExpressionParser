# ExpressionParser
Parser for mathematical expressions.

## Features
- Supports PEMDAS priority:
  1. Parentheses
  2. Exponents
  3. Unary minus
  4. Multiplication and division
  5. Addition and subtraction
- Functions: `sin`, `cos`, `log`, and exponents
- Supports creating variables

## Syntax
- `()` : Parentheses
- `*` : Multiplication
- `/` : Division
- `+` : Addition
- `-` : Unary and binary subtraction
- `sin` : Sine
- `cos` : Cosine
- `log` : Logarithm
- Constants can be written with `.` and `,` as period

## Usage
First, create an object of `ExprParser` class and load an expression either by the constructor or the `parseExpr` method. The method will return `-1` if the expression has an error; the constructor sets `err` to `-1` if there's an error.
If the expression has variables in it, they can be set with the `setVar` or `var` methods. Variables can also be accessed with `getVarIter`.
Finally, use `calcExpr` to calculate the expression and get its result. After this, the result can be accessed with the `getAnswer` method.

### Example:
```cpp
int main()
{
    int err = 0;
    ExprParser expr("12 + sin(4) * (24.3 - x^(y - 2) / 8)", err);
    if (err)
    {
        // If error
        std::cout << "\nError" << std::endl;
    }
    else
    {
        if (expr.hasVars())
        {
            // If has variables
            std::cout << '\n';
            auto iter = expr.getVarIter();
            // Entering variables
            while (iter != expr.getVars().end())
            {
                std::cout << "Enter variable " << iter->first << ": ";
                std::cin >> iter->second;
                ++iter;
            }
            std::cout << '\n';
        }
        // Print expression
        std::cout << "Infix notation:\n";
        print_expr(expr.getInfix());
        std::cout << "\nPostfix notation:\n";
        print_expr(expr.getPostfix());
        // Final result
        std::cout << "\nAnswer: " << expr.calcExpr() << std::endl;
    }
}
```

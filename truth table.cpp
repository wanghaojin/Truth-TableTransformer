#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <bitset>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>

std::set<std::string> extractVariables(const std::string &expression)
{
    std::set<std::string> variables;
    for (char c : expression)
    {
        if (isalpha(c))
        {
            variables.insert(std::string(1, c));
        }
    }
    return variables;
}

bool evaluateSubExpression(const std::string &subExpression, const std::map<std::string, bool> &varValues)
{
    std::istringstream iss(subExpression);
    std::string token;
    std::vector<bool> values;
    std::vector<char> operators;

    while (iss >> token)
    {
        if (token == "(")
        {
            operators.push_back('(');
        }
        else if (token == ")")
        {
            while (!operators.empty() && operators.back() != '(')
            {
                char op = operators.back();
                operators.pop_back();

                bool rhs = values.back();
                values.pop_back();
                bool lhs = values.back();
                values.pop_back();

                if (op == '&')
                {
                    values.push_back(lhs && rhs);
                }
                else if (op == '|')
                {
                    values.push_back(lhs || rhs);
                }
                else if (op == '>')
                {
                    values.push_back(!lhs || rhs);
                }
            }
            if (!operators.empty() && operators.back() == '(')
            {
                operators.pop_back();
            }
        }
        else if (token == "&" || token == "|" || token == "->")
        {
            char op = token[0];
            if (op == '-' && token.size() == 2)
            {
                op = token[1];
            }
            while (!operators.empty() && operators.back() != '(' && (operators.back() != '>' || op != '>'))
            {
                op = operators.back();
                operators.pop_back();

                bool rhs = values.back();
                values.pop_back();
                bool lhs = values.back();
                values.pop_back();

                if (op == '&')
                {
                    values.push_back(lhs && rhs);
                }
                else if (op == '|')
                {
                    values.push_back(lhs || rhs);
                }
                else if (op == '>')
                {
                    values.push_back(!lhs || rhs);
                }
            }
            operators.push_back(op);
        }
        else
        {
            values.push_back(varValues.at(token));
        }
    }

    while (!operators.empty())
    {
        char op = operators.back();
        operators.pop_back();

        bool rhs = values.back();
        values.pop_back();
        bool lhs = values.back();
        values.pop_back();

        if (op == '&')
        {
            values.push_back(lhs && rhs);
        }
        else if (op == '|')
        {
            values.push_back(lhs || rhs);
        }
        else if (op == '>')
        {
            values.push_back(!lhs || rhs);
        }
    }

    return values.back();
}
std::vector<std::string> extractSubExpressions(const std::string &expression)
{
    std::vector<std::string> subExpressions;
    std::string token;
    std::istringstream iss(expression);

    int openParenthesis = 0;
    std::string currentExpr;

    while (iss >> token)
    {
        if (token == "(")
        {
            if (openParenthesis > 0)
            {
                currentExpr += token + " ";
            }
            openParenthesis++;
        }
        else if (token == ")")
        {
            openParenthesis--;
            if (openParenthesis > 0)
            {
                currentExpr += token + " ";
            }
            else
            {
                subExpressions.push_back(currentExpr);
                currentExpr = "";
            }
        }
        else if (openParenthesis > 0)
        {
            currentExpr += token + " ";
        }
    }

    return subExpressions;
}

bool isLogicOperator(const std::string &str)
{
    return str == "&" || str == "|" || str == "->";
}

std::string replaceLogicOperatorsWithLatex(const std::string &str)
{
    std::string result = str;
    size_t pos;

    while ((pos = result.find("&")) != std::string::npos)
    {
        result.replace(pos, 1, "\\land");
    }

    while ((pos = result.find("|")) != std::string::npos)
    {
        result.replace(pos, 1, "\\lor");
    }

    while ((pos = result.find("->")) != std::string::npos)
    {
        result.replace(pos, 2, "\\to");
    }

    return result;
}

std::string generateLatex(const std::set<std::string> &variables, const std::string &expression)
{
    std::vector<std::string> subExpressions = extractSubExpressions(expression);
    subExpressions.push_back(expression); // 添加原始表达式

    std::string latex = "\\begin{tabular}{";
    for (size_t i = 0; i < variables.size() + subExpressions.size() - 1; ++i)
    {
        latex += "|c";
    }
    latex += "|}\n\\hline\n";

    // 添加变量和子表达式标题
    for (const auto &var : variables)
    {
        latex += var + " & ";
    }
    for (size_t i = 0; i < subExpressions.size(); ++i)
    {
        latex += replaceLogicOperatorsWithLatex(subExpressions[i]);
        if (i < subExpressions.size() - 1)
        {
            latex += " & ";
        }
    }
    latex += "\\\\\n\\hline\n";

    size_t numRows = std::pow(2, variables.size());
    for (size_t row = 0; row < numRows; ++row)
    {
        std::bitset<32> binaryRow(row);
        std::map<std::string, bool> varValues;
        size_t col = 0;
        for (const auto &var : variables)
        {
            varValues[var] = binaryRow[col];
            latex += (binaryRow[col] ? "T" : "F") + std::string(" & ");
            ++col;
        }

        for (const auto &subExpr : subExpressions)
        {
            bool result = evaluateSubExpression(subExpr, varValues);
            latex += (result ? "T" : "F") + std::string(" & ");
        }

        latex.pop_back(); // Remove last &
        latex += "\\\\\n\\hline\n";
    }

    latex += "\\end{tabular}";
    return latex;
}

int main()
{
    std::string expression;
    std::cout << "逻辑关系表达式：";
    std::getline(std::cin, expression);
    std::set<std::string> variables = extractVariables(expression);
    std::string latex = generateLatex(variables, expression);
    std::cout << latex << std::endl;

    return 0;
}

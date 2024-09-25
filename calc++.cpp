#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <vector>
#include <cmath>

// Operator precedence and associativity
const std::map<char, int> opmap{{'+', 1}, {'-', 1}, {'*', 2}, {'/', 2}, {'^', 3}};

auto is_operator = [](char c) { return opmap.find(c) != opmap.end(); };
auto is_right_associative = [](char op) { return op == '^'; };

// Tokenizes the input expression into numbers, operators, and parentheses
std::vector<std::string> tokenize(const std::string& expr) {
    std::vector<std::string> tokens;
    std::string num;
    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (std::isdigit(c) || c == '.') num += c;                  // Collect number
        else {
            if (!num.empty()) tokens.push_back(num), num.clear();    // Push number
            if (std::isspace(c)) continue;
            if (c == '-' && (i == 0 || expr[i-1] == '(' || is_operator(expr[i-1]))) num += c;
            else tokens.push_back(std::string(1, c));                // Push operator or parentheses
        }
    }
    if (!num.empty()) tokens.push_back(num);  // Push remaining number
    return tokens;
}

// Convert infix to postfix using Shunting Yard algorithm
std::queue<std::string> infix_to_postfix(const std::vector<std::string>& infix) {
    std::stack<char> ops;
    std::queue<std::string> output;

    for (const auto& token : infix) {
        if (std::isdigit(token[0]) || (token[0] == '-' && token.size() > 1)) output.push(token);  // Push number
        else if (is_operator(token[0])) {
            while (!ops.empty() && is_operator(ops.top()) &&
                   ((is_right_associative(token[0]) && opmap.at(token[0]) < opmap.at(ops.top())) ||
                    (!is_right_associative(token[0]) && opmap.at(token[0]) <= opmap.at(ops.top()))))
                output.push(std::string(1, ops.top())), ops.pop();
            ops.push(token[0]);
        } else if (token == "(") ops.push('(');
        else if (token == ")") {
            while (ops.top() != '(') output.push(std::string(1, ops.top())), ops.pop();
            ops.pop();  // Pop '('
        }
    }
    while (!ops.empty()) output.push(std::string(1, ops.top())), ops.pop();
    return output;
}

// Evaluate the postfix expression
double evaluate_postfix(std::queue<std::string>& postfix) {
    std::stack<double> operands;
    while (!postfix.empty()) {
        const auto& token = postfix.front();
        postfix.pop();
        if (std::isdigit(token[0]) || (token[0] == '-' && token.size() > 1)) operands.push(std::stod(token));
        else {
            auto val2 = operands.top(); operands.pop();
            auto val1 = operands.top(); operands.pop();
            operands.push(token[0] == '+' ? val1 + val2 : token[0] == '-' ? val1 - val2 :
                          token[0] == '*' ? val1 * val2 : token[0] == '/' ? val1 / val2 : std::pow(val1, val2));
        }
    }
    return operands.top();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " \"expression\"\n";
        return 1;
    }

    std::string expr = argv[1];  // Get the expression from command-line argument
    auto postfix = infix_to_postfix(tokenize(expr));
    std::cout << "Result: " << evaluate_postfix(postfix) << "\n";

    return 0;
}

#include <iostream>
#include <stack>
#include <sstream>
#include <vector>
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <string> // Ensure string is included

using namespace std;

class SmartCalculator {
private:
    vector<string> history;

    bool isOperator(char op) {
        return op == '+' || op == '-' || op == '*' || op == '/' || op == '^';
    }

    int precedence(char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        if (op == '^') return 3;
        return 0;
    }

    double applyOp(double a, double b, char op) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/':
                if (b == 0) throw runtime_error("Division by zero");
                return a / b;
            case '^': return pow(a, b);
        }
        throw runtime_error("Unknown operator");
    }

    void process(stack<double>& values, stack<char>& ops) {
        // Check if we have enough values to perform an operation
        if (values.size() < 2 || ops.empty()) throw runtime_error("Invalid expression");

        double b = values.top(); values.pop();
        double a = values.top(); values.pop();
        char op = ops.top(); ops.pop();

        values.push(applyOp(a, b, op));
    }

    string cleanInput(const string& input) {
        string cleaned;
        for (size_t i = 0; i < input.length(); i++) {
            char c = input[i];
            if (isdigit((unsigned char)c) || 
                isOperator(c) || 
                c == '.' || c == '(' || c == ')' || c == ' ') {
                cleaned += c;
            }
        }
        return cleaned;
    }

public:
    double evaluate(const string& rawInput) {
        string expression = cleanInput(rawInput);
        if (expression.empty()) throw runtime_error("Empty input");

        stack<double> values;
        stack<char> ops;

        for (size_t i = 0; i < expression.length(); i++) {
            if (isspace(expression[i])) continue;

            // Handle Numbers
            if (isdigit((unsigned char)expression[i]) || expression[i] == '.') {
                string valStr;
                while (i < expression.length() &&
                      (isdigit((unsigned char)expression[i]) || expression[i] == '.')) {
                    valStr += expression[i];
                    i++;
                }
                i--; 
                
                stringstream ss(valStr);
                double num;
                ss >> num;
                values.push(num);
            }
            // Opening bracket
            else if (expression[i] == '(') {
                ops.push(expression[i]);
            }
            // Closing bracket
            else if (expression[i] == ')') {
                while (!ops.empty() && ops.top() != '(') {
                    process(values, ops);
                }
                if (ops.empty()) throw runtime_error("Mismatched parentheses");
                ops.pop(); // remove '('
            }
            // Operator
            else if (isOperator(expression[i])) {
                while (!ops.empty() && ops.top() != '(' &&
                       precedence(ops.top()) >= precedence(expression[i])) {
                    process(values, ops);
                }
                ops.push(expression[i]);
            }
        }

        while (!ops.empty()) {
            if (ops.top() == '(') throw runtime_error("Mismatched parentheses");
            process(values, ops);
        }

        if (values.empty()) throw runtime_error("Invalid expression");
        return values.top();
    }

    // FIXED: Using stringstream instead of to_string for Dev-C++ compatibility
    void addToHistory(const string& expr, double result) {
        stringstream ss;
        ss << expr << " = " << result;
        history.push_back(ss.str());
    }

    void showHistory() {
        if (history.empty()) {
            cout << "\nNo history yet." << endl;
            return;
        }
        cout << "\n--- History ---";
        for (size_t i = 0; i < history.size(); i++) {
            cout << "\n" << i + 1 << ". " << history[i];
        }
        cout << "\n---------------\n";
    }
};

int main() {
    SmartCalculator calc;
    string input;

    cout << "=== AI Smart Calculator ===\n";
    cout << "Example: 5 + 3 * (2 ^ 2)\n";
    cout << "Commands: history, exit\n";

    while (true) {
        cout << "\n> ";
        if (!getline(cin, input)) break;

        if (input == "exit") break;
        if (input == "history") {
            calc.showHistory();
            continue;
        }
        if (input.empty()) continue;

        try {
            double result = calc.evaluate(input);
            calc.addToHistory(input, result);
            cout << "Result: " << result << endl;
        } catch (exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    return 0;
}

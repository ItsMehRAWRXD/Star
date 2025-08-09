#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

class CodeAssistant {
public:
    std::string generateCode(const std::string& request) {
        std::string lowerRequest = request;
        std::transform(lowerRequest.begin(), lowerRequest.end(), lowerRequest.begin(), ::tolower);
        
        // Hello World
        if (lowerRequest.find("hello world") != std::string::npos) {
            if (lowerRequest.find("python") != std::string::npos) {
                return "print(\"Hello, World!\")\n";
            } else if (lowerRequest.find("java") != std::string::npos) {
                return "public class HelloWorld {\n"
                       "    public static void main(String[] args) {\n"
                       "        System.out.println(\"Hello, World!\");\n"
                       "    }\n"
                       "}\n";
            } else if (lowerRequest.find("javascript") != std::string::npos || 
                       lowerRequest.find("js") != std::string::npos) {
                return "console.log(\"Hello, World!\");\n";
            } else if (lowerRequest.find("c++") != std::string::npos || 
                       lowerRequest.find("cpp") != std::string::npos) {
                return "#include <iostream>\n\n"
                       "int main() {\n"
                       "    std::cout << \"Hello, World!\" << std::endl;\n"
                       "    return 0;\n"
                       "}\n";
            } else if (lowerRequest.find(" c") != std::string::npos) {
                return "#include <stdio.h>\n\n"
                       "int main() {\n"
                       "    printf(\"Hello, World!\\n\");\n"
                       "    return 0;\n"
                       "}\n";
            } else {
                // Default to C++
                return "#include <iostream>\n\n"
                       "int main() {\n"
                       "    std::cout << \"Hello, World!\" << std::endl;\n"
                       "    return 0;\n"
                       "}\n";
            }
        }
        
        // Calculator
        if (lowerRequest.find("calculator") != std::string::npos) {
            if (lowerRequest.find("python") != std::string::npos) {
                return "def add(a, b):\n"
                       "    return a + b\n\n"
                       "def subtract(a, b):\n"
                       "    return a - b\n\n"
                       "def multiply(a, b):\n"
                       "    return a * b\n\n"
                       "def divide(a, b):\n"
                       "    if b == 0:\n"
                       "        return \"Error: Division by zero\"\n"
                       "    return a / b\n\n"
                       "while True:\n"
                       "    print(\"\\nCalculator\")\n"
                       "    print(\"1. Add\")\n"
                       "    print(\"2. Subtract\")\n"
                       "    print(\"3. Multiply\")\n"
                       "    print(\"4. Divide\")\n"
                       "    print(\"5. Exit\")\n"
                       "    \n"
                       "    choice = input(\"Enter choice (1-5): \")\n"
                       "    \n"
                       "    if choice == '5':\n"
                       "        break\n"
                       "    \n"
                       "    if choice in ['1', '2', '3', '4']:\n"
                       "        num1 = float(input(\"Enter first number: \"))\n"
                       "        num2 = float(input(\"Enter second number: \"))\n"
                       "        \n"
                       "        if choice == '1':\n"
                       "            print(f\"{num1} + {num2} = {add(num1, num2)}\")\n"
                       "        elif choice == '2':\n"
                       "            print(f\"{num1} - {num2} = {subtract(num1, num2)}\")\n"
                       "        elif choice == '3':\n"
                       "            print(f\"{num1} * {num2} = {multiply(num1, num2)}\")\n"
                       "        elif choice == '4':\n"
                       "            print(f\"{num1} / {num2} = {divide(num1, num2)}\")\n";
            } else {
                // C++ calculator
                return "#include <iostream>\n"
                       "#include <cmath>\n\n"
                       "using namespace std;\n\n"
                       "int main() {\n"
                       "    double num1, num2, result;\n"
                       "    char operation;\n"
                       "    \n"
                       "    cout << \"Simple Calculator\" << endl;\n"
                       "    cout << \"Enter first number: \";\n"
                       "    cin >> num1;\n"
                       "    cout << \"Enter operation (+, -, *, /): \";\n"
                       "    cin >> operation;\n"
                       "    cout << \"Enter second number: \";\n"
                       "    cin >> num2;\n"
                       "    \n"
                       "    switch(operation) {\n"
                       "        case '+':\n"
                       "            result = num1 + num2;\n"
                       "            cout << num1 << \" + \" << num2 << \" = \" << result << endl;\n"
                       "            break;\n"
                       "        case '-':\n"
                       "            result = num1 - num2;\n"
                       "            cout << num1 << \" - \" << num2 << \" = \" << result << endl;\n"
                       "            break;\n"
                       "        case '*':\n"
                       "            result = num1 * num2;\n"
                       "            cout << num1 << \" * \" << num2 << \" = \" << result << endl;\n"
                       "            break;\n"
                       "        case '/':\n"
                       "            if (num2 != 0) {\n"
                       "                result = num1 / num2;\n"
                       "                cout << num1 << \" / \" << num2 << \" = \" << result << endl;\n"
                       "            } else {\n"
                       "                cout << \"Error: Division by zero!\" << endl;\n"
                       "            }\n"
                       "            break;\n"
                       "        default:\n"
                       "            cout << \"Invalid operation!\" << endl;\n"
                       "    }\n"
                       "    \n"
                       "    return 0;\n"
                       "}\n";
            }
        }
        
        // Fibonacci
        if (lowerRequest.find("fibonacci") != std::string::npos) {
            if (lowerRequest.find("python") != std::string::npos) {
                return "def fibonacci(n):\n"
                       "    if n <= 0:\n"
                       "        return []\n"
                       "    elif n == 1:\n"
                       "        return [0]\n"
                       "    elif n == 2:\n"
                       "        return [0, 1]\n"
                       "    \n"
                       "    fib = [0, 1]\n"
                       "    for i in range(2, n):\n"
                       "        fib.append(fib[i-1] + fib[i-2])\n"
                       "    return fib\n\n"
                       "n = int(input(\"Enter the number of Fibonacci numbers: \"))\n"
                       "print(f\"Fibonacci sequence: {fibonacci(n)}\")\n";
            } else {
                return "#include <iostream>\n"
                       "#include <vector>\n\n"
                       "using namespace std;\n\n"
                       "vector<int> fibonacci(int n) {\n"
                       "    vector<int> fib;\n"
                       "    if (n >= 1) fib.push_back(0);\n"
                       "    if (n >= 2) fib.push_back(1);\n"
                       "    \n"
                       "    for (int i = 2; i < n; i++) {\n"
                       "        fib.push_back(fib[i-1] + fib[i-2]);\n"
                       "    }\n"
                       "    return fib;\n"
                       "}\n\n"
                       "int main() {\n"
                       "    int n;\n"
                       "    cout << \"Enter the number of Fibonacci numbers: \";\n"
                       "    cin >> n;\n"
                       "    \n"
                       "    vector<int> fib = fibonacci(n);\n"
                       "    cout << \"Fibonacci sequence: \";\n"
                       "    for (int num : fib) {\n"
                       "        cout << num << \" \";\n"
                       "    }\n"
                       "    cout << endl;\n"
                       "    \n"
                       "    return 0;\n"
                       "}\n";
            }
        }
        
        // File reader
        if (lowerRequest.find("read file") != std::string::npos || 
            lowerRequest.find("file reader") != std::string::npos) {
            if (lowerRequest.find("python") != std::string::npos) {
                return "filename = input(\"Enter filename: \")\n\n"
                       "try:\n"
                       "    with open(filename, 'r') as file:\n"
                       "        content = file.read()\n"
                       "        print(\"File contents:\")\n"
                       "        print(content)\n"
                       "except FileNotFoundError:\n"
                       "    print(f\"Error: File '{filename}' not found.\")\n"
                       "except Exception as e:\n"
                       "    print(f\"Error reading file: {e}\")\n";
            } else {
                return "#include <iostream>\n"
                       "#include <fstream>\n"
                       "#include <string>\n\n"
                       "using namespace std;\n\n"
                       "int main() {\n"
                       "    string filename;\n"
                       "    cout << \"Enter filename: \";\n"
                       "    cin >> filename;\n"
                       "    \n"
                       "    ifstream file(filename);\n"
                       "    if (!file.is_open()) {\n"
                       "        cerr << \"Error: Could not open file '\" << filename << \"'\" << endl;\n"
                       "        return 1;\n"
                       "    }\n"
                       "    \n"
                       "    string line;\n"
                       "    cout << \"File contents:\" << endl;\n"
                       "    while (getline(file, line)) {\n"
                       "        cout << line << endl;\n"
                       "    }\n"
                       "    \n"
                       "    file.close();\n"
                       "    return 0;\n"
                       "}\n";
            }
        }
        
        // Bubble sort
        if (lowerRequest.find("bubble sort") != std::string::npos || 
            lowerRequest.find("sort") != std::string::npos) {
            if (lowerRequest.find("python") != std::string::npos) {
                return "def bubble_sort(arr):\n"
                       "    n = len(arr)\n"
                       "    for i in range(n):\n"
                       "        for j in range(0, n - i - 1):\n"
                       "            if arr[j] > arr[j + 1]:\n"
                       "                arr[j], arr[j + 1] = arr[j + 1], arr[j]\n"
                       "    return arr\n\n"
                       "# Test the function\n"
                       "numbers = [64, 34, 25, 12, 22, 11, 90]\n"
                       "print(f\"Original array: {numbers}\")\n"
                       "bubble_sort(numbers)\n"
                       "print(f\"Sorted array: {numbers}\")\n";
            } else {
                return "#include <iostream>\n"
                       "#include <vector>\n\n"
                       "using namespace std;\n\n"
                       "void bubbleSort(vector<int>& arr) {\n"
                       "    int n = arr.size();\n"
                       "    for (int i = 0; i < n - 1; i++) {\n"
                       "        for (int j = 0; j < n - i - 1; j++) {\n"
                       "            if (arr[j] > arr[j + 1]) {\n"
                       "                swap(arr[j], arr[j + 1]);\n"
                       "            }\n"
                       "        }\n"
                       "    }\n"
                       "}\n\n"
                       "int main() {\n"
                       "    vector<int> numbers = {64, 34, 25, 12, 22, 11, 90};\n"
                       "    \n"
                       "    cout << \"Original array: \";\n"
                       "    for (int num : numbers) {\n"
                       "        cout << num << \" \";\n"
                       "    }\n"
                       "    cout << endl;\n"
                       "    \n"
                       "    bubbleSort(numbers);\n"
                       "    \n"
                       "    cout << \"Sorted array: \";\n"
                       "    for (int num : numbers) {\n"
                       "        cout << num << \" \";\n"
                       "    }\n"
                       "    cout << endl;\n"
                       "    \n"
                       "    return 0;\n"
                       "}\n";
            }
        }
        
        return "I can generate code for:\n"
               "- hello world\n"
               "- calculator\n"
               "- fibonacci\n"
               "- file reader\n"
               "- bubble sort\n"
               "\nTry: 'hello world in python' or 'calculator in c++'\n";
    }
};

int main() {
    CodeAssistant assistant;
    std::string request;
    
    std::cout << "Code Assistant\n";
    std::cout << "Tell me what code you want (type 'exit' to quit)\n\n";
    
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, request);
        
        if (request == "exit" || request == "quit") {
            break;
        }
        
        if (request.empty()) {
            continue;
        }
        
        std::string code = assistant.generateCode(request);
        std::cout << "\n" << code << "\n";
    }
    
    return 0;
}
#include <string>
#include <iostream>
#include <vector>
#include <codecvt>
#include <locale>
#include <cmath>
namespace calculation
{
    double xrooty(double x, double y)
    {
        return pow(y, 1 / x);
    }
    double factorial(int num)
    {
        double result = 1;
        for (int i = 0; i < num; i++)
        {
            result *= (i + 1);
        }
        return result;
    }
    // 1, 0, -1, 0, ...
    // https://socratic.org/questions/how-do-you-find-the-taylor-series-of-f-x-cos-x
    double costyler(double degrees, int perceison = 15)
    {
        while (degrees > 360)
        {
            degrees -= 360;
        }
        while (degrees < -360)
        {
            degrees += 360;
        }
        double result = 1;
        bool is_positive = false;
        double radiant = degrees * ((double)3.141592653589793238 / 180);
        for (int i = 1; i < perceison; i++)
        {
            double denominater = factorial(i * 2);
            double numerator = pow(radiant, i * 2);
            double multiplier = is_positive ? 1 : -1;
            result += (numerator / denominater) * multiplier;
            is_positive = !is_positive;
        }
        return result;
    };
    // 0, 1, 0, -1
    double sintyler(double degrees, int perceison = 15)
    {
        while (degrees > 360)
        {
            degrees -= 360;
        }
        while (degrees < -360)
        {
            degrees += 360;
        }
        bool is_positive = false;
        double radiant = degrees * ((double)3.141592653589793238462643383279 / 180);
        double result = radiant;
        for (int i = 1; i < perceison; i++)
        {
            double denominater = factorial((i * 2) + 1);
            double numerator = pow(radiant, (i * 2) + 1);
            double multiplier = is_positive ? 1 : -1;
            result += (numerator / denominater) * multiplier;
            is_positive = !is_positive;
        }
        return result;
    }
    double tantyler(double degrees, int perceison = 15)
    {
        while (degrees > 360)
        {
            degrees -= 360;
        }
        while (degrees < -360)
        {
            degrees += 360;
        }
        double radiant = degrees * ((double)3.141592653589793238 / 180);
        return tan(radiant);
    };

    std::string ws2s(const std::wstring &utf16Str)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(utf16Str);
    }
    std::wstring s2ws(const std::string &utf8Str)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.from_bytes(utf8Str);
    }

    struct token
    {
        std::string type;
        double value = -1;
        std::string value_string;
    };
    std::vector<token> tokenizer(std::wstring calculation)
    {
        bool is_error = false;
        bool in_number = false;
        bool in_operator = false;
        int paranthesesLayer = 0;

        std::string current_token;
        std::vector<token> result;

        for (int i = 0; i < calculation.size(); i++)
        {
            if (is_error)
            {
                return std::vector<token>{};
            }
            std::string curr = ws2s(std::wstring{calculation[i]});
            std::string prev = i > 0 ? ws2s(std::wstring{calculation[i - 1]}) : "START";
            std::string next = ((i + 1) < calculation.size()) ? ws2s(std::wstring{calculation[i + 1]}) : "END";

            current_token += curr;
            // Is currently parsing number
            if (in_number)
            {
                if ((std::string{"0123456789."}).find(next) == std::string::npos)
                {
                    in_number = false;
                    double parsed_num = atof(current_token.c_str());
                    current_token = "";
                    result.push_back(token{"number", parsed_num});
                }
                continue;
            }

            // Is in parantheses
            if (paranthesesLayer > 0 && curr != ")")
            {
                continue;
            }

            // Is in multi-char operator (eg [COS])
            if (in_operator)
            {
                if (curr == "]")
                {
                    current_token.pop_back();
                    in_operator = false;
                    result.push_back(token{current_token});
                    current_token = "";
                }
                continue;
            }

            // Check for number
            if ((std::string{"0123456789"}).find(curr) != std::string::npos)
            {
                in_number = true;

                // Check for 1 character number
                if ((std::string{"0123456789."}).find(next) == std::string::npos)
                {
                    in_number = false;
                    double parsed_num = atof(current_token.c_str());
                    current_token = "";
                    result.push_back(token{"number", parsed_num});
                }
                continue;
            }

            // Check for basic operations

            if (curr == "+")
            {
                current_token = "";
                result.push_back(token{"plus"});
                continue;
            }
            if (curr == "-")
            {
                if (i == 0)
                {
                    in_number = true;
                    continue;
                }
                current_token = "";
                result.push_back(token{"minus"});
                continue;
            }
            if (curr == "×")
            {
                current_token = "";
                result.push_back(token{"multiply"});
                continue;
            }
            if (curr == "÷")
            {
                current_token = "";
                result.push_back(token{"divide"});
                continue;
            }
            if (curr == "^")
            {
                current_token = "";
                result.push_back(token{"power"});
                continue;
            }
            if (curr == "√")
            {
                current_token = "";
                result.push_back(token{"sqrt"});
            }

            // Factorial
            if (curr == "!")
            {
                current_token = "";
                result.push_back(token{"factorial"});
            }

            // Check for PI
            if (curr == "π")
            {
                current_token = "";
                result.push_back(token{"number", (double)3.141592653589793238});
                continue;
            }

            // Check for special operators (eg [COS])
            if (curr == "[")
            {
                current_token = "";
                in_operator = true;

                continue;
            }

            // Check for parentheses
            if (curr == "(")
            {
                paranthesesLayer++;
                continue;
            }
            if (curr == ")")
            {
                paranthesesLayer--;
                if (paranthesesLayer == 0)
                {
                    result.push_back(token{"parantheses", value_string : current_token});
                    current_token = "";
                }
            }
        }
        return result;
    }
    double calculate(std::wstring calculation)
    {
        auto tokenized = tokenizer(calculation);
        bool is_error = tokenized.size() == 0;
        if (is_error)
        {
            return NAN;
        }
        // Start with first number
        double result = tokenized[0].value;
        // Follow PEMDAS
        for (int i = 0; i < tokenized.size(); i++)
        {
            // first do parantheses
            // replace parantheses in vector with number
            if (tokenized[i].type == "parantheses")
            {
                std::string without_parantheses = tokenized[i].value_string.substr(1, tokenized[i].value_string.size() - 2);
                tokenized[i].value = calculate(s2ws(without_parantheses));
                tokenized[i].type = "number";
                tokenized[i].value_string = "";
            }
        }
        // Exponents
        for (int i = 0; i < tokenized.size(); i++)
        {
            if (tokenized[i].type == "power")
            {
                tokenized[i + 1].value = (double)pow(tokenized[i - 1].value, tokenized[i + 1].value);
                tokenized[i + 1].type = "number";
                tokenized.erase(tokenized.begin() + i - 1);
                tokenized.erase(tokenized.begin() + i - 1);
                i--;
            }
            if (tokenized[i].type == "sqrt")
            {
                tokenized[i - 1].value = xrooty(2, tokenized[i - 1].value);
                tokenized.erase(tokenized.begin() + i);
                i--;
            }
        }
        // Factorial, cos
        for (int i = 0; i < tokenized.size(); i++)
        {
            if (tokenized[i].type == "factorial")
            {
                tokenized[i - 1].value = factorial(tokenized[i - 1].value);
                tokenized.erase(tokenized.begin() + i);
            }
            if (tokenized[i].type == "COS")
            {
                tokenized[i - 1].value = costyler(tokenized[i - 1].value);
                tokenized.erase(tokenized.begin() + i);
            }
            if (tokenized[i].type == "SIN")
            {
                tokenized[i - 1].value = sintyler(tokenized[i - 1].value);
                tokenized.erase(tokenized.begin() + i);
            }
            if(tokenized[i].type == "TAN"){
                tokenized[i-1].value = tantyler(tokenized[i-1].value);
                tokenized.erase(tokenized.begin() + i);
            }
        }

        // Multiply
        for (int i = 0; i < tokenized.size(); i++)
        {
            if (tokenized[i].type == "multiply")
            {
                tokenized[i + 1].value = tokenized[i - 1].value * tokenized[i + 1].value;
                tokenized[i + 1].type = "number";
                tokenized.erase(tokenized.begin() + i - 1);
                tokenized.erase(tokenized.begin() + i - 1);
                i--;
            }
            if (tokenized[i].type == "divide")
            {
                tokenized[i + 1].value = tokenized[i - 1].value / tokenized[i + 1].value;
                tokenized[i + 1].type = "number";
                tokenized.erase(tokenized.begin() + i - 1);
                tokenized.erase(tokenized.begin() + i - 1);
                i--;
            }
        }

        // Plus and minus
        for (int i = 0; i < tokenized.size(); i++)
        {
            if (tokenized[i].type == "plus")
            {
                tokenized[i + 1].value = tokenized[i - 1].value + tokenized[i + 1].value;
                tokenized[i + 1].type = "number";
                tokenized.erase(tokenized.begin() + i - 1);
                tokenized.erase(tokenized.begin() + i - 1);
                i--;
            }
            if (tokenized[i].type == "minus")
            {
                tokenized[i + 1].value = tokenized[i - 1].value - tokenized[i + 1].value;
                tokenized[i + 1].type = "number";
                tokenized.erase(tokenized.begin() + i - 1);
                tokenized.erase(tokenized.begin() + i - 1);
                i--;
            }
        }
        if (tokenized.size() > 1)
        {
            return NAN;
        }
        result = tokenized[0].value;
        return result;
    }
}
int main()
{
    std::string user_calculation;
    std::cin >> user_calculation;
    std::wstring calculation = calculation::s2ws(user_calculation);
    std::cout << "rechnung: " << calculation::ws2s(calculation) << std::endl
              << std::endl;
    auto result = calculation::tokenizer(calculation);
    for (auto tok : result)
    {
        std::cout << "typ: " << tok.type;
        if (tok.value != -1)
        {
            std::cout << " wert: " << tok.value;
        }
        if (tok.value_string.size() != 0)
        {
            std::cout << " wert: " << tok.value_string;
        }
        std::cout << std::endl;
    }
    std::cout << "result: " << calculation::calculate(calculation) << std::endl;
}
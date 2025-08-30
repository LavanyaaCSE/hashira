#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <cctype> // For std::isspace

// A simple structure to hold a share's data.
struct Share {
    std::string x;
    std::string y;
    int base;
};

// --- BigInt Class Implementation (from scratch) ---
// This class handles arithmetic for arbitrarily large numbers.
class BigInt {
public:
    std::string value;
    bool is_negative;

    BigInt(std::string val = "0", bool neg = false) : is_negative(neg) {
        if (val.empty()) {
            value = "0";
        } else {
            value = removeLeadingZeros(val);
        }
        if (value == "0") {
            is_negative = false;
        }
    }

    // Helper to remove leading zeros from a number string.
    static std::string removeLeadingZeros(const std::string& s) {
        size_t first_digit = s.find_first_not_of('0');
        if (std::string::npos == first_digit) {
            return "0";
        }
        return s.substr(first_digit);
    }

    // Comparison operators for BigInt
    bool operator<(const BigInt& other) const {
        if (is_negative != other.is_negative) {
            return is_negative;
        }
        if (value.length() != other.value.length()) {
            return (value.length() < other.value.length()) != is_negative;
        }
        return (value < other.value) != is_negative;
    }

    bool operator==(const BigInt& other) const {
        return value == other.value && is_negative == other.is_negative;
    }

    bool operator!=(const BigInt& other) const {
        return !(*this == other);
    }

    // Unsigned comparison
    static int compareUnsigned(const std::string& s1, const std::string& s2) {
        if (s1.length() > s2.length()) return 1;
        if (s1.length() < s2.length()) return -1;
        if (s1 > s2) return 1;
        if (s1 < s2) return -1;
        return 0;
    }

    // Addition operation
    BigInt add(const BigInt& other) const {
        if (is_negative == other.is_negative) {
            std::string result = addStrings(value, other.value);
            return BigInt(result, is_negative);
        } else {
            int cmp = compareUnsigned(value, other.value);
            if (cmp == 0) return BigInt("0");
            std::string result = (cmp > 0) ? subtractStrings(value, other.value) : subtractStrings(other.value, value);
            bool result_negative = (cmp > 0) ? is_negative : other.is_negative;
            return BigInt(result, result_negative);
        }
    }

    // Subtraction operation
    BigInt subtract(const BigInt& other) const {
        BigInt temp = other;
        temp.is_negative = !temp.is_negative;
        return this->add(temp);
    }

    // Multiplication operation
    BigInt multiply(const BigInt& other) const {
        std::string s1 = value;
        std::string s2 = other.value;
        if (s1 == "0" || s2 == "0") return BigInt("0");

        std::vector<int> result(s1.length() + s2.length(), 0);
        int i_n1 = 0;
        int i_n2 = 0;

        for (int i = s1.length() - 1; i >= 0; i--) {
            int carry = 0;
            int n1 = s1[i] - '0';
            i_n2 = 0;

            for (int j = s2.length() - 1; j >= 0; j--) {
                int n2 = s2[j] - '0';
                int sum = n1 * n2 + result[i_n1 + i_n2] + carry;
                carry = sum / 10;
                result[i_n1 + i_n2] = sum % 10;
                i_n2++;
            }
            if (carry > 0) {
                result[i_n1 + i_n2] += carry;
            }
            i_n1++;
        }

        std::string res_str = "";
        int i = result.size() - 1;
        while (i >= 0 && result[i] == 0) i--;
        if (i == -1) return BigInt("0");

        while (i >= 0) {
            res_str += std::to_string(result[i--]);
        }
        return BigInt(res_str, is_negative != other.is_negative);
    }

    // Division operation, fixed for handling signs correctly
    BigInt divide(const BigInt& other) const {
        if (other.value == "0") throw std::runtime_error("Division by zero");

        bool result_is_negative = is_negative != other.is_negative;
        BigInt abs_dividend = is_negative ? BigInt(value, false) : *this;
        BigInt abs_divisor = other.is_negative ? BigInt(other.value, false) : other;

        if (compareUnsigned(abs_dividend.value, abs_divisor.value) < 0) return BigInt("0");

        std::string quotient_str = "";
        BigInt current_dividend("0");

        for (char digit_char : abs_dividend.value) {
            current_dividend = current_dividend.multiply(BigInt("10")).add(BigInt(std::string(1, digit_char)));
            int count = 0;
            while (compareUnsigned(current_dividend.value, abs_divisor.value) >= 0) {
                current_dividend = current_dividend.subtract(abs_divisor);
                count++;
            }
            quotient_str += std::to_string(count);
        }
        
        return BigInt(quotient_str, result_is_negative);
    }

    // Convert from a different base string to BigInt (base 10)
    static BigInt fromBase(const std::string& val, int base) {
        if (base < 2 || base > 36) throw std::runtime_error("Invalid base");
        BigInt result("0");
        BigInt power_of_base("1");
        BigInt b(std::to_string(base));
        for (int i = val.length() - 1; i >= 0; --i) {
            int digit;
            if (val[i] >= '0' && val[i] <= '9') {
                digit = val[i] - '0';
            } else if (val[i] >= 'a' && val[i] <= 'z') {
                digit = val[i] - 'a' + 10;
            } else {
                throw std::runtime_error("Invalid character in base conversion");
            }
            if (digit >= base) throw std::runtime_error("Invalid digit for base");
            
            result = result.add(power_of_base.multiply(BigInt(std::to_string(digit))));
            power_of_base = power_of_base.multiply(b);
        }
        return result;
    }

private:
    // Helper function to add two positive strings
    static std::string addStrings(const std::string& s1, const std::string& s2) {
        std::string result = "";
        int i = s1.length() - 1;
        int j = s2.length() - 1;
        int carry = 0;
        while (i >= 0 || j >= 0 || carry) {
            int sum = carry;
            if (i >= 0) sum += s1[i--] - '0';
            if (j >= 0) sum += s2[j--] - '0';
            result += std::to_string(sum % 10);
            carry = sum / 10;
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    // Helper function to subtract two positive strings (assuming s1 > s2)
    static std::string subtractStrings(const std::string& s1, const std::string& s2) {
        std::string result = "";
        std::string s_larger = s1, s_smaller = s2;
        int n1 = s_larger.length(), n2 = s_smaller.length();
        std::reverse(s_larger.begin(), s_larger.end());
        std::reverse(s_smaller.begin(), s_smaller.end());
        int carry = 0;
        for (int i = 0; i < n2; i++) {
            int sub = ((s_larger[i] - '0') - (s_smaller[i] - '0') - carry);
            if (sub < 0) {
                sub = sub + 10;
                carry = 1;
            } else {
                carry = 0;
            }
            result.push_back(sub + '0');
        }
        for (int i = n2; i < n1; i++) {
            int sub = ((s_larger[i] - '0') - carry);
            if (sub < 0) {
                sub = sub + 10;
                carry = 1;
            } else {
                carry = 0;
            }
            result.push_back(sub + '0');
        }
        std::reverse(result.begin(), result.end());
        return removeLeadingZeros(result);
    }
};

// --- JSON Parser (from scratch) ---
// This function manually parses a simple JSON string to extract required data.
void parseJSON(const std::string& json, int& n, int& k, std::vector<Share>& shares) {
    size_t pos = 0;
    
    // Find and parse "n"
    pos = json.find("\"n\":", pos);
    if (pos == std::string::npos) return;
    pos += 4;
    size_t end_pos = json.find_first_of(",}", pos);
    n = std::stoi(json.substr(pos, end_pos - pos));

    // Find and parse "k"
    pos = json.find("\"k\":", pos);
    if (pos == std::string::npos) return;
    pos += 4;
    end_pos = json.find_first_of(",}", pos);
    k = std::stoi(json.substr(pos, end_pos - pos));

    pos = end_pos + 1; // Start searching after the "keys" object
    
    // Find and parse all share entries
    while (true) {
        size_t key_start = json.find('"', pos);
        if (key_start == std::string::npos) break;
        size_t key_end = json.find('"', key_start + 1);
        std::string share_key = json.substr(key_start + 1, key_end - key_start - 1);
        
        size_t obj_start = json.find('{', key_end);
        size_t obj_end = json.find('}', obj_start);
        std::string obj_str = json.substr(obj_start, obj_end - obj_start + 1);

        Share s;
        s.x = share_key;

        // Parse base
        size_t base_pos = obj_str.find("\"base\":");
        size_t base_val_start = obj_str.find('"', base_pos + 7) + 1;
        size_t base_val_end = obj_str.find('"', base_val_start);
        s.base = std::stoi(obj_str.substr(base_val_start, base_val_end - base_val_start));

        // Parse value
        size_t val_pos = obj_str.find("\"value\":");
        size_t val_val_start = obj_str.find('"', val_pos + 8) + 1;
        size_t val_val_end = obj_str.find('"', val_val_start);
        s.y = obj_str.substr(val_val_start, val_val_end - val_val_start);
        
        shares.push_back(s);
        pos = obj_end + 1;
    }
}

// --- Combinations Generator (recursive) ---
// Generates all combinations of k shares from the given vector of shares.
void generateCombinations(const std::vector<Share>& all_shares, int start_index, int k, std::vector<Share>& current_combination, std::vector<std::vector<Share>>& all_combinations) {
    if (k == 0) {
        all_combinations.push_back(current_combination);
        return;
    }
    for (int i = start_index; i <= (int)all_shares.size() - k; ++i) {
        current_combination.push_back(all_shares[i]);
        generateCombinations(all_shares, i + 1, k - 1, current_combination, all_combinations);
        current_combination.pop_back();
    }
}

BigInt findSecret(const std::vector<Share>& shares) {
    BigInt secret("0");
    int k = shares.size();

    std::vector<BigInt> x_vals, y_vals;
    for (const auto& s : shares) {
        x_vals.push_back(BigInt(s.x));
        y_vals.push_back(BigInt::fromBase(s.y, s.base));
    }

    for (int j = 0; j < k; ++j) {
        BigInt y_j = y_vals[j];
        BigInt numerator("1");
        BigInt denominator("1");

        for (int i = 0; i < k; ++i) {
            if (i != j) {
                // Numerator: (-x_i)
                numerator = numerator.multiply(BigInt("0").subtract(x_vals[i]));
                // Denominator: (x_j - x_i)
                denominator = denominator.multiply(x_vals[j].subtract(x_vals[i]));
            }
        }
        BigInt term = y_j.multiply(numerator).divide(denominator);
        secret = secret.add(term);
    }

    return secret;
}

// --- Main Function ---
int main() {
    // Test case 1 
    const std::string test_case_1 = R"({
        "keys": {
            "n": 4,
            "k": 3
        },
        "1": { "base": "10", "value": "4" },
        "2": { "base": "2", "value": "111" },
        "3": { "base": "10", "value": "12" },
        "6": { "base": "4", "value": "213" }
    })";

    // Test case 2 
    const std::string test_case_2 = R"({
        "keys": {
            "n": 10,
            "k": 7
        },
        "1": { "base": "6", "value": "13444211440455345511" },
        "2": { "base": "15", "value": "aed7015a346d635" },
        "3": { "base": "15", "value": "6aeeb69631c227c" },
        "4": { "base": "16", "value": "e1b5e05623d881f" },
        "5": { "base": "8", "value": "316034514573652620673" },
        "6": { "base": "3", "value": "2122212201122002221120200210011020220200" },
        "7": { "base": "3", "value": "20120221122211000100210021102001201112121" },
        "8": { "base": "6", "value": "20220554335330240002224253" },
        "9": { "base": "12", "value": "45153788322a1255483" },
        "10": { "base": "7", "value": "1101613130313526312514143" }
    })";


    // Run Test Case 1
    std::cout << "--- Running Test Case 1 ---" << std::endl;
    try {
        int n, k;
        std::vector<Share> all_shares;
        parseJSON(test_case_1, n, k, all_shares);
        
        std::vector<std::vector<Share>> all_combinations;
        std::vector<Share> current_combination;
        generateCombinations(all_shares, 0, k, current_combination, all_combinations);

        std::map<std::string, int> secret_counts;
        for (const auto& combo : all_combinations) {
            BigInt secret = findSecret(combo);
            secret_counts[secret.value]++;
        }
        
        std::string correct_secret_str;
        int max_count = 0;
        for (const auto& pair : secret_counts) {
            if (pair.second > max_count) {
                max_count = pair.second;
                correct_secret_str = pair.first;
            }
        }
        
        std::cout << "Correct Secret: " << correct_secret_str << std::endl;

        std::map<std::string, bool> keys_in_good_combos;
        for (const auto& combo : all_combinations) {
            BigInt secret = findSecret(combo);
            if (secret.value == correct_secret_str) {
                for (const auto& share : combo) {
                    keys_in_good_combos[share.x] = true;
                }
            }
        }

        std::cout << "Incorrect Key(s): ";
        bool found_bad = false;
        for (const auto& share : all_shares) {
            if (keys_in_good_combos.find(share.x) == keys_in_good_combos.end()) {
                std::cout << share.x << " ";
                found_bad = true;
            }
        }
        if (!found_bad) {
            std::cout << "None";
        }
        std::cout << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "\n--- Running Test Case 2 ---" << std::endl;

    // Run Test Case 2
    try {
        int n, k;
        std::vector<Share> all_shares;
        parseJSON(test_case_2, n, k, all_shares);

        std::vector<std::vector<Share>> all_combinations;
        std::vector<Share> current_combination;
        generateCombinations(all_shares, 0, k, current_combination, all_combinations);

        std::map<std::string, int> secret_counts;
        for (const auto& combo : all_combinations) {
            BigInt secret = findSecret(combo);
            secret_counts[secret.value]++;
        }
        
        std::string correct_secret_str;
        int max_count = 0;
        for (const auto& pair : secret_counts) {
            if (pair.second > max_count) {
                max_count = pair.second;
                correct_secret_str = pair.first;
            }
        }
        
        std::cout << "Correct Secret: " << correct_secret_str << std::endl;

        std::map<std::string, bool> keys_in_good_combos;
        for (const auto& combo : all_combinations) {
            BigInt secret = findSecret(combo);
            if (secret.value == correct_secret_str) {
                for (const auto& share : combo) {
                    keys_in_good_combos[share.x] = true;
                }
            }
        }

        std::cout << "Incorrect Key(s): ";
        bool found_bad = false;
        for (const auto& share : all_shares) {
            if (keys_in_good_combos.find(share.x) == keys_in_good_combos.end()) {
                std::cout << share.x << " ";
                found_bad = true;
            }
        }
        if (!found_bad) {
            std::cout << "None";
        }
        std::cout << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
# BASIC_DSA_PROBLEMS

This repository contains a C++ implementation of Shamir’s Secret Sharing Scheme, allowing you to:

1. Reconstruct a secret from a set of shares.

2. Handle shares in different bases (e.g., binary, decimal, hexadecimal).

3. Perform arithmetic on very large numbers using a custom BigInt class.

4. Parse shares from a JSON-like input format and identify correct and incorrect keys.

Shamir’s Secret Sharing is a cryptographic algorithm that splits a secret into multiple parts (n shares), requiring at least k shares to reconstruct the secret.

## Features
1. Full arbitrary-precision integer arithmetic (addition, subtraction, multiplication, division).

2. JSON-like input parsing for n, k, and shares with base conversion.

3. Lagrange interpolation for secret reconstruction.

4. Identification of invalid or incorrect shares.

## Code Structure

BigInt class: Implements arbitrary-precision integers, supports arithmetic operations, and base conversion.

Share struct: Represents a single share with x-coordinate, y-coordinate, and base.

parseJSON(): Parses input JSON string and extracts n, k, and all shares.

generateCombinations(): Generates all combinations of shares of size k.

findSecret(): Reconstructs the secret using Lagrange interpolation.

main(): Runs test cases, prints the reconstructed secret, and lists incorrect keys.

## 📌 How to Use

1. Clone the repository:

```
git clone https://github.com/your-username/basic-dsa-problems.git
```
2. Compile:

```
g++ -std=c++17 main.cpp -o shamir
```
3. Run:

```
./shamir
```

## How it works

1. Parse JSON: Extract n, k, and all shares.

2. Generate Combinations: Form all possible subsets of shares of size k.

3. Reconstruct Secret: For each combination, use findSecret():

4. Compute numerator as product of -x_i terms.

5. Compute denominator as product of (x_j - x_i) terms.

6. Divide numerator by denominator to get Lagrange coefficient.

7. Multiply by y_j and sum all terms to get the secret.

8. Identify Incorrect Keys: Any share not appearing in a combination producing the correct secret is marked invalid.

## Notes

1. The program supports arbitrary bases from 2 to 36.

2. Big numbers are handled using the custom BigInt class.

3. Designed for educational and demonstration purposes; for production-grade cryptography, use vetted libraries.

## ⭐ Support
If this repository helped you, give it a star ⭐!

# Ciphers – Classical Encryption & Decryption Toolkit (C++)

This project is a C++ console application that lets you experiment with classical ciphers such as **Caesar** and **substitution ciphers**.  
It supports:

- Encrypting and decrypting text with the **Caesar cipher**
- Applying a **random substitution cipher** to text
- Scoring how "English-like" a string is using **quadgrams**
- **Breaking substitution ciphers** with a hill-climbing algorithm using quadgram frequencies

The program is menu-driven and runs entirely in the terminal.

---

## 🧩 Features (Step-by-Step)

When you run the program, you see a **menu**:

- `C` – Encrypt with Caesar Cipher  
- `D` – Decrypt Caesar Cipher  
- `E` – Compute English-ness Score  
- `A` – Apply Random Substitution Cipher  
- `S` – Decrypt Substitution Cipher from Console  
- `F` – Decrypt Substitution Cipher from File  
- `R` – Set Random Seed for Testing  
- `X` – Exit Program  

Below is what each option does internally.

---

### 1. Caesar Encryption (`C`)

1. The program asks the user to **enter text** to encrypt.
2. It then asks for a **shift amount** (an integer).
3. Each alphabetic character is:
   - Converted to uppercase,
   - Rotated forward by the given shift within `A–Z` using the `rot(char, int)` function.
4. Non-letter characters are ignored or treated as spaces (so punctuation is not encrypted).
5. The encrypted text is printed to the console.

Core function:  

- `char rot(char c, int amount)` – rotates a single character within the alphabet.  
- `string rot(const string& line, int amount)` – applies rotation to a full string.

---

### 2. Caesar Decryption (`D`)

1. The program loads a **dictionary** from `dictionary.txt` into a `vector<string>`.
2. The user enters the **ciphertext**.
3. The text is split into words and rotated with **all 26 possible shifts**.
4. For each shift:
   - The program counts how many words match the dictionary (`numWordsIn`).
   - If more than half of the words are dictionary words, that line is printed as a **likely decryption**.
5. If no shift produces enough matches, it prints: `"No good decryptions found"`.

This uses:

- `splitBySpaces` – splits a line into words.  
- `rot(vector<string>& strings, int amount)` – rotates each word.  
- `numWordsIn` – counts how many words appear in the dictionary.

---

### 3. English-ness Score (`E`)

1. The program loads **quadgrams** and their counts from `english_quadgrams.txt`.
2. It creates a `QuadgramScorer` using those quadgrams and frequencies.
3. The user enters a string.
4. The `scoreString` function:
   - Cleans the string to uppercase letters only.
   - Slides a window of length 4 over the string to extract quadgrams.
   - Adds up the score from the `QuadgramScorer` for each quadgram.
5. The final score is printed as the **"English-ness"** of the string.

Core function:  

- `double scoreString(const QuadgramScorer& scorer, const string& s)`

---

### 4. Apply Random Substitution Cipher (`A`)

1. The program generates a **random substitution cipher** using `genRandomSubstCipher()`.
2. The user enters plaintext text.
3. For every letter:
   - It finds the letter’s index in the constant `ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"`.
   - It substitutes it with the cipher letter at that index.
4. Non-letter characters are preserved as-is.
5. The encrypted result is printed to the console.

Core function:  

- `string applySubstCipher(const vector<char>& cipher, const string& s)`

---

### 5. Decrypt Substitution Cipher (`S` and `F`)

Both `S` and `F` use the same underlying function: `decryptSubstCipherCommand`.

- `S` – console-based decryption  
- `F` – file-based decryption  

#### a. Input Mode

1. The user is prompted for an **input filename**.
2. If a filename is provided and the file exists:
   - The program reads the entire **ciphertext from the file**.
   - Then prompts for an **output filename**.
3. If the file cannot be opened:
   - The entered string is treated directly as the ciphertext.
4. If the input filename is left blank:
   - The program prompts: `"Enter text to decrypt:"` and reads ciphertext from the console.

#### b. Hill-Climbing Decryption

To break the substitution cipher:

1. A **random substitution key** is generated.
2. The ciphertext is decrypted using that key.
3. The resulting plaintext is scored using quadgrams (`scoreString`).
4. The algorithm repeatedly:
   - Swaps two letters in the key.
   - Re-decrypts the text with this new key.
   - Accepts the new key if the score improves.
5. This runs until there’s no improvement after a fixed number of iterations (`maxNoImprovement`).
6. To avoid local maxima, the algorithm runs **multiple times** (e.g., 10 restarts) and keeps the best-scoring decryption.

Core function:

- `string hillClimbDecrypt(const string& ciphertext, const QuadgramScorer& scorer)`

The best decryption is then either:

- Printed to the console (`Best decryption: ...`), or  
- Written to the output file in file mode.

---

### 6. Random Seed (`R`)

- The user can set a **specific random seed** for reproducible results.  
- Uses `Random::seed(stoi(seed_str));`  
- This controls:
  - Random substitution keys  
  - Any other randomized behavior  

If the input is not a valid integer, an error message is shown.

---

## ⚙️ Dependencies & Files

The project uses:

- `utils.h` + `Random` class with `std::mt19937`
- Headers:
  - `include/caesar_dec.h`
  - `include/caesar_enc.h`
  - `include/subst_dec.h`
  - `include/subst_enc.h`
- Data files:
  - `dictionary.txt` – list of English words (one per line)
  - `english_quadgrams.txt` – lines of the form `QUAD,COUNT`

Standard libraries used include `<algorithm>`, `<cctype>`, `<cmath>`, `<cstdlib>`, `<ctime>`, `<fstream>`, `<iostream>`, `<limits>`, `<random>`, `<sstream>`, `<string>`, and `<vector>`.

---

## 🔧 How to Build and Run

From the project root (adjust filenames as needed):

```bash
g++ -std=c++17 -o ciphers main.cpp \
    -Iinclude


---

## ▶️ Video Demo
https://uofi.box.com/s/gq5yc9s5gas7kfv6dyjtw62l4uee3zo6

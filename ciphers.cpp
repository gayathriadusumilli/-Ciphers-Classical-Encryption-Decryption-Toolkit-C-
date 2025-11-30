#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void printMenu();

int main() {
  // Seed the random number generator using Random::seed()
  Random::seed(time(NULL));
  string command;

  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  while (true) {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    if (!getline(cin, command) || command == "X" || command == "x") {
      cout << "Exiting program." << endl;
      break;
    }
    cout << endl;

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      if (getline(cin, seed_str)) {
        try {
          Random::seed(stoi(seed_str));
        } catch (const invalid_argument&) {
          cout << "Invalid input. Please enter a valid integer." << endl;
        }
      }
    } else if (command == "C" || command == "c") {
      caesarEncryptCommand();
    } else if (command == "D" || command == "d") {
      ifstream dict_file("dictionary.txt");
      if (!dict_file) {
        cout << "Error: dictionary.txt not found!" << endl;
        continue;
      }
      vector<string> dict;
      string word;
      while (dict_file >> word) {
        dict.push_back(word);
      }
      caesarDecryptCommand(dict);
    } else if (command == "E" || command == "e") {
      ifstream quadgram_file("english_quadgrams.txt");
      if (!quadgram_file) {
        cout << "Error: english_quadgrams.txt not found!" << endl;
        continue;
      }
      vector<string> quadgrams;
      vector<int> counts;
      string line;
      while (getline(quadgram_file, line)) {
        size_t comma_pos = line.find(',');
        if (comma_pos != string::npos) {
          quadgrams.push_back(line.substr(0, comma_pos));
          try {
            counts.push_back(stoi(line.substr(comma_pos + 1)));
          } catch (const invalid_argument&) {
            counts.push_back(0);
          }
        }
      }
      QuadgramScorer scorer(quadgrams, counts);
      computeEnglishnessCommand(scorer);
    } else if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    } else if (command == "S" || command == "s" || command == "F" ||
               command == "f") {
      ifstream quadgram_file("english_quadgrams.txt");
      if (!quadgram_file) {
        cout << "Error: english_quadgrams.txt not found!" << endl;
        continue;
      }
      vector<string> quadgrams;
      vector<int> counts;
      string line;
      while (getline(quadgram_file, line)) {
        size_t comma_pos = line.find(',');
        if (comma_pos != string::npos) {
          quadgrams.push_back(line.substr(0, comma_pos));
          try {
            counts.push_back(stoi(line.substr(comma_pos + 1)));
          } catch (const invalid_argument&) {
            counts.push_back(0);
          }
        }
      }
      QuadgramScorer scorer(quadgrams, counts);
      // For both console mode (S) and file mode (F) use the combined function.
      decryptSubstCipherCommand(scorer);
    } else {
      cout << "Invalid command. Please try again." << endl;
    }
    cout << endl;
  }
  return 0;
}

void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "F - Decrypt Substitution Cipher from File" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

#pragma region CaesarEnc

char rot(char c, int amount) {
  if (isalpha(c)) {
    char base = 'A';
    return (char)((((toupper(c) - base) + amount) % 26 + 26) % 26 + base);
  }
  return c;
}

string rot(const string& line, int amount) {
  stringstream ss;
  for (char c : line) {
    ss << rot(c, amount);
  }
  return ss.str();
}

void caesarEncryptCommand() {
  cout << "Enter text to encrypt: ";
  string input;
  getline(cin, input);

  cout << "Enter the shift amount: ";
  int amount;
  cin >> amount;
  cin.ignore(numeric_limits<streamsize>::max(), '\n');

  stringstream ss;
  for (char c : input) {
    if (isalpha(c))
      ss << rot(c, amount);
    else if (isspace(c))
      ss << ' ';
  }
  string encrypted = ss.str();
  while (!encrypted.empty() && encrypted.back() == ' ') encrypted.pop_back();
  cout << encrypted << endl;
}

#pragma endregion CaesarEnc

#pragma region CaesarDec

void rot(vector<string>& strings, int amount) {
  for (string& s : strings) {
    s = rot(s, amount);
  }
}

string clean(const string& s) {
  string result;
  for (char c : s) {
    if (isalpha(c)) result += toupper(c);
  }
  return result;
}

vector<string> splitBySpaces(const string& s) {
  vector<string> words;
  istringstream iss(s);
  string word;
  while (iss >> word) {
    words.push_back(word);
  }
  return words;
}

string joinWithSpaces(const vector<string>& words) {
  string result;
  for (size_t i = 0; i < words.size(); i++) {
    if (i > 0) result += " ";
    result += words[i];
  }
  return result;
}

int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  int count = 0;
  for (const string& word : words) {
    if (find(dict.begin(), dict.end(), word) != dict.end()) count++;
  }
  return count;
}

void caesarDecryptCommand(const vector<string>& dict) {
  cout << "Enter text to decrypt: ";
  string text;
  getline(cin, text);

  vector<string> words = splitBySpaces(text);
  bool foundAnyDecryption = false;

  for (int shift = 0; shift < 26; shift++) {
    vector<string> rotated = words;
    rot(rotated, shift);
    int countMatches = numWordsIn(rotated, dict);
    if (countMatches > (int)rotated.size() / 2) {
      cout << joinWithSpaces(rotated) << endl;
      foundAnyDecryption = true;
    }
  }
  if (!foundAnyDecryption) {
    cout << "No good decryptions found" << endl;
  }
}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  string result;
  for (char c : s) {
    if (isalpha(c))
      result += cipher[ALPHABET.find(toupper(c))];
    else
      result += c;
  }
  return result;
}

void applyRandSubstCipherCommand() {
  vector<char> cipher = genRandomSubstCipher();
  cout << "Enter text to encrypt: ";
  string text;
  getline(cin, text);
  cout << "Encrypted text: " << applySubstCipher(cipher, text) << endl;
}

#pragma endregion SubstEnc

#pragma region SubstDec
// ----- Substitution Decryption Functions -----

// Scores a string based on English quadgrams
double scoreString(const QuadgramScorer& scorer, const string& s) {
  double totalScore = 0.0;
  string cleaned = clean(s);  // Clean the string to remove non-letters
  for (size_t i = 0; i + 3 < cleaned.size(); ++i) {
    string quadgram = cleaned.substr(i, 4);   // Extract quadgram
    totalScore += scorer.getScore(quadgram);  // Add quadgram score
  }
  return totalScore;
}

// Command to compute and display the English-ness score of a given string
void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  string text;
  cout << "Enter a string to score: ";
  getline(cin, text);
  double score = scoreString(scorer, text);
  cout << "English-ness score: " << score << endl;
}

// Hill-climbing algorithm to decrypt a substitution cipher
string hillClimbDecrypt(const string& ciphertext,
                        const QuadgramScorer& scorer) {
  vector<char> currentCipher = genRandomSubstCipher();
  string bestDecryption = applySubstCipher(currentCipher, ciphertext);
  double bestScore = scoreString(scorer, bestDecryption);

  int noImprovementCount = 0;
  const int maxNoImprovement = 500;  // Reduced threshold for faster termination

  while (noImprovementCount < maxNoImprovement) {
    vector<char> newCipher = currentCipher;
    int index1 = Random::randInt(25);
    int index2;
    do {
      index2 = Random::randInt(25);
    } while (index1 == index2);
    swap(newCipher[index1], newCipher[index2]);
    string newDecryption = applySubstCipher(newCipher, ciphertext);
    double newScore = scoreString(scorer, newDecryption);
    if (newScore > bestScore) {
      currentCipher = newCipher;
      bestScore = newScore;
      bestDecryption = newDecryption;
      noImprovementCount = 0;
    } else {
      noImprovementCount++;
    }
  }
  return bestDecryption;
}

// Combined command to decrypt substitution cipher from console or file
void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  cout << "Enter input filename (or leave blank to input text): ";
  string userInput;
  getline(cin, userInput);
  if (!userInput.empty()) {
    ifstream fin(userInput);
    if (fin.good()) {
      // File mode: read ciphertext from file, then prompt for output filename.
      stringstream buffer;
      buffer << fin.rdbuf();
      string ciphertext = buffer.str();
      fin.close();
      cout << "Enter output filename: ";
      string outputFilename;
      getline(cin, outputFilename);
      string bestDecryption;
      double bestScore = -1e9;
      for (int i = 0; i < 10; ++i) {  // Reduced iterations for speed
        string decryption = hillClimbDecrypt(ciphertext, scorer);
        double score = scoreString(scorer, decryption);
        if (score > bestScore) {
          bestScore = score;
          bestDecryption = decryption;
        }
      }
      ofstream fout(outputFilename, ios::out);
      if (!fout.is_open()) {
        cout << "Couldn't open output file given to command" << endl;
        return;
      }
      fout << bestDecryption;
      fout.close();
      cout << "Decryption complete. Output written to " << outputFilename
           << endl;
    } else {
      // If file can't be opened, treat userInput as ciphertext.
      string bestDecryption;
      double bestScore = -1e9;
      for (int i = 0; i < 10; ++i) {
        string decryption = hillClimbDecrypt(userInput, scorer);
        double score = scoreString(scorer, decryption);
        if (score > bestScore) {
          bestScore = score;
          bestDecryption = decryption;
        }
      }
      cout << "Best decryption: " << bestDecryption << endl;
    }
  } else {
    cout << "Enter text to decrypt: ";
    string ciphertext;
    getline(cin, ciphertext);
    string bestDecryption;
    double bestScore = -1e9;
    for (int i = 0; i < 10; ++i) {
      string decryption = hillClimbDecrypt(ciphertext, scorer);
      double score = scoreString(scorer, decryption);
      if (score > bestScore) {
        bestScore = score;
        bestDecryption = decryption;
      }
    }
    cout << "Best decryption: " << bestDecryption << endl;
  }
}

#pragma endregion SubstDec

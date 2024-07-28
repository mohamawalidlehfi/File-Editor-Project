#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

string recursiveReplace(string word, const string& oldWord, const string& newWord, int& charCount, int& wordCount) {
    if (word.empty()) {
        return "";
    }

    if (word.find(oldWord) == 0) {
        wordCount++;
        charCount += oldWord.length();
        return newWord + recursiveReplace(word.substr(oldWord.size()), oldWord, newWord, charCount, wordCount);
    }

    if (word[0] == oldWord[0]) {
        charCount++;
        word[0] = newWord[0];
    }

    return word[0] + recursiveReplace(word.substr(1), oldWord, newWord, charCount, wordCount);
}

void processText(string& text, const string& oldWord, const string& newWord, int& charCount, int& wordCount) {
    stringstream textStream(text);
    string word;
    string processedText;

    while (textStream >> word) {
        word = recursiveReplace(word, oldWord, newWord, charCount, wordCount);
        processedText += word + " ";
    }

    text = processedText;
}

int main() {
    ifstream file("input.txt");
    if (!file) {
        cerr << "Unable to open file!\n";
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    file.close();

    int charCount = 0;
    int wordCount = 0;

    auto start = high_resolution_clock::now();
    processText(content, "Lorem", "walid", charCount, wordCount);
    auto end = high_resolution_clock::now();

    ofstream outFile("Output_Recursion.txt");
    outFile << content;
    outFile.close();


    cout << "Geçen süre: " << duration_cast<seconds>(end - start).count() << " saniyeler\n";
    cout << "Karakterler değişti: " << charCount << "\n";
    cout << "Kelimeler değişti: " << wordCount << "\n";

    return 0;
}

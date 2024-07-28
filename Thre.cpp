#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
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

void processChunk(string& chunk, const string& oldWord, const string& newWord, int& charCount, int& wordCount) {
    stringstream chunkStream(chunk);
    string word;
    string processedChunk;

    while (chunkStream >> word) {
        word = recursiveReplace(word, oldWord, newWord, charCount, wordCount);
        processedChunk += word + " ";
    }

    chunk = processedChunk;
}

int main() {
    ifstream file("input.txt");
    if (!file) {
        cerr << "Dosya açılamıyor!\n";
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    file.close();

    unsigned int numThreads = thread::hardware_concurrency();
    vector<thread> threads;
    vector<int> charCounts(numThreads, 0);
    vector<int> wordCounts(numThreads, 0);
    vector<string> chunks(numThreads);
    unsigned int chunkSize = content.size() / numThreads;

    for (unsigned int i = 0; i < numThreads; ++i) {
        chunks[i] = content.substr(i * chunkSize, chunkSize);
    }

    auto start = high_resolution_clock::now();

    for (unsigned int i = 0; i < numThreads; ++i) {
        threads.emplace_back(processChunk, ref(chunks[i]), "Lorem", "walid", ref(charCounts[i]), ref(wordCounts[i]));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = high_resolution_clock::now();

    string processedContent;
    int toplamKarakter = 0;
    int toplamKelime = 0;
    for (unsigned int i = 0; i < numThreads; ++i) {
        processedContent += chunks[i];
        toplamKarakter += charCounts[i];
        toplamKelime += wordCounts[i];
    }

    ofstream outFile("Output_Threads.txt");
    outFile << processedContent;
    outFile.close();

    cout << "Geçen süre: " << duration_cast<seconds>(end - start).count() << " saniyeler\n";
    cout << "Karakterler değişti: " << toplamKarakter << "\n";
    cout << "Kelimeler değişti: " << toplamKelime << "\n";
    return 0;
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;
using namespace std::chrono;

mutex outputMutex;
string recursiveReplace(string Word, const string& oldWord, const string& newWord, int& charCount, int& WordCount) {
    if (Word.empty()) {
        return "";
    }
    if (Word.find(oldWord) == 0) {
        WordCount++;
        charCount += oldWord.length();
        return newWord + recursiveReplace(Word.substr(oldWord.size()), oldWord, newWord, charCount, WordCount);
    }
    if (Word[0] == oldWord[0]) {
        charCount++;
        Word[0] = newWord[0];
    }
    return Word[0] + recursiveReplace(Word.substr(1), oldWord, newWord, charCount, WordCount);
}

void processChunk(string& chunk, const string& oldWord, const string& newWord, int& charCount, int& WordCount) {
    stringstream chunkStream(chunk);
    string Word;
    string processedChunk;

    while (chunkStream >> Word) {
        Word = recursiveReplace(Word, oldWord, newWord, charCount, WordCount);
        processedChunk += Word + " ";
    }
    chunk = processedChunk;
}
void threadFunction(vector<string>& chunks, int index, const string& oldWord, const string& newWord, int& charCount, int& WordCount) {
    processChunk(chunks[index], oldWord, newWord, charCount, WordCount);
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
    vector<int> WordCounts(numThreads, 0);
    vector<string> chunks(numThreads);
    unsigned int chunkSize = content.size() / numThreads;

    for (unsigned int i = 0; i < numThreads; ++i) {
        chunks[i] = content.substr(i * chunkSize, chunkSize);
    }
    auto start = high_resolution_clock::now();

    for (unsigned int i = 0; i < numThreads; ++i) {
        threads.emplace_back(threadFunction, ref(chunks), i, "Lorem", "walid", ref(charCounts[i]), ref(WordCounts[i]));
    }

    for (auto& t : threads) {
        t.join();
    }

    ofstream finalOutFile("Output_ParallelAndVactor.txt");
    int toplamKarakter = 0;
    int toplamKelime = 0;
    for (unsigned int i = 0; i < numThreads; ++i) {
        finalOutFile << chunks[i];
        toplamKarakter += charCounts[i];
        toplamKelime += WordCounts[i];
    }
    finalOutFile.close();

    auto end = high_resolution_clock::now();

    cout << "Geçen süre: " << duration_cast<seconds>(end - start).count() << " saniyeler\n";
    cout << "Karakterler değişti: " << toplamKarakter << "\n";
    cout << "Kelimeler değişti: " << toplamKelime << "\n";

    return 0;
}

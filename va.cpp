#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <immintrin.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

void vectorizedReplace(string& chunk, int& charCount) {
    __m256i oldChars = _mm256_set1_epi8('a');
    __m256i newChars = _mm256_set1_epi8('b');
    size_t chunkSize = chunk.size();

    for (size_t i = 0; i + 32 <= chunkSize; i += 32) {
        __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&chunk[i]));
        __m256i mask = _mm256_cmpeq_epi8(data, oldChars);
        __m256i result = _mm256_blendv_epi8(data, newChars, mask);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(&chunk[i]), result);
    }

    for (size_t i = chunkSize - chunkSize % 32; i < chunkSize; ++i) {
        if (chunk[i] == 'a') {
            chunk[i] = 'b';
            charCount++;
        }
    }
}

string recursiveReplace(string word, const string& oldWord, const string& newWord, int& wordCount) {
    if (word.empty()) {
        return "";
    }

    if (word.find(oldWord) == 0) {
        wordCount++;
        return newWord + recursiveReplace(word.substr(oldWord.size()), oldWord, newWord, wordCount);
    }

    return word[0] + recursiveReplace(word.substr(1), oldWord, newWord, wordCount);
}

void processChunk(string& chunk, const string& oldWord, const string& newWord, int& charCount, int& wordCount) {
    vectorizedReplace(chunk, charCount);
    stringstream chunkStream(chunk);
    string word;
    string processedChunk;

    while (chunkStream >> word) {
        word = recursiveReplace(word, oldWord, newWord, wordCount);
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
        threads.emplace_back(processChunk, ref(chunks[i]), "Lorem", "Walid", ref(charCounts[i]), ref(wordCounts[i]));
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

    ofstream outFile("Output_VectorizedAndThreads.txt");
    outFile << processedContent;
    outFile.close();

    cout << "Geçen süre: " << duration_cast<seconds>(end - start).count() << " saniyeler\n";
    cout << "Karakterler değişti: " << toplamKarakter << "\n";
    cout << "Kelimeler değişti: " << toplamKelime << "\n";
    return 0;
}

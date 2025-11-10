#include <iostream>
#include <vector>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <chrono>

const int MIN_VALUE = 0;
const int MAX_VALUE = 2;
const int VECTOR_SIZE = 1000000;
const int PRECISION = 12;

void generateRandomData(std::vector<double>& data) {
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = MIN_VALUE + (MAX_VALUE - MIN_VALUE) * (rand() / static_cast<double>(RAND_MAX));
    }
}


void partialSum(const std::vector<double>& data, size_t start, size_t end, double& result) {
    result = 0;
    for (size_t i = start; i < end; ++i) {
        result += data[i];
    }
}

int main() {
    std::cout.precision(PRECISION);
    srand(static_cast<unsigned>(time(nullptr)));

    unsigned int numThreads = std::thread::hardware_concurrency();
    std::cout << "Detected threads: " << numThreads << std::endl;

    std::vector<double> values(VECTOR_SIZE);
    generateRandomData(values);

    
    auto t1_start = std::chrono::high_resolution_clock::now();
    double singleSum = 0;
    for (double val : values) {
        singleSum += val;
    }
    auto t1_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> singleTime = t1_end - t1_start;

    std::cout << "Single-threaded sum: " << singleSum << std::endl;
    std::cout << "Single-threaded time: " << singleTime.count() << " seconds" << std::endl;

   
    std::vector<std::thread> threadPool;
    std::vector<double> threadResults(numThreads, 0);
    size_t blockSize = VECTOR_SIZE / numThreads;

    auto t2_start = std::chrono::high_resolution_clock::now();
    for (unsigned int i = 0; i < numThreads; ++i) {
        size_t startIdx = i * blockSize;
        size_t endIdx = (i == numThreads - 1) ? VECTOR_SIZE : startIdx + blockSize;

        threadPool.emplace_back(partialSum, std::cref(values), startIdx, endIdx, std::ref(threadResults[i]));
    }

    for (auto& th : threadPool) {
        th.join();
    }

    double multiSum = 0;
    for (double part : threadResults) {
        multiSum += part;
    }
    auto t2_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> multiTime = t2_end - t2_start;

    std::cout << "Multi-threaded sum: " << multiSum << std::endl;
    std::cout << "Multi-threaded time: " << multiTime.count() << " seconds" << std::endl;

    return 0;
}

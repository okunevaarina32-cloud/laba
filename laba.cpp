#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <chrono>

using namespace std;
using namespace std::chrono;

enum class DataType {
    RANDOM,
    SORTED,
    REVERSED
};

string typeToString(DataType type) {
    if (type == DataType::RANDOM) return "random";
    if (type == DataType::SORTED) return "sorted";
    return "reversed";
}

struct Metrics {
    long long comparisons = 0;
    long long swaps = 0;
    long long time_us = 0;
};

vector<int> generateArray(int n, DataType type) {
    vector<int> arr(n);
    
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 64001 - 32000;
    }
    
    if (type == DataType::SORTED) {
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (arr[j] > arr[j + 1]) {
                    int temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }
    }
    else if (type == DataType::REVERSED) {
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (arr[j] > arr[j + 1]) {
                    int temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }
        for (int i = 0; i < n / 2; i++) {
            int temp = arr[i];
            arr[i] = arr[n - 1 - i];
            arr[n - 1 - i] = temp;
        }
    }
    
    return arr;
}

Metrics bubbleClassic(vector<int> arr) {
    Metrics m;
    
    auto start = high_resolution_clock::now();
    
    int n = arr.size();
    
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            m.comparisons++;
            
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                m.swaps++;
            }
        }
    }
    
    auto end = high_resolution_clock::now();
    m.time_us = duration_cast<microseconds>(end - start).count();
    
    return m;
}

Metrics bubbleFlag(vector<int> arr) {
    Metrics m;
    
    auto start = high_resolution_clock::now();
    
    int n = arr.size();
    bool swapped;
    
    for (int i = 0; i < n - 1; i++) {
        swapped = false;
        
        for (int j = 0; j < n - i - 1; j++) {
            m.comparisons++;
            
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                m.swaps++;
                swapped = true;
            }
        }
        
        if (!swapped) break;
    }
    
    auto end = high_resolution_clock::now();
    m.time_us = duration_cast<microseconds>(end - start).count();
    
    return m;
}

Metrics bubbleBound(vector<int> arr) {
    Metrics m;
    
    auto start = high_resolution_clock::now();
    
    int n = arr.size();
    int lastSwap;
    
    do {
        lastSwap = 0;
        
        for (int i = 1; i < n; i++) {
            m.comparisons++;
            
            if (arr[i - 1] > arr[i]) {
                int temp = arr[i - 1];
                arr[i - 1] = arr[i];
                arr[i] = temp;
                m.swaps++;
                lastSwap = i;
            }
        }
        
        n = lastSwap;
        
    } while (n > 0);
    
    auto end = high_resolution_clock::now();
    m.time_us = duration_cast<microseconds>(end - start).count();
    
    return m;
}

int main() {
    srand(time(0));
    
    vector<int> sizes = {10, 100, 500, 1000, 2000, 5000, 10000};
    
    vector<DataType> types = {DataType::RANDOM, DataType::SORTED, DataType::REVERSED};
    
    ofstream file("results.csv");
    file << "Size;DataType;Algorithm;Comparisons;Swaps;Time_us\n";
    
    for (int n : sizes) {
        for (DataType type : types) {
            vector<int> original = generateArray(n, type);
            
            vector<int> arr1(n);
            vector<int> arr2(n);
            vector<int> arr3(n);
            
            memcpy(arr1.data(), original.data(), n * sizeof(int));
            memcpy(arr2.data(), original.data(), n * sizeof(int));
            memcpy(arr3.data(), original.data(), n * sizeof(int));
            
            Metrics m1 = bubbleClassic(arr1);
            Metrics m2 = bubbleFlag(arr2);
            Metrics m3 = bubbleBound(arr3);
            
            file << n << ";" << typeToString(type) << ";classic;"
                 << m1.comparisons << ";" << m1.swaps << ";" << m1.time_us << "\n";
            
            file << n << ";" << typeToString(type) << ";flag;"
                 << m2.comparisons << ";" << m2.swaps << ";" << m2.time_us << "\n";
            
            file << n << ";" << typeToString(type) << ";bound;"
                 << m3.comparisons << ";" << m3.swaps << ";" << m3.time_us << "\n";
        }
    }
    
    file.close();
    cout << "Done\n";
    return 0;
}
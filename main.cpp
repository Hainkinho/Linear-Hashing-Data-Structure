#include<iostream>
#include <vector>
#include "ADS_set.h"
#include <set>
#include <algorithm>

using namespace std;

int main() {
    vector<size_t> numbers;

    for (size_t i = 0; i < 100000; ++i) {
        numbers.push_back(i);
    }

    // ADS_set<size_t, 7> set1;
    // set1.insert(numbers.begin(), numbers.end());
    // set1.dump();

    // vector<string> words{"d-4711"};
    // int numOfNewValues = std::distance(words.end(), words.begin());
    // cout << "size: " << numOfNewValues << endl;
    ADS_set<size_t, 53> set2;
    set2.insert(numbers.begin(), numbers.end());
    ADS_set<size_t, 53> set3;
    set3.insert(set2.begin(), set2.end());
    ADS_set<size_t, 53> set4;
    set4 = set3;
    ADS_set<size_t, 53> set5{set4};

    // set2.clear();
    set3.clear();
    set3.insert({1,23,453,534,65,546,56,46,57,6,867,87});

    set2.dump();
    set3.dump();
    set4.dump();
    set5.dump();

    // set2.clear();
    // set2.insert({"d-4711"});
    // set2.dump();
    // cout << set2.size() << endl;

    

    return 0;
}
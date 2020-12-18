#include<iostream>
#include"ADS_set.h"

using namespace std;

class LinkedList {
    private:    
    static constexpr size_t N = 1;
    size_t sz = 0;
    size_t values[N];
    LinkedList* nextBucket = nullptr;

    bool isEqual(const size_t& a, const size_t& b) const {
        return a == b;
    }

    public:
    LinkedList() {}

    ~LinkedList() {
        sz=0;
        nextBucket = nullptr;
    }

    void add(const size_t& key) {
        if (sz == N) throw std::runtime_error("Linked List ist schon voll -> Wert konnte nicht eingefügt werden!");
        values[sz] = key;
        sz++;
    }

    void remove(const size_t& indexToRemove) {
        if (indexToRemove >= sz) return;
        for (size_t i = indexToRemove+1; i < sz; ++i) {
            values[i-1] = values[i];
        }
        sz--;
    }

    void erase(const size_t& key) {
        for (size_t i = 0; i < sz; ++i) {
            if (isEqual(key, values[i])) {
                remove(i);
                return;
            }
        }
        throw std::runtime_error("LinkedList::erase() hat keinen Wert zum löschen gefunden!");
    }

    bool find(const size_t& key) const {
        for (size_t i = 0; i < sz; i++) {
            if (isEqual(key, values[i])) return true;
        }
        return false;
    }

    size_t& getKey(size_t index) {
        return values[index];
    }

    size_t pop() {
        if (empty()) throw std::runtime_error("LinkedList::pop() geht nicht, da LinkedList leer ist!");
        sz--;
        return values[sz];
    }

    bool empty() const { return sz <= 0; }
    size_t size() const { return sz; }

    void setNextBucket(LinkedList* nextBucket) { this->nextBucket = nextBucket; }
    LinkedList* getNextBucket() const { return nextBucket; }

    vector<size_t> getValues() const {
        vector<size_t> res{};
        for (size_t i = 0; i < sz; i++) {
            res.push_back(values[i]);
        }
        return res;
    }

    void to_string() const {
        for (size_t i = 0; i < sz; i++) {
            cout << values[i] << " ";
        }
        cout << endl;
    }
};

int main() {

    LinkedList list;
    list.add(1);
    // list.add(2);
    // list.add(3);
    // list.add(4);
    
    list.to_string();

    list.remove(0);

    list.to_string();

    return 0;
}
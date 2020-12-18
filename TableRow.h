#ifndef TableRow_h
#define TableRow_h

#define DEBUG

#include<iostream>
#include<vector>

using std::vector;

template <typename Key, size_t N = 5>
class TableRow {
    using value_type = Key;
    using key_type = Key;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    // using const_iterator = /* constant ForwardIterator */
    // using iterator = const_iterator;
    using key_compare = std::less<key_type>;   // B+-Tree
    using key_equal = std::equal_to<key_type>; // Hashing
    using hasher = std::hash<key_type>;        // Hashing

    private:
    class LinkedList {

    private:    
        // friend class TableRow;
        size_type sz = 0;
        // key_type* values = nullptr;
        key_type values[N];
        LinkedList* nextBucket = nullptr;

        bool isEqual(const key_type& a, const key_type& b) const {
            return key_equal{}(a, b);
        }
        
        /* Verschiebt die nachvolgenden Elemente über das Element zu löchen und sz wird um eins weniger. */
        void remove(const size_type& indexToRemove) {
            for (size_type i = indexToRemove; i < sz-1; i++) {
                values[i] = values[i+1];
            }
            sz--;
        }

    public:
        LinkedList() {}

        ~LinkedList() {
            sz=0;
            nextBucket = nullptr;
        }

        void add(const key_type& key) {
            if (sz == N) throw std::runtime_error("Linked List ist schon voll -> Wert konnte nicht eingefügt werden!");
            values[sz] = key;
            sz++;
        }

        void erase(const key_type& key) {
            for (size_type i = 0; i < sz; i++) {
                if (isEqual(key, values[i])) {
                    remove(i);
                    return;
                }
            }
            
            throw std::runtime_error("LinkedList::erase() hat keinen Wert zum löschen gefunden!");
        }

        bool find(const key_type& key) const {
            for (size_type i = 0; i < sz; i++) {
                if (isEqual(key, values[i])) return true;
            }
            return false;
        }

        key_type getKey(size_type index) const {
            return values[index];
        }

        key_type pop() {
            if (empty()) throw std::runtime_error("LinkedList::pop() geht nicht, da LinkedList leer ist!");
            sz--;
            return values[sz];
        }

        bool empty() const { return sz <= 0; }
        size_type size() const { return sz; }

        void setNextBucket(LinkedList* nextBucket) { this->nextBucket = nextBucket; }
        LinkedList* getNextBucket() const { return nextBucket; }

        vector<key_type> getValues() const {
            vector<key_type> res{};
            for (size_type i = 0; i < sz; i++) {
                res.push_back(values[i]);
            }
            return res;
        }

    };

    // ? ======================================================================================

    size_type numberOfValues = 0;
    LinkedList* firstBucket = nullptr;
    LinkedList* lastBucket = nullptr;

    LinkedList* getLastBucket() { return lastBucket; }

    LinkedList* getBucketBevor(LinkedList* bucket) const {
        LinkedList* curBucket = firstBucket;
        while(curBucket != nullptr) { 
            LinkedList* nextBucket = curBucket->getNextBucket();
            if (nextBucket == bucket) {
                return curBucket;
            }
            curBucket = nextBucket;
        }
        return nullptr;
    }

    void reOrganize(LinkedList* startBucket) {
        if (numberOfValues <= 0) return;
        if (startBucket != lastBucket) {
            key_type keyToAdd = lastBucket->pop();
            startBucket->add(keyToAdd);
        }
        if (lastBucket->empty()) {
            LinkedList* newLastBucket = getBucketBevor(lastBucket);
            delete lastBucket;
            newLastBucket->setNextBucket(nullptr);
            lastBucket = newLastBucket;
        }
    }

public:
    TableRow() {
        this->firstBucket = new LinkedList();
        this->lastBucket = firstBucket;
    }

    TableRow(const TableRow& rhs) : TableRow() {
        for (auto bucketValues : rhs.getValues()) {
            for(auto key : bucketValues) {
                add(key);
            }
        }
    }

    TableRow& operator=(const TableRow& rhs){
        std::cout << "Copy Assignment" << std::endl;
        clear();
        for (auto bucketValues : rhs.getValues()) {
            for(auto key : bucketValues) {
                add(key);
            }
        }
        return *this;
    }

    ~TableRow() {
        clear();
        delete firstBucket;
        firstBucket = nullptr;
        lastBucket = nullptr;
    }

    bool add(const key_type& key) {
        bool overflow = false;
        if (lastBucket->size() < N) {
            lastBucket->add(key);
        } else {
            LinkedList* newBucket = new LinkedList();
            newBucket->add(key);
            lastBucket->setNextBucket(newBucket);
            this->lastBucket = newBucket;
            overflow = true;
        }
        numberOfValues++;
        return overflow;
    }

    bool find(const key_type& key) const {
        LinkedList* curBucket = firstBucket;
        while(curBucket != nullptr) {
            if (curBucket->find(key)) return true;
            curBucket = curBucket->getNextBucket();
        }
        return false;
    }

    bool erase(const key_type& key) {
        LinkedList* curBucket = firstBucket;
        while(curBucket != nullptr) { 
            if (curBucket->find(key)) {
                curBucket->erase(key);
                numberOfValues--;
                reOrganize(curBucket);
                return true;
            }
            curBucket = curBucket->getNextBucket();
        }
        return false;
    }

    vector<key_type> clear() {
        vector<key_type> res;
        LinkedList* curBucket = firstBucket;
        while(curBucket != nullptr) {
            for (const key_type& key : curBucket->getValues()) {
                res.push_back(key);
            }
            LinkedList* next = curBucket->getNextBucket();
            delete curBucket;
            curBucket = next;
        }
        firstBucket = new LinkedList();
        lastBucket = firstBucket;
        numberOfValues = 0;
        return res;
    }

    size_type getNumberOfValues() const { return numberOfValues; }

    vector<vector<key_type>> getValues() const {
        vector<vector<key_type>> res;
        LinkedList* curBucket = firstBucket;
        while(curBucket != nullptr) {
            res.push_back(curBucket->getValues());
            curBucket = curBucket->getNextBucket();
        }

        return res;
    }

    #ifdef DEBUG

        void dump(std::ostream& o = std::cerr) const {
            vector<vector<key_type>> valuesVec = getValues();

            for (const auto& values : valuesVec) {
                for (size_type i = 0; i < N; i++) {
                    if (i < values.size()) {
                        o << values[i];
                    } else {
                        o << "#";
                    }
                    
                    if (i < N-1) o << ", ";
                }
                o << " -> ";
            }
            o <<" ("<<valuesVec.size()<<" Blöcke)"<< "\n";
        }

    #endif

};


#endif
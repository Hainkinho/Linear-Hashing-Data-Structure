#ifndef ADS_SET_H
#define ADS_SET_H

#include<iostream>
#include<vector>

// using std::vector;

template <typename Key, size_t N = 7>  // N = Bucket Size
class ADS_set {
public:
    class Iterator;
    using value_type = Key;
    using key_type = Key;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using const_iterator = Iterator;
    using iterator = const_iterator;
    using key_compare = std::less<key_type>;   // B+-Tree
    using key_equal = std::equal_to<key_type>; // Hashing
    using hasher = std::hash<key_type>;        // Hashing

    private:
    class TableRow {
        private:
        class LinkedList {
        private:    
            size_type sz = 0;
            key_type values[N];
            LinkedList* nextBucket = nullptr;
            
            /* Verschiebt die nachvolgenden Elemente über das Element zu löschen und sz wird um eins weniger. */
            void remove(const size_type& indexToRemove) {
                if (indexToRemove >= sz) return;
                for (size_t i = indexToRemove+1; i < sz; ++i) {
                    values[i-1] = values[i];
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
                for (size_type i = 0; i < sz; ++i) {
                    if (key_equal{}(key, values[i])) {
                        remove(i);
                        return;
                    }
                }
                throw std::runtime_error("LinkedList::erase() hat keinen Wert zum löschen gefunden!");
            }

            key_type pop() {
                if (empty()) throw std::runtime_error("LinkedList::pop() geht nicht, da LinkedList leer ist!");
                sz--;
                return values[sz];
            }

            void setNextBucket(LinkedList* nextBucket) { this->nextBucket = nextBucket; }


            // GETTERS - LinkedList
            key_type& getKey(size_type index) { return values[index]; }
            bool empty() const { return sz <= 0; }
            size_type size() const { return sz; }
            LinkedList* getNextBucket() const { return nextBucket; }

            bool find(const key_type& key) const {
                for (size_type i = 0; i < sz; i++) {
                    if (key_equal{}(key, values[i])) return true;
                }
                return false;
            }

            std::vector<key_type> getValues() const {
                std::vector<key_type> res{};
                for (size_type i = 0; i < sz; i++) {
                    res.push_back(values[i]);
                }
                return res;
            }
        };






























        // ? class TableRow
        // * Es wird bei der Initialisierung kein Bucket initialisiert! Wenn du einen brauchst, dann musst du initFirstBucket() aufrufen!
        private:
        size_type numberOfValues = 0;
        LinkedList* firstBucket = nullptr;
        LinkedList* lastBucket = nullptr;

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
        TableRow() {}

        TableRow(const TableRow& rhs) : TableRow() {
            for (auto bucketValues : rhs.getValues()) {
                for(auto key : bucketValues) {
                    add(key);
                }
            }
        }

        TableRow& operator=(const TableRow& rhs){
            clear();
            initFirstBucket();
            for (auto bucketValues : rhs.getValues()) {
                for(auto key : bucketValues) {
                    add(key);
                }
            }
            return *this;
        }

        ~TableRow() {
            if (firstBucket != nullptr) {
                clear();
            }
        }

        void initFirstBucket() {
            firstBucket = new LinkedList();
            lastBucket = firstBucket;
        }

        key_type& at(const size_type index) {
            if (index >= numberOfValues) throw std::runtime_error("at(): Kein Wert gefunden! (index >= numberOfValues)");

            size_type curIndex = 0;
            LinkedList* curBucket = firstBucket;
            while(curBucket != nullptr) {
                if (index < curIndex + N) {
                    return curBucket->getKey(index % N);
                } else {
                    curIndex += N;
                    curBucket = curBucket->getNextBucket();
                }
            }
            
            throw std::runtime_error("at(): Kein Wert gefunden!");
        }

        bool add(const key_type& key) {
            bool overflow = false;
            if (!isFull()) {
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

        void clear() {
            LinkedList* curBucket = firstBucket;
            while(curBucket != nullptr) {
                LinkedList* next = curBucket->getNextBucket();
                delete curBucket;
                curBucket = next;
            }
            firstBucket = nullptr;
            lastBucket = nullptr;
            numberOfValues = 0;
        }

        void clear(std::vector<key_type>& vec) {
            vec.reserve(this->numberOfValues);
            LinkedList* curBucket = firstBucket;
            while(curBucket != nullptr) {
                for (const key_type& key : curBucket->getValues()) {
                    vec.push_back(key);
                }
                LinkedList* next = curBucket->getNextBucket();
                delete curBucket;
                curBucket = next;
            }
            firstBucket = nullptr;
            lastBucket = nullptr;
            numberOfValues = 0;
        }

        void rewire(TableRow& rhs) {
            if (this == &rhs) { return; }  //Prevent Rewire Same 
            this->firstBucket = rhs.firstBucket;
            this->lastBucket = rhs.lastBucket;
            this->numberOfValues = rhs.numberOfValues;
            rhs.firstBucket = nullptr;
            rhs.lastBucket = nullptr;
        }


        // GETTERS - TableRow
        size_type size() const { return numberOfValues; }
        bool hasBucket() const { return firstBucket != nullptr; }
        bool isFull() const { return lastBucket->size() >= N; }
        bool empty() const { return this->numberOfValues <= 0; }
        size_type getNumberOfValues() const { return numberOfValues; }

        bool find(const key_type& key) const {
            LinkedList* curBucket = firstBucket;
            while(curBucket != nullptr) {
                if (curBucket->find(key)) return true;
                curBucket = curBucket->getNextBucket();
            }
            return false;
        }

        std::vector<std::vector<key_type>> getValues() const {
            std::vector<std::vector<key_type>> res;
            LinkedList* curBucket = firstBucket;
            while(curBucket != nullptr) {
                res.push_back(curBucket->getValues());
                curBucket = curBucket->getNextBucket();
            }
            return res;
        }
    };






























    // ? class ADS_set
    // * WICHTIG: Wenn du einen neuen Wert einfügst, dann musst du es in ADS_set& operator=(const ADS_set& other) UND auch in void swap(ADS_set& other) einfügen!

    TableRow* tableRows = nullptr;
    size_type numberOfRows = 0;
    size_type maxNumberOfRows = 0;
    size_type numberOfValues = 0;
    size_type nextToSplit = 0;
    size_type round = 0;

    size_type pow2(size_type exponent) const { return 1 << exponent; }

    /* Returned die minimale Anzahl and tableRows, welche auch in jeder neuen Runde ist. ( 2^round ) */
    size_type getMinRowSize() const { return pow2(round); }

    /* Returned die maximale Anzahl and tableRows, welche auch am Ende jeder Runde ist. ( 2^round+1 ) */
    size_type getMaxRowSize() const { return pow2(round+1); }

    
    size_type h(const key_type& key) const { return hasher{}(key) % getMinRowSize(); }
    size_type g(const key_type& key) const { return hasher{}(key) % getMaxRowSize(); }
    size_type getTableRowIndex(const key_type& key) const {
        size_type rowIndex = h(key);
        if (rowIndex < nextToSplit) {
            return g(key);
        }
        return rowIndex;
    }

    size_type lastIndexToSplit() const { return getMinRowSize() - 1; }

    void insertWith(const key_type& key) {
        size_type tableRowIndex = getTableRowIndex(key);
        if (!tableRows[tableRowIndex].find(key)) {
            if (tableRows[tableRowIndex].isFull()) {
                handleOverflow();
                tableRowIndex = getTableRowIndex(key);
            }
            tableRows[tableRowIndex].add(key);
            this->numberOfValues += 1;
        }
    }

    void handleOverflow() {
        std::vector<key_type> keys;
        tableRows[nextToSplit].clear(keys);
        tableRows[nextToSplit].initFirstBucket();
        extendTable();
        if (nextToSplit == lastIndexToSplit()) {
            nextToSplit = 0;
            round++;
        } else {
            nextToSplit++;
        }
        for (const key_type& key : keys) {
            rehash(key);
        }
    }

    void rehash(const key_type& key) {
        size_type tableRowIndex = getTableRowIndex(key);
        tableRows[tableRowIndex].add(key);
    }

    // Essentielle funktion bezüglich performance und Speicherverbrauch!
    void extendTable() {
        if ((numberOfRows+1) > maxNumberOfRows) {
            if (round < 13) {
                reserve(numberOfRows + pow2(round+1));
            } else {
                reserve(numberOfRows + 30'000);
            }
        }
        tableRows[numberOfRows].initFirstBucket();
        numberOfRows++;
    }

    void reserve(size_t newNumberOfRows) {
        if (newNumberOfRows < maxNumberOfRows) return;
        maxNumberOfRows = newNumberOfRows;
        TableRow* newTableRows = new TableRow[maxNumberOfRows];
        for (size_type i = 0; i < numberOfRows; i++) {
            newTableRows[i].rewire(tableRows[i]);
        }

        delete[] tableRows;
        tableRows = nullptr;
        tableRows = newTableRows;
    }

    std::vector<key_type> getValuesAt(const key_type& key) const {
        size_type rowIndex = getTableRowIndex(key);
        std::vector<std::vector<key_type>> valuesVec = getValuesAtIndex(rowIndex);
        std::vector<key_type> res;
        for (const auto& vals : valuesVec) {
            for (const auto& val : vals) {
                res.push_back(val);
            }
        }
        return res;
    }

    std::vector<std::vector<key_type>> getValuesAtIndex(const size_type& rowIndex) const {
        return tableRows[rowIndex].getValues();
    }


public:
    ADS_set() {
        this->tableRows = new TableRow[1];
        this->tableRows[0].initFirstBucket();
        this->numberOfRows = 1;
        this->maxNumberOfRows = 1;
        this->numberOfValues = 0;
        this->nextToSplit = 0;
        this->round = 0;
    }

    ADS_set(std::initializer_list<key_type> ilist) : ADS_set() { 
        insert(ilist.begin(), ilist.end()); 
    }

    template<typename InputIt> 
    ADS_set(InputIt first, InputIt last) : ADS_set() { insert(first, last); }

    ADS_set(const ADS_set& other): ADS_set() {
        *this = other;
    }

    ~ADS_set() {
        delete[] tableRows;
    }

    ADS_set& operator=(const ADS_set& other) {
        clear();
        delete[] this->tableRows;
        this->numberOfRows = other.numberOfRows;
        this->maxNumberOfRows = other.maxNumberOfRows;
        this->numberOfValues = other.numberOfValues;
        this->nextToSplit = other.nextToSplit;
        this->round = other.round;

        this->tableRows = new TableRow[maxNumberOfRows];
        for (size_type i = 0; i < numberOfRows; ++i) {
            this->tableRows[i].initFirstBucket();
            this->tableRows[i] = other.tableRows[i];
        }
        return *this;
    }
    ADS_set& operator=(std::initializer_list<key_type> ilist) {
        *this = ADS_set();
        insert(ilist);
        return *this;
    }

    void insert(std::initializer_list<key_type> ilist) {
        size_type newNumberOfRows = (numberOfValues + ilist.size()) / (N);
        reserve(newNumberOfRows);
        insert(ilist.begin(), ilist.end());
    }
    template<typename InputIt> 
    void insert(InputIt first, InputIt last) {
        for (auto it = first; it != last; ++it) {
            insertWith(*it);
        }
    }

    std::pair<iterator,bool> insert(const key_type& key) {
        size_type storeNumberOfValues = numberOfValues;
        insertWith(key);
        bool addedValue = numberOfValues > storeNumberOfValues;
        return std::pair<iterator,bool>(find(key), addedValue);
    }

    size_type erase(const key_type& key) {
        size_type tableIndex = getTableRowIndex(key);
        if (tableRows[tableIndex].erase(key) == 0) {
            return 0;
        } else {
            numberOfValues--;
            return 1;
        }
    }

    /* Achtung: Setzt das ADS_set wieder auf den Werkszustand. Das bedeutet, dass noch immer eine TableRow im Speicher alloziert ist! */
    void clear() {
        ADS_set<Key, N> tmp;
        swap(tmp);
    }

    void swap(ADS_set& other) {
        using std::swap;
        swap(tableRows, other.tableRows);
        swap(numberOfRows, other.numberOfRows);
        swap(maxNumberOfRows, other.maxNumberOfRows);
        swap(numberOfValues, other.numberOfValues);
        swap(nextToSplit, other.nextToSplit);
        swap(round, other.round);
    }


    // GETTERS - ADS_set
    size_type size() const { return numberOfValues; }
    bool empty() const { return size() == 0; }

    iterator find(const key_type& key) const {
        size_type rowIndex = getTableRowIndex(key);
        for (size_type curIndex = 0; curIndex < tableRows[rowIndex].size(); ++curIndex) {
            if (key_equal{}(tableRows[rowIndex].at(curIndex), key)) {
                return Iterator(tableRows+rowIndex, tableRows+numberOfRows, curIndex);
            }
        }
        return end();
    }

    /* Rückgabewert: Die Anzahl der Elemente im Container mit Schlüssel key (0 oder 1).*/
    size_type count(const key_type& key) const {
        size_type tableIndex = getTableRowIndex(key);
        return tableRows[tableIndex].find(key);
    }

    void dump(std::ostream& o = std::cerr) const {
        o << "================" << "\n";
        for (size_type rowIndex = 0; rowIndex < numberOfRows; rowIndex++) {
            if (nextToSplit == rowIndex) o << "->";
            else o << "  ";
            o << rowIndex << ":  ";

            std::vector<std::vector<key_type>> valuesVec = getValuesAtIndex(rowIndex);
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
            o <<" ("<<valuesVec.size()<<" Blöcke, " << tableRows[rowIndex].size() << " values)\n";
            if (rowIndex == lastIndexToSplit()) o << "--------lastIndexToSplit--------" << "\n";
        }
        o << "================" << "\n";
        o << "bucketSize = " << N << "\n";
        o << "round = " << round << "\n";
        o << "nextToSplit = " << nextToSplit << "\n";
        o << "lastIndexToSplit = " << lastIndexToSplit() << "\n";
        o << "numberOfValues: "<< numberOfValues << "\n";
        o << "numberOfRows: " << numberOfRows << "\n";
        o << "maxNumberOfRows: " << maxNumberOfRows << "\n";
    }

    
    // ITERATORS
    const_iterator begin() const {
        if (empty()) return end();
        return Iterator(tableRows, tableRows+numberOfRows);
    }
    
    const_iterator end() const {
        return Iterator(tableRows+numberOfRows, tableRows+numberOfRows);
    }
};





























template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {
public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::forward_iterator_tag;
    private:
    TableRow* rowPtr;
    TableRow* endRowPtr;
    size_type curIndex = 0;

    void nextRow() {
        ++rowPtr;
        if (rowPtr != endRowPtr) {
            if (rowPtr->empty()) nextRow();
        }
    }

    public:
    explicit Iterator(TableRow* rowPtr = nullptr, TableRow* endRowPtr = nullptr, size_type curIndex = 0) {
        this->rowPtr = rowPtr;
        this->endRowPtr = endRowPtr;
        this->curIndex = curIndex;

        if (rowPtr != endRowPtr && rowPtr->empty()) {
            nextRow();
        }
    }
    reference operator*() const { return rowPtr->at(curIndex); }
    pointer operator->() const { return &rowPtr->at(curIndex); }
    Iterator &operator++() {
        ++curIndex;
        if (curIndex >= rowPtr->size()) {
            curIndex = 0;
            nextRow();
        }
        return *this;
    }
    Iterator operator++(int) {
        Iterator copy = *this;
        operator++();
        return copy;
    }
    friend bool operator==(const Iterator &lhs, const Iterator &rhs) {
        return (lhs.rowPtr == rhs.rowPtr && lhs.curIndex == rhs.curIndex);
    }
    friend bool operator!=(const Iterator &lhs, const Iterator &rhs) {
        return !(lhs == rhs);
    }
};


template <typename Key, size_t N>
bool operator==(const ADS_set<Key,N>& lhs, const ADS_set<Key,N>& rhs) {
    if (lhs.size() != rhs.size()) return false;

    for (auto it = lhs.begin(); it != lhs.end(); ++it) {
        if (rhs.find(*it) == rhs.end()) return false;
    }
    return true;
}

template <typename Key, size_t N>
bool operator!=(const ADS_set<Key,N>& lhs, const ADS_set<Key,N>& rhs) { return !(lhs == rhs); }

template <typename Key, size_t N> 
void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) { lhs.swap(rhs); }

#endif
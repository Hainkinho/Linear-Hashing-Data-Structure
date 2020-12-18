
#ifndef ADS_SET_H
#define ADS_SET_H

// ADS_set.h Double Hashing
// 
// VU Algorithmen und Datenstrukturen - WS 2020 Universitaet Wien
// Container - Projekt
// https://cewebs.cs.univie.ac.at/ADS/ws20/
//
// Beispielimplementierung mit "Double Hashing - Linear Probing" 
// Erstellt in den Lehrveranstaltungen im SS 2019
// 29.3.2019 (1. Abgabe)
// 5.4.2019 (voller Funktionsumfang)
//
// Nicht optimiert und eventuell fehlerhaft (Fehler bitte melden)
//
// Die Zurverfügungstellung des Programmes oder Teilen davon auf anderen Plattformen,
// Repositories, etc. ist nicht zulässig.

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

template <typename Key, size_t N = 7>
class ADS_set {
public:
  class Iterator;
  using value_type = Key;
  using key_type = Key;
  using reference = key_type &;
  using const_reference = const key_type &;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = Iterator;
  using const_iterator = Iterator;
  using key_compare = std::less<key_type>;   // B+-Tree
  using key_equal = std::equal_to<key_type>; // Hashing
  using hasher = std::hash<key_type>;        // Hashing

private:
  enum class Mode {free = 0, used, freeagain, end};
  struct element {
    key_type key;
    Mode mode {Mode::free};
  };
  element *table {nullptr};
  size_type table_size {0}, curr_size {0};
  float max_lf {0.7};
  size_type h(const key_type &key) const { return hasher{}(key) % table_size; }
  size_type g(const key_type &, size_type pos) const { return (pos + 1) % table_size; }
  void reserve(size_type n);
  void rehash(size_type n);
  element *insert_(const key_type &key);
  element *find_(const key_type &key) const;
public:
  ADS_set() { rehash(N); }
  ADS_set(std::initializer_list<key_type> ilist): ADS_set{} { insert(ilist); }
  template<typename InputIt> ADS_set(InputIt first, InputIt last): ADS_set{} { insert(first,last); } 
  ADS_set(const ADS_set &other): ADS_set{} {
    reserve(other.curr_size);
    for (const auto &k: other) {
      insert_(k);
    }
  }
  ~ADS_set() { delete[] table; }

  ADS_set &operator=(const ADS_set &other) {
    if (this == &other) return *this;
    ADS_set tmp{other};
    swap(tmp);
    return *this;
  }
  ADS_set &operator=(std::initializer_list<key_type> ilist) {
    ADS_set tmp{ilist};
    swap(tmp);
    return *this;
  }
  size_type size() const { return curr_size; }
  bool empty() const { return curr_size == 0; }

  size_type count(const key_type &key) const { return !!find_(key); }
  iterator find(const key_type &key) const {
    if (auto p {find_(key)}) return iterator{p};
    return end();
  }
  void clear() {
    ADS_set tmp;
    swap(tmp);
  }
  void swap(ADS_set &other) {
    using std::swap;
    swap(table, other.table);
    swap(curr_size, other.curr_size);
    swap(table_size, other.table_size);
    swap(max_lf, other.max_lf);
  }

  void insert(std::initializer_list<key_type> ilist) { insert(std::begin(ilist), std::end(ilist)); }
  std::pair<iterator,bool> insert(const key_type &key) {
    if (auto p {find_(key)}) return {iterator{p},false};
    reserve(curr_size+1);
    return {iterator{insert_(key)},true};
  }
  template<typename InputIt> void insert(InputIt first, InputIt last);

  size_type erase(const key_type &key) {
    if (auto p {find_(key)}) {
      p->mode = Mode::freeagain;
      --curr_size;
      return 1;
    }
    return 0;
  }
    
  const_iterator begin() const { return const_iterator{table}; }
  const_iterator end() const { return const_iterator{table+table_size}; }

#if TEST == 'z'
  const_iterator z() const { return const_iterator{table, true}; }
#elif TEST == 'y'
  const_iterator y() const { return const_iterator{table, true}; }
#elif TEST == 'x'
  size_t x() const {
    if (curr_size < 2) throw std::runtime_error{"order 66"};
    key_type *min1_ptr {nullptr}, *min2_ptr {nullptr};
    size_t min1_pos{0}, min2_pos{0}, pos{0};
    for (size_type idx{0}; idx < table_size; ++idx) {
      if (table[idx].mode == Mode::used) {
        if (!min1_ptr || key_compare{}(table[idx].key, *min1_ptr)) {
          min2_ptr = min1_ptr;
          min2_pos = min1_pos;
          min1_ptr = &table[idx].key;
          min1_pos = pos;
        } else if (!min2_ptr || key_compare{}(table[idx].key, *min2_ptr)) {
          min2_ptr = &table[idx].key;
          min2_pos = pos;
        }
        ++pos;
      }
    }
    return min1_pos > min2_pos ? min1_pos - min2_pos : min2_pos - min1_pos;
  }
#elif TEST == 'w'
  const_iterator w() const {
    // return const_iterator{std::min_element(begin(),end(),key_compare{}),true};
    const_iterator min_it {begin()};
    for (auto it {begin()}; it != end(); ++it) if (key_compare{}(*it, *min_it)) min_it = it;
    return const_iterator{min_it, true};
  }
#endif

  void dump(std::ostream &o = std::cerr) const;

  friend bool operator==(const ADS_set &lhs, const ADS_set &rhs) {
    if (lhs.curr_size != rhs.curr_size) return false;
    for (const auto &k: rhs) if (!lhs.count(k)) return false;
    return true;
  }
  friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs) { return !(lhs == rhs); }
};

template <typename Key, size_t N>
typename ADS_set<Key,N>::element *ADS_set<Key,N>::insert_(const key_type &key) {
  size_type idx {h(key)}, start_idx{idx};
  while (table[idx].mode != Mode::free && table[idx].mode != Mode::freeagain) {
    if ((idx = g(key, idx)) == start_idx) throw std::runtime_error{"insert_ leider nicht"};
  }
  table[idx].key = key;
  table[idx].mode = Mode::used;
  ++curr_size;
  return table+idx;
}

template <typename Key, size_t N>
typename ADS_set<Key,N>::element *ADS_set<Key,N>::find_(const key_type &key) const {
  size_type idx {h(key)}, start_idx{idx};
  do {
    switch (table[idx].mode) {
      case Mode::free:
        return nullptr;
      case Mode::used:
        if (key_equal{}(table[idx].key,key)) return table+idx;
      case Mode::freeagain:
        break;
      case Mode::end:
        throw std::runtime_error{"find_ reached END"};
    }
  } while ((idx = g(key, idx)) != start_idx);
  return nullptr;
}

template <typename Key, size_t N>
template<typename InputIt> void ADS_set<Key,N>::insert(InputIt first, InputIt last) {
  for (auto it {first}; it != last; ++it) {
    if (!count(*it)) {
      reserve(curr_size+1);
      insert_(*it);
    }
  }
}

template <typename Key, size_t N>
void ADS_set<Key,N>::reserve(size_type n) {
  if (n > table_size * max_lf) {
    size_type new_table_size {table_size};
    do {
      new_table_size = new_table_size * 2 + 1;
    } while (n > new_table_size * max_lf);
    rehash(new_table_size);
  }
}

template <typename Key, size_t N>
void ADS_set<Key,N>::rehash(size_type n) {
  size_type new_table_size {std::max(N, std::max(n,size_type(curr_size / max_lf)))};
  element *new_table {new element[new_table_size+1]};
  new_table[new_table_size].mode = Mode::end;
  size_type old_table_size {table_size};
  element *old_table {table};
  curr_size = 0;
  table = new_table;
  table_size = new_table_size;
  for (size_type idx{0}; idx < old_table_size; ++idx) {
    if (old_table[idx].mode == Mode::used) insert_(old_table[idx].key);
  }
  delete[] old_table;
}

template <typename Key, size_t N>
void ADS_set<Key,N>::dump(std::ostream &o) const {
  o << "curr_size = " << curr_size << " table_size = " << table_size << "\n";
  for (size_type idx{0}; idx < table_size+1; ++idx) {
    o << idx << ": ";
    switch (table[idx].mode) {
      case Mode::free:
        o << "--free";
        break;
      case Mode::freeagain:
        o << "--freeagain";
        break;
      case Mode::used:
        o << table[idx].key;
        break;
      case Mode::end:
        o << "--END";
        break;
    }
    o << "\n";
  }
}


template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {
public:
  using value_type = Key;
  using difference_type = std::ptrdiff_t;
  using reference = const value_type &;
  using pointer = const value_type *;
  using iterator_category = std::forward_iterator_tag;
private:
  element *pos;
#if TEST == 'z'
  bool special, jump;
#elif TEST == 'y'
  bool special;
  size_t n;
#elif TEST == 'w'
  bool special = false;
#endif
  void skip() { while (pos->mode != Mode::used && pos->mode != Mode::end) ++pos; }

public:
#if TEST == 'w'
  Iterator(const Iterator &it, bool special): Iterator{it} { this->special = special; }
#endif
#if TEST == 'z'
  explicit Iterator(element *pos = nullptr, bool special = false): pos{pos}, special{special}, jump{false} {
    if (this->pos) skip(); 
  }
#elif TEST == 'y'
  explicit Iterator(element *pos = nullptr, bool special = false): pos{pos}, special{special}, n{1} {
    if (!this->pos) return;
    skip(); 
    if (this->special && this->pos->mode != Mode::end) {
      ++this->pos;
      skip();
    }
  }
#else
  explicit Iterator(element *pos = nullptr): pos{pos} { if (pos) skip(); }
#endif
  reference operator*() const { return pos->key; }
  pointer operator->() const { return &pos->key; }
#if TEST == 'z'
  Iterator &operator++() { 
    if (special) {
      if ((jump = !jump)) return *this;
      ++pos; 
      skip(); 
      if (pos->mode == Mode::end) return *this;
    }
    ++pos; 
    skip();
    return *this;
  }
#elif TEST == 'y'
  Iterator &operator++() { 
    if (special) {
      if (n++%2) return *this;
      ++pos; 
      skip(); 
      if (pos->mode == Mode::end) return *this;
    }
    ++pos; 
    skip(); 
    return *this;
  }
#elif TEST == 'w'
  Iterator &operator++() { 
    do {
      ++pos; 
      skip();
    } while (special && pos->mode != Mode::end);
    return *this;
  }
#else
  Iterator &operator++() { ++pos; skip(); return *this; }
#endif
  Iterator operator++(int) { auto rc {*this}; ++*this; return rc; }
  friend bool operator==(const Iterator &lhs, const Iterator &rhs) { return lhs.pos == rhs.pos; }
  friend bool operator!=(const Iterator &lhs, const Iterator &rhs) { return !(lhs == rhs); }
};

template <typename Key, size_t N> void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) { lhs.swap(rhs); }

#endif // ADS_SET_H

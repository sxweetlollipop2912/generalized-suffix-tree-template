# Generalized Suffix Tree in C++

[Original implementation in Java](https://github.com/abahgat/suffixtree).
<br>
Referencing with this [C++ implementation](https://github.com/murraycu/murrayc-suffix-tree/tree/ukkonen) (suffix tree on
one string).

There is an all-in-one header `SuffixTree.h` at root directory.

All details regarding the algorithm is at the original repo.

### Operations
- `put(list, index)`: adds a `list` (not limited to string) associated with an `index` (not limited to int) to the tree. `index` will be returned at later retrievals.
- `search(sub-list)`: returns a std::set of `indexes` of the lists containing `sub-list`.

### Example
More examples in `main.cpp`.
``` c++
vector<string> words = {"qwe", "rtyr", "uio", "pas", "dfg", "hjk", "lzx", "cvb", "bnm"};

SuffixTree<string, int> tree;

for (int idx = 0; idx < words.size(); idx++) {
    tree.put(words[idx], idx);
}

for (int idx = 0; idx < words.size(); idx++) {
    set result = tree.search(word[idx].substr(1, 2));
    // found!
    assert(result.find(idx) != result.end());
}
```

### Added features

- Allow any type of list, not limited to string, with some requirements:
    1. Typename `value_type` (type of elements), `size_type` and `const_iterator` are defined and public.
    2. Support of `fast random access` is preferred (or expect drastic decrease in performance). `begin` and `end` iterator must
       meet `LegacyInputIterator` at minimum (for std::next, std::distance).
- Allow custom data type as element, as long as
    - `< operator` is defined.
- Allow custom data type as index, as long as
    - `< operator` is defined.
- Lists are stored as begin and end iterators. Hence, it is **EXTREMELY IMPORTANT** to store those lists elsewhere.

### Other stuff

- Requires *C++11* at minimum.

- In a sense, this suffix tree is similar to a map, where key is the list, value is the index, but search condition is different.

-  This template is originally created to help perform search queries in a dictionary.

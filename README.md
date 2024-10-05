# Generalized Suffix Tree in C++

Enhancing the [original implementation in Java](https://github.com/abahgat/suffixtree).
<br>
Referencing with [C++ implementation](https://github.com/murraycu/murrayc-suffix-tree/tree/ukkonen) (suffix tree on
one string).

All-in-one header: `SuffixTree.h`.

### Operations
- `put(list, value)`: adds a `list` associated with a `value`. `value` will be returned at later retrievals.
- `search(sub-list)`: returns a std::set of `values` of the lists containing `sub-list`.

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

### New features

- Allow any type of `sequence container` as `list`, if:
    1. Typename `value_type` (type of list elements), `size_type` and `const_iterator` are public.
    2. `begin` and `end` iterator must meet `LegacyInputIterator` at minimum.
- Allow custom type as list element if
    - `< operator` is defined.
- Allow custom type as `value` if
    - `< operator` is defined.

### Misc
- DO NOT DESTROY the lists. They are only stored as begin and end iterators in the tree.

- Requires C++11 at minimum.

- This template is originally created to help perform search queries in a dictionary.

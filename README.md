# Generalized Suffix Tree in C++

Enhancing the [original implementation in Java](https://github.com/abahgat/suffixtree).
<br>
Referencing with [C++ implementation](https://github.com/murraycu/murrayc-suffix-tree/tree/ukkonen) (suffix tree on
one string).

All-in-one header: [`SuffixTree.h`](https://github.com/sxweetlollipop2912/suffix-tree-template/blob/main/SuffixTree.h).

### Operations
- `put(list, value)`: adds a `list` associated with a `value`. `value` will be returned at later retrievals.
- `search(sub-list)`: returns a std::set of `values` of the lists containing `sub-list`.

### Example
More examples in [`main.cpp`](https://github.com/sxweetlollipop2912/suffix-tree-template/blob/main/main.cpp).
``` c++
vector<string> words = {"qwe", "rtyr", "uio", "pas", "dfg", "hjk", "lzx", "cvb", "bnm"};

SuffixTree<string, int> tree;

for (int idx = 0; idx < words.size(); idx++) {
    // By putting `words[idx]` into SuffixTree, it becomes a candidate for future searches.
    // If `words[idx]` is part of a search result, SuffixTree will returns the `idx`
    // (the 2nd param) as this word identifier.
    tree.put(words[idx], idx);
}

for (int idx = 0; idx < words.size(); idx++) {
    set result = tree.search(word[idx].substr(1, 2));
    // found!
    assert(result.find(idx) != result.end());
}
```

### Notable features

1. Besides searching on strings, **this template allows searching on any other type of list / array / ... (container that stores objects of the same type in a linear arrangement)**, if:
    - Typename `value_type` (type of list elements), `size_type` and `const_iterator` are public.
    - `begin` and `end` iterator must meet `LegacyInputIterator` at minimum.

This means you can search on C++ std containers like `std::vector`. Other std containers may be applicable as well, but I haven't checked.

2. If you use a container other than string, the element type must satisfy:
    - `< operator` is defined (so that Suffix Tree can operate on it)

3. If you use an arbitrary type (other than index integers) as identifier, the type must satisfy:
    - `< operator` is defined (so that it can be put into `std::set`)

### Misc
- DO NOT DESTROY the lists. They are only stored as begin and end iterators in the tree.

- Requires C++11 at minimum.

- This template is originally created to help perform search queries in a dictionary.

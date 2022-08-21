# Generalized Suffix Tree in C++

[Original implementation in Java](https://github.com/abahgat/suffixtree).
<br>
Referencing with this [C++ implementation](https://github.com/murraycu/murrayc-suffix-tree/tree/ukkonen) (suffix tree on
one string).

You can find details of this version of Ukkonen's algorithm at the original repo.

There is an all-in-one header at root directory.

### More features were added

- Allow any type of list (with some requirements), not limited to string. Those requirements are:
    1. Typename `value_type` (type of elements), `size_type` and `const_iterator` are defined and public.
    2. Support of `fast random access` is preferred (or expect drastic decrease in performance). `begin` and `end` iterator must
       meet `LegacyInputIterator` at minimum (for std::next, std::distance).
- Allow custom data type as element, as long as
    - `< operator` is defined.
- Allow custom data type as index, as long as
    - `< operator` is defined.
- Lists are stored as begin and end iterators. Hence, it is **EXTREMELY IMPORTANT** to store those lists elsewhere.

Requires *C++11* at minimum.

In a sense, this suffix tree is similar to a map. Only that, key must be a list, and search condition is different.

This template is originally created to help perform search queries in a dictionary.
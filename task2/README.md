# Task 2. Slice-n-dice

## Preparation

Complete [Task 1](https://github.com/Mrkol/metaprogramming-course/tree/master/tasks/span) first.

Review how inheritance works in C++. Does inheritance necessarily imply an object-oriented approach?

## Task

### Slice

Implement `Slice`, a more general version of `std::span` with an additional template parameter, `stride`, specifying the step between consecutive elements. For example:

```c++
std::array<int, 10> data;
std::iota(data.begin(), data.end(), 0);
Slice<int, 5, 2> even_elements{data};
```

Here, `even_elements` must contain `0, 2, 4, 6, 8`. The implementation must also support runtime values for either parameter or both parameters by using `std::dynamic_extent` and `dynamic_stride`, respectively.

### Slice Interface

The `Slice` template must reproduce the interface of [`std::span`](https://en.cppreference.com/w/cpp/container/span), except for the following operations:

- `size_bytes`
- `as_bytes`
- `as_writable_bytes`
- `subspan`

These operations assume `stride == 1`, so omit them for this task. Also ignore the "Helper templates" section. The constructors of `std::span` involve many subtleties; instead of reproducing them exactly as documented on cppreference, write your own constructors so that the tests pass.

Add the following operations related to `stride`:

- `Skip` methods that change the stride.
- `DropFirst` methods that discard a number of elements from the beginning.
- `DropLast` methods that discard a number of elements from the end.
- A version of the iterator-and-size constructor that also accepts a stride.

A `Subslice` method analogous to `subspan` is not required. Supporting every combination of runtime and compile-time parameters in such a method would lead to a combinatorial explosion. Instead, provide the simpler methods `First`, `Last`, `DropFirst`, `DropLast`, and `Stride`. Each must have two overloads: one taking a template argument and one taking a regular function argument.

Remember that `const Slice<T>` and `Slice<const T>` are different. The former prevents modification of the `Slice` object itself; the latter prevents modification of the elements viewed by the slice.

A slice with static parameters must be implicitly convertible to the corresponding slice with dynamic parameters, whether one parameter or both become dynamic. Conversion from `Slice<T>` to `Slice<const T>` must also be implicit.

Each specialization of `Slice` must have the smallest possible object size. Do not store the extent or stride at runtime when it is known at compile time. Consider how to achieve this without implementing the entire task four times. Inheritance may help. You may use all the facilities of C++ and completely change the solution template; the implementation must pass the tests.

Keep helper templates out of the global namespace by placing them in a separate namespace.

## Submission Requirements

**Points:** 250.

The `Slice` template must be available in the global namespace when `Slice.hpp` is included. Additional files and classes are allowed. Any `.cpp` files in the directory will be compiled and linked with the tests automatically, although they are unlikely to be needed for this task.

Push your code to the `slice` branch and open a pull request targeting `master`.

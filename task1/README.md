# Task 1. Warm-up

## Preparation

Review templates and explicit specializations, both full and partial.

Recall that the standard library's `std::span` template provides a non-owning view of a sequence of objects stored contiguously in memory. It serves as a common interface for different kinds of [contiguous containers](https://en.cppreference.com/w/cpp/named_req/ContiguousContainer). Using it as a function parameter allows callers to pass a `std::vector` with any allocator, a `std::array` of any size, wrappers such as [`std::flat_map`](https://en.cppreference.com/w/cpp/header/flat_map), and containers from third-party libraries, which are common in practice.

Note that `std::span` has two template parameters: the element type and the extent. The extent can be `std::dynamic_extent`, meaning that the size is unknown at compile time and will be determined at runtime.

Review [user-defined deduction guides](https://en.cppreference.com/w/cpp/language/class_template_argument_deduction).

Some operations on `std::span` may be invalid. For example, constructing a `std::span<int, 1000>` from an empty `std::vector<int>` is invalid because the vector does not contain a thousand elements. Use the `MPC_VERIFY(cond, message)` macro from `"lib/assert.hpp"` to check this and other invariants at runtime. A dedicated macro is used to make these checks easy to test.

## Task

Implement your own version of `std::span`.

Remember that `const Span<T>` and `Span<const T>` are different. The former prevents modification of the `Span<T>` object itself; the latter prevents modification of the `T` objects viewed by the span.

Each specialization of `Span` must have the smallest possible object size. In particular, a specialization whose extent is known at compile time must not contain a field that stores the size at runtime.

Use pointers as iterators.

Add assertions for out-of-bounds operations in every method.

## Submission Requirements

**Points:** 100.

The `Span` template must be available in the global namespace when `Span.hpp` is included.

Push your code to the `span` branch and open a pull request targeting `master`.

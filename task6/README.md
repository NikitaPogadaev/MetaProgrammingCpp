# Task 6. Enumerating Enumerators

## Preparation

Use the latest stable version of GCC or Clang. Older versions are likely to work as well, but have not been tested.

Review [scoped and unscoped enumerations](https://en.cppreference.com/w/cpp/language/enum) and their underlying types.

Experiment with the contents of the built-in variable [`__PRETTY_FUNCTION__`](https://gcc.gnu.org/onlinedocs/gcc/Function-Names.html) inside function templates. Try using an enum value as a non-type template argument. What does it contain if an *arbitrary* number is cast to the enum type and passed as a template argument?

Note that all methods of `std::string_view` are declared `constexpr`.

## Task

Implement the class template `EnumeratorTraits` with the template parameters and public methods specified in the solution template.

Let `Enum` be a scoped or unscoped enumeration. The value of an enumerator `e` of type `Enum` is defined as `static_cast<std::underlying_type_t<Enum>>(e)`.

- `size()` returns the number of enumerators in `Enum`.
- `at(i)` returns the i-th enumerator in ascending order of value.
- `nameAt(i)` returns the name of the i-th enumerator in ascending order of value.

**The following conditions are guaranteed:**

- Unscoped enumerations have an explicitly specified underlying type. Determine why this is necessary.
- The absolute value of every enumerator is at most `MAXN`.
- All enumerators have distinct values.
- The argument to `nameAt` and `at` is between `0` and `size() - 1`, inclusive.

Otherwise, the behavior is undefined.

### Complexity Requirements

- `size`, `at`, and `nameAt` must run in constant time with respect to both `MAXN` and the number of enumerators.
- Storage allocated during instantiation may grow linearly with `MAXN`. This refers to data explicitly stored in fields or variables; the compiler's internal memory consumption is outside the scope of this requirement.
- There are no requirements on compilation time during instantiation.

> A note on compile-time memory usage
>
> If linear memory usage at compile time were required, the standard recursive approach to processing parameter packs would not be suitable.
>
> The compiler uses the template argument list as a key when caching template instantiations. With the usual approach of removing a few arguments from the beginning of a pack and recursively instantiating the remaining tail, the total number of template arguments across all instantiations is the sum of an arithmetic progression. It therefore grows quadratically with the length of the original pack.
>
> Recursion is not needed to solve this task, however.

For `MAXN <= 512`, the code must compile with the latest stable versions of both Clang and GCC without flags that increase template recursion depth, constexpr function recursion depth, or the number of constexpr evaluation steps.

## Example

```cpp
enum Fruit : unsigned short {
    APPLE, MELON
};

enum class Shape {
    SQUARE, CIRCLE = 5, LINE, POINT = -2
};

static_assert(EnumeratorTraits<Shape>::size() == 4);
static_assert(EnumeratorTraits<Fruit>::size() == 2);

static_assert(EnumeratorTraits<Fruit>::nameAt(0) == "APPLE");
static_assert(EnumeratorTraits<Fruit>::nameAt(1) == "MELON");

static_assert(EnumeratorTraits<Shape>::nameAt(0) == "POINT");
static_assert(EnumeratorTraits<Shape>::at(0) == Shape::POINT);
static_assert(EnumeratorTraits<Shape>::at(1) == Shape::SQUARE);
static_assert(EnumeratorTraits<Shape>::at(3) == Shape::LINE);
```

## Submission Requirements

**Points:** 300.

The `EnumeratorTraits` class must be available in the global namespace when `EnumeratorTraits.hpp` is included. This header must be located in the `enumerators` directory at the repository root.

Push your code to the `enumerators` branch and open a pull request targeting `master`.

You may use the full standard library, including type traits and the concepts library, as needed.

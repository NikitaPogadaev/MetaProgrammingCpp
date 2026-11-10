# Task 3. Infinite Laziness

## Preparation

The seminar introduced type lists and several ways to define them. In this task, you will extend the "old" approach using modern language facilities.

Review how class template instantiation works: instantiation of declarations, definitions, and members.

Review constraints and concepts, what a concept represents at a conceptual level, and how constraints interact with explicit specializations and overloads.

Recall that declarations from a base class can be introduced into a derived class using `using BaseClassName::EntityName;`. This works for functions, data members, and *type aliases*.

Recall that constraints can be used to specialize class templates and overload functions. However, compilers have limited ability to infer implications between sets of constraints, so obtaining the desired behavior may require some effort.

## Task

### Working with Infinite Type Lists

Consider the following concept:

```c++
template<class TL>
concept TypeSequence =
    requires {
        typename TL::Head;
        typename TL::Tail;
    };
```

This concept is intended to describe *infinite sequences of types*. There is also an implicit requirement that `TL::Tail` satisfy `TypeSequence`, although expressing this requirement in C++20 is probably not possible.

How can a sequence of types be infinite when the compiler has only a finite amount of memory?

The answer is the same as in functional programming languages: lazy evaluation. The compiler instantiates members of class templates lazily, including aliases such as `Tail`. These aliases can therefore refer to the same template with different arguments.

For example:

```c++
template<class T>
struct FunnyStarrySequence {
    using Head = T;
    using Tail = FunnyStarrySequence<T*>;
};
```

Instantiations of this template describe infinite lists of the form `(T, T*, T**, T***, T****, ...)`. The compiler does not store the entire list in memory, only a rule for obtaining subsequent elements. It computes only as many elements as are requested.

Working exclusively with infinite sequences of types is inconvenient, so consider the following definitions:

```c++
struct Nil {};

template<class TL>
concept Empty = std::derived_from<TL, Nil>;

template<class TL>
concept TypeList = Empty<TL> || TypeSequence<TL>;
```

The `Empty` concept describes an empty list as any type derived from `Nil`. The `TypeList` concept describes a *potentially infinite list* as either an empty list or a sequence. The implicit requirement on `TL::Tail` must now be relaxed: it must satisfy the more general `TypeList` concept rather than `TypeSequence` specifically.

A more principled approach would be to define `TypeList` independently of `TypeSequence` and treat them as separate concepts. However, the chosen definition lets the compiler infer that every sequence is also a list, which can be useful for function overloading.

Implement a collection of operations on infinite lists, working out how to represent concrete type lists along the way.

### "New" Type Lists

To distinguish variadic type lists from potentially infinite lists, call them *type tuples*. They are needed in a few places in this task, but do not require particularly elaborate algorithms. Their definition is:

```c++
template<class... Ts>
struct TTuple {};
```

All such tuples can be grouped into a single family using the `TypeTuple` concept. Study its implementation in the solution template and consider what else this technique can achieve.

### Part 1: Operations

Place the following metafunctions in the `type_lists` namespace:

- `Cons<T, TL>`: a list whose first element is `T`, followed by the elements of `TL`.
- `FromTuple<TT>` / `ToTuple<TL>`: conversions between **finite** lists and tuples. These are useful for debugging.
- `Repeat<T>`: an infinite list containing only `T`.
- `Take<N, TL>`: the first `N` elements of the potentially infinite list `TL`.
- `Drop<N, TL>`: the list `TL` without its first `N` elements.
- `Replicate<N, T>`: a list of `N` elements, each equal to `T`.
- `Map<F, TL>`: a list of the results of applying `F` to the elements of `TL`.
- `Filter<P, TL>`: a list containing only elements for which `P<_>::Value` is true. Preserve their relative order.
- `Iterate<F, T>`: a list starting with `T`, where each subsequent element is the result of applying the metafunction `F` to the previous element.
- `Cycle<TL>`: an infinite list formed by repeatedly cycling through the finite list `TL`.
- `Inits<TL>`: a list of all prefixes of `TL`, ordered by increasing length.
- `Tails<TL>`: a list of all suffixes of `TL`, ordered by increasing length of the prefix removed from the original list.
- `Scanl<OP, T, TL>`: a sequence starting with `T`, where each subsequent element is obtained by applying `OP<_, _>` to the current accumulated element and the next element of `TL`.
- `Foldl<OP, T, TL>`: the type obtained as `OP<... OP<OP<T, TL[0]>, TL[1]> ... >`. The result is undefined for an infinite sequence.
- `Foldr<OP, T, TL>`: the type obtained as `OP<TL[0], OP<TL[1], ... OP<TL[n-1], T> ...>>`. The result is undefined for an infinite sequence.
- `Zip2<L, R>`: a list of pairs, with each pair containing the elements at the same index in `L` and `R`, in order.
- `Zip<TL...>`: a list of tuples, each containing one element at the same index from every input list.

**Bonus (+100 points):**

- `GroupBy<EQ, TL>`: a list of lists of **consecutive** elements of `TL` that are equal **pairwise in sequence**: each next element must be equal to the current one. This can, for example, be used to identify increasing subsequences. Equality means `EQ<T, S>::Value == true`. Grouping the letters of "Mississippi" by equality produces `["M", "i", "ss", "i", "ss", "i", "pp", "i"]`. If every element of an *infinite* sequence `TL` is equal, the behavior is undefined; consider what prevents it from being defined. Here, `EQ` is a metapredicate satisfying the axioms of equality.

### Part 2: Numeric Sequences

Place the contents of this section in the global namespace.

Introduce the following class:

```c++
template<auto V>
struct ValueTag{ static constexpr auto Value = V; };
```

It stores values inside types, making it possible to work with infinite lists of values at compile time. Define the following types:

- `Nats`: the natural numbers `(0, 1, 2, 3, ...)`.
- `Fib`: the Fibonacci numbers `(0, 1, 1, 2, ...)`.
- `Primes`: the prime numbers `(2, 3, 5, 7, ...)`.

Reuse the operations implemented above wherever possible. Place these definitions in `fun_value_sequences.hpp`.

## Examples and Tests

In general, deciding whether two infinite sequences are equal reduces to the halting problem. For debugging, follow the approach used in the [tests](https://github.com/Mrkol/metaprogramming-course/tree/master/tasks/type_lists/tests): take a finite portion of an infinite list, convert it to a tuple, and compare the tuples using `std::is_same`. If a list contains other lists, truncate and convert them recursively using `Map` and a curried version of `Take`.

## Submission Requirements

**Points:** 400 + 100.

Push your code to the `type_lists` branch and open a pull request targeting `master`. Add the grader as a reviewer.

Apply constraints to template arguments wherever possible. Where appropriate, group related constraints into concepts.

## Background

During a seminar on type lists, the instructor was browsing the Haskell `Data.List` reference and, on reaching functions for infinite lists, remarked that these were beyond what could be implemented. It turns out they are not. A few days of experimentation showed that infinite lists can indeed be implemented in C++, with a reasonably elegant result. Their practical usefulness may be debatable, but they make an excellent exercise.

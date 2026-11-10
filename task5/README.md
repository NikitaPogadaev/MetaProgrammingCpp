# Task 5. A Very Smart Non-Pointer

## Preparation

Review [concepts and constraints](https://en.cppreference.com/w/cpp/language/constraints) and the standard library's [object concepts](https://en.cppreference.com/w/cpp/concepts).

Review the available approaches to implementing type erasure.

Review the public interfaces of C++ value wrappers such as `std::optional` and `std::unique_ptr`. Which operators must they support? Which constructors are needed?

## Task

Implement a class template `Spy` that wraps an arbitrary object and logs accesses to it.

The wrapped object must be stored **by value**. Calling such wrappers pointers is misleading, so they will be called smart non-pointers.

If `s` is a value of type `Spy<T>`, the expression `s->member` must access the non-static member `member` of the wrapped object.

### Loggers

The `setLogger` method installs a logger. After the evaluation of each expression that accesses the wrapped object through `operator->`, invoke the logger if one is installed. The logger receives the number of accesses to the object during the evaluation of that expression.

Accesses through `operator*` are not logged.

If the wrapped type `T` is not copyable, `Spy<T>` must support move-only loggers.

If a single expression both accesses the wrapped object and changes the logger, the behavior is undefined.

Using `std::function` or `std::any` is prohibited in this task.

### Preserving Concepts

A smart non-pointer `W` is said to preserve a concept `C` if, for every type `T`:

1. If `T` satisfies `C`, then `W<T>` satisfies `C`.
2. If `T` models `C`, then `W<T>` models `C`.

Your `Spy` must preserve the fundamental object concepts: `std::movable`, `std::copyable`, `std::semiregular`, and `std::regular`. To achieve this, you may impose additional constraints on the template argument of `setLogger`.

Comparison operators must compare the wrapped objects and ignore the loggers. Copying or moving a `Spy` must copy or move both its logger and its wrapped object.

If a single expression both accesses the wrapped object and moves the smart non-pointer, the behavior is undefined. Copying creates a new object whose accesses must be counted **separately**.

### Bonus: Allocators and Small Buffer Optimization (+2 Units)

Support custom allocators and small buffer optimization for loggers. This task uses C++20. Instead of placement new, use `std::allocator_traits<Alloc>::construct`, which in turn calls `std::construct_at`. Instead of calling a destructor directly, use `std::allocator_traits<Alloc>::destroy`, which in turn calls `std::destroy_at`.

Since the logger type is unknown when a `Spy` is created, use an allocator for `std::byte`. However, `construct` and `destroy` take the object type as a template parameter, so the same allocator can allocate storage for and construct loggers of different types.

Review the [AllocatorAwareContainer](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer) named requirements and this [article on allocator propagation](https://www.foonathan.net/2015/10/allocatorawarecontainer-propagation-pitfalls/), particularly `propagate_on_container_copy_assignment` and `propagate_on_container_move_assignment`. Remember to reallocate storage when move-assigning a `Spy` if `propagate_on_container_move_assignment` is false and the allocators differ.

### Bonus 2: A Generic Virtual Function Table (No Points or Tests)

Design or adapt and implement a generic virtual function table mechanism. The resulting `Spy` should support a user-selected "move-only" policy that removes the type-erased move function and reduces its size by a few bytes. Also provide a policy that places the virtual function table in static storage. In that case, virtual calls require two levels of indirection, but the `Spy` object becomes even smaller.

## Example

```c++
struct Holder {
    int x = 0;
    bool isPositive() const {
        return x > 0;
    }
};

Spy s{Holder{}};
static_assert(std::semiregular<decltype(s)>);

s.setLogger([](auto n) { std::cout << n << std::endl; });

s->isPositive() && s->x--; // prints 1
s->x++ + s->x++; // prints 2
s->isPositive() && s->x--; // prints 2

s.setLogger([dummy = std::unique_ptr<int>()](auto n) {}); // compilation error

// -----------------------------------

struct MoveOnly {
    MoveOnly() = default;

    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator =(MoveOnly&&) = default;

    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator =(const MoveOnly&) = delete;

    ~MoveOnly() noexcept = default;
};

Spy t{MoveOnly{}};

t.setLogger([dummy = std::unique_ptr<int>()](auto n) {}); // ok

static_assert(std::movable<decltype(t)>);
static_assert(!std::copyable<decltype(t)>);
```

## Questions to Consider

1. What problems would arise if a const version of `operator->` were added?
2. How would you modify `Spy` to support access to the wrapped object from multiple threads?
3. Can every combination of `noexcept` specifications on the logger's type-erased methods be handled cleanly?
4. It would be useful to preserve the access counter when a move occurs within an expression, as though all accesses referred to the same object:

   ```c++
   (a->doSmth(), (b = std::move(a))->doSmth()); // 2
   ```

   Consider how to implement this without dynamic memory allocation. Why would the solution work? Implement it if you wish.

## Submission Requirements

**Points:** 300 + 200.

Push your code to the `spy` branch and open a pull request targeting `master`.

You may use the full standard library except for `std::function` and `std::any`.

Answering the questions above is optional.

## Tests

The tests for this task are built with AddressSanitizer and UndefinedBehaviorSanitizer. Take particular care when implementing the bonus requirements.

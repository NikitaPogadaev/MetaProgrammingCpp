# Task 4. Sounds of Nature

## Preparation

Since C++20, a non-type template parameter may have a class type. Previously, only integral types, pointers, references, and enumeration types could be used. The following is now possible:

```c++
struct Data {
    int a;
    char b;
};

template <Data input>
class StaticDataHolder {};

StaticDataHolder<{5, 'c'}> holder;
```

There are, of course, [many restrictions](https://en.cppreference.com/w/cpp/language/template_parameters#Non-type_template_parameter). GCC has supported this feature since version 9. It will be useful for this task.

Review how to define [user-defined string literal operators](https://en.cppreference.com/w/cpp/language/user_literal).

Review how `dynamic_cast` works. What is the difference between `dynamic_cast<T*>(ptr)` and `dynamic_cast<T&>(value)`?

Review [variadic templates](https://en.cppreference.com/w/cpp/language/parameter_pack).

**Use GCC 10.**

## Task

### Mapping Polymorphic Types to Objects

Suppose there is a polymorphic hierarchy of classes derived from `Base`. Each class in the hierarchy is associated with a value of type `Target`. Given a reference `Base& object`, find the value associated with the actual type of `object`.

Create a generic mechanism for defining such mappings.

Implement the `Mapping` and `PolymorphicMapper` classes and the static method `PolymorphicMapper::map` in `string_mapper.hpp`.

When `map(object)` is called, the mapper must search `Mappings...` for the first `Mapping<From, target>` such that the actual type of `object` is `From` or a class derived from `From`, and return `target`. Return `std::nullopt` if no matching mapping is found.

The code **must not compile** if, when `PolymorphicMapper<Base, Target, Mappings...>::map` is instantiated, any type in `Mappings...` is not a `Mapping<SomeClass, Target some_value>` where `SomeClass` is derived from `Base`. This may happen naturally as a consequence of the implementation, but provide a meaningful diagnostic for the user where possible.

The mappings are guaranteed to be supplied in inheritance order: derived classes precede their base classes. Otherwise, the behavior is undefined.

If the requirements are unclear, read the following section and then the example.

### Bonus: Unordered Mappings (+1 Unit)

This section is optional.

Make the mapper work even when the mappings are supplied in an arbitrary order. See the [tests for unordered mappings](https://github.com/Mrkol/metaprogramming-course/blob/master/tasks/mapper/tests/unsorted.cpp).

**At runtime, the mapper must use O(n) calls to `dynamic_cast`.**

### Compile-Time Strings

1. Implement a class template `FixedString` with one non-type template parameter, `size_t max_length`, a constructor with parameters `const char* string, size_t length`, and an implicit conversion operator to `std::string_view`. The class must store the first `length` characters of `string` and expose them when converted to `std::string_view`. If `length > max_length`, the behavior is undefined.
2. Implement the literal operator `""_cstr`, returning a `FixedString<256>` containing the literal. If the literal is longer than 256 characters, the behavior is undefined.
3. Make `FixedString` and `""_cstr` usable at compile time.
4. Make objects of type `FixedString` usable as non-type template arguments.

Pay attention to the assertions at the beginning of the main test file. They must pass, but passing them alone is not sufficient to make the class suitable as the type of a non-type template parameter. See the restrictions linked above.

## Example

Suppose a class hierarchy represents animals. Given a pointer to an animal, determine which sound it makes. The following code should work:

```cpp
class Animal {
public:
    virtual ~Animal() = default;
};

class Cat : public Animal {};
class Cow : public Animal {};
class Dog : public Animal {};

using MyMapper = PolymorphicMapper<
    Animal, FixedString<256>,
    Mapping<Cat, "Meow"_cstr>,
    Mapping<Dog, "Bark"_cstr>,
    Mapping<Cow, "Moo"_cstr>
>;


std::unique_ptr<Animal> some_animal{new Dog()};
std::string_view dog_sound = *MyMapper::map(*some_animal);
ASSERT_EQ(dog_sound, "Bark");
```

Also review the [tests](https://github.com/Mrkol/metaprogramming-course/blob/master/tasks/mapper/tests/main.cpp).

## Submission Requirements

**Points:** 200 + 100.

Push your code to the `mapper` branch and open a pull request targeting `master`.

You may use the full standard library, including type traits and the concepts library, as needed.

Feel free to modify the solution template; it is deliberately minimal.

## Background

The author encountered a similar problem when converting C++ exceptions to Java exceptions and throwing them through JNI. A large hierarchy of C++ exception types was involved. An exception was caught by reference to a base-class object, and the corresponding Java class name then had to be determined.

Before C++20, this could be implemented using the [GNU extension for literal operators](https://habr.com/ru/post/243581/).

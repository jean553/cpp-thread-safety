# cpp-thread-safety

## Compile and run the example

```bash
g++ main.cpp -o cpp-thread-safety -lpthread
./cpp-thread-safety
```

## The problem to solve

Two threads access concurently the same attribute `value` through the method `action`.

```cpp
class MyClass {

public:

    void action() const & noexcept {

        constexpr unsigned short ITERATIONS {10};
        for (
            auto i = 0;
            i < ITERATIONS;
            i += 1
        ) {
            constexpr unsigned short TIME {1000};
            std::this_thread::sleep_for(
                std::chrono::milliseconds(TIME)
            );

            value += 1;

            std::cout << value << std::endl;
        }
    }

    void process() const & noexcept {

        std::thread firstThread(
            &MyClass::action,
            this
        );

        std::thread secondThread(
            &MyClass::action,
            this
        );

        firstThread.join();
        secondThread.join();
    }

private:

    mutable int value {0};
};

int main() {

    MyClass object;
    object.process();

    return EXIT_SUCCESS;
}
```

The method `action` is `const`. As it modifies the `value` attribute,
`value` is declared as `mutable`.

By executing this code, we get a list with (sometimes) two times
the same number. Example:

```
3
3
...
6
6
...
```

The displayed range is from 1 to 9 with double values (sometimes).

In fact, there is a concurrent access to the variable `value`,
and that's what happen when the two threads to read and write it at the same time.

## `std::mutex` solution

We can add a mutex and a lock guard. Using the them, we can protect the attribute
update action and the output action to the screen.

```cpp
{
    std::lock_guard<std::mutex> guard(mutex);

    value += 1;

    std::cout << value << std::endl;
}

...

private:

    mutable std::mutex mutex;
```

The rendered output is now correct. In fact, we have the guarantee
that the two threads never update and output `value` at the same time.

The displayed range is from 1 to 19 without double values and without two numbers
on the same line (`std::cout` is part of the `std::locak_guard` block).

## `std::atomic` solution

Instead of a lock guard, it is possible to simply declare `value` as atomic.

```cpp
private:

    mutable std::atomic<int> value {0};
```

Once again, the output is the one expected, except that (sometimes),
two different numbers are on the same line.

In fact, contrary to the previous example, the `std::cout` action
is not part of the `atomic` action. That's explain why some numbers
are on the same line.

This solution works as long as there is only one attribute to modify
at every method call. If there is more than one attribute to modify,
it is often better to use the `std::mutex` and `std::lock_guard` method.

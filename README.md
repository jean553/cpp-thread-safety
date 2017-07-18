# cpp-thread-safety

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
```

In fact, there is a concurrent access to the variable `value`,
and that's what happen when the two threads to read and write it at the same time.

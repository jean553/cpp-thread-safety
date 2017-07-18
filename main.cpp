#include <iostream>
#include <thread>
#include <mutex>

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

            {
                std::lock_guard<std::mutex> guard(mutex);

                value += 1;

                std::cout << value << std::endl;
            }
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

    mutable std::mutex mutex;
    mutable int value {0};
};

int main() {

    MyClass object;
    object.process();

    return EXIT_SUCCESS;
}

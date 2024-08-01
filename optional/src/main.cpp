#include <iostream>
#include "optional.h"

int main() {
    try {
        opt::optional<int> opt1;  // Default constructed empty optional
        std::cout << "opt1 has value? " << std::boolalpha << opt1.has_value() << std::endl;

        // Testing emplace and value
        opt1.emplace(42);
        std::cout << "opt1 value: " << opt1.value() << std::endl;

        // Testing copy constructor and operator=
        opt::optional<int> opt2 = opt1;
        std::cout << "opt2 value: " << opt2.value() << std::endl;

        // Testing move constructor and operator=
        opt::optional<int> opt3 = std::move(opt2);
        std::cout << "opt3 value: " << opt3.value() << std::endl;

        // Testing value_or
        opt::optional<int> opt4{};  // Empty optional
        int value = opt4.value_or(100);
        std::cout << "opt4 value_or: " << value << std::endl;

        // Testing bad_optional_access exception
        opt::optional<int> opt5{};
        try {
            int val = opt5.value();  // Should throw bad_optional_access
        } catch (const opt::optional<int>::bad_optional_access& ex) {
            std::cerr << "Exception: " << ex.what() << std::endl;
        }

        // Testing or_else
        
        auto fallback = []() { return opt::optional<int>(50); };
        auto result = opt5.or_else(fallback);
        std::cout << "opt5 or_else: " << result.value() << std::endl;

        // Testing modifiers
        opt5.reset();
        std::cout << "opt5 has value after reset? " << std::boolalpha << opt5.has_value() << std::endl;

        // Testing comparison operators
        opt::optional<int> opt6(10);
        opt::optional<int> opt7(20);
        std::cout << "opt6 < opt7? " << std::boolalpha << (opt6 < opt7) << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }

    return 0;
}

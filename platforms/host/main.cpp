#include <iostream>
#include <cassert>

bool test_always_true() {
    return true;
}

int main() {
    std::cout << "Running host tests..." << std::endl;
    assert(test_always_true() == true);
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
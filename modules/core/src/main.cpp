#include <iostream>
#include "chainforge/core/hash.hpp"

int main() {
    std::cout << "ChainForge Core Test" << std::endl;
    
    try {
        chainforge::core::Hash hash = chainforge::core::Hash::random();
        std::cout << "Generated hash: " << hash.to_hex() << std::endl;
        std::cout << "Hash size: " << hash.size() << " bytes" << std::endl;
        std::cout << "Test completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

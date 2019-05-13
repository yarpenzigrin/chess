#ifndef TEST_CHESSTEST_HPP_
#define TEST_CHESSTEST_HPP_

#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

using Test = std::function<void(void)>;
std::vector<Test> tests;
std::vector<const char*> failures;
std::stringstream test_output;

namespace
{

static bool add_test(void (*fn)(void), const char* test_name)
{
    tests.push_back([fn, test_name](){
        printf("***** %s ***** ", test_name);
        try {
            test_output = std::stringstream();
            fn();
        } catch (const std::runtime_error& e) {
            std::cout << '\n' << test_output.str();
            failures.push_back(test_name);
            printf("^^^^^ %s ^^^^^\n", test_name);
            return;
        }
        printf("OK\n");
    });
    return true;
}

}  // namespace

#define TEST(name)                                                                  \
    void name();                                                                    \
    static bool name##_init = add_test(name, #name);                                \
    void name()                                                                     \

#define ASSERT(cond) if (!(cond)) { printf("\n!!! %s:%d Condition:\n  \"%s\"\nfailed.\n\n", \
    __FILE__, __LINE__, #cond);                                                             \
    throw std::runtime_error("Assertion failed."); }

int main() {
    for (auto test : tests)
        test();
    if (!failures.empty())
    {
        printf("\n\nFAILURES:\n");
        for (auto failed_test : failures)
            printf(" * %s\n", failed_test);
    }
    return 0;
}

#endif  // TEST_CHESSTEST_HPP_

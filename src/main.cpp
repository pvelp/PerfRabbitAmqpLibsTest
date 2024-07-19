#include <deque>
#include <argparse/argparse.hpp>
#include "SimpleAmqpExecutor.hpp"


template <typename E>
void test_rw(E executor){
    executor.test_rw();
}

template <typename E>
void test_immediately_rw(E executor){
    executor.test_immediately_rw();
}

int main(int argc, char* argv[]){
    auto args = argparse::parse<MyArgs>(argc, argv);
    SimpleAmqpExecutor executor = SimpleAmqpExecutor(args);
    bool is_connected = executor.connect();
    if (is_connected){
        if (args.test_type){
            test_immediately_rw(executor);
        }
        else
        {
            test_rw(executor);
        }
    }
    return 0;
}

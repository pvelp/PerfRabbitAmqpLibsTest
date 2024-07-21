#include <deque>
#include <argparse/argparse.hpp>
#include "SimpleAmqpExecutor.hpp"
#include "RabbitmqcExecutor.hpp"

template <typename E>
void run(E executor, const bool& type){
    if (!type)
        {
            std::clog << "Run read/ack/write test" << std::endl;
            executor.test_rw();
        } else {
            std::clog << "Run read/write/ack test" << std::endl;
            executor.test_immediately_rw();
        }
}

int main(int argc, char* argv[]){
    auto args = argparse::parse<MyArgs>(argc, argv);

    if (args.lib_type == "simple"){
        std::clog << "Run SimpleAmqpExecutor" << std::endl;
        SimpleAmqpExecutor executor = SimpleAmqpExecutor(args);
        bool is_connected = executor.connect(); 
        if (is_connected) run(executor, args.test_type);
    }
    else 
    {
        std::clog << "Run RabbitmqExecutor" << std::endl;
        RabbitmqcExecutor executor = RabbitmqcExecutor(args);
        bool is_connected = executor.connect();
        if (is_connected) run(executor, args.test_type);
    }

    return 0;
}

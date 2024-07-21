#include <deque>
#include <argparse/argparse.hpp>
#include "SimpleAmqpExecutor.hpp"
#include "RabbitmqcExecutor.hpp"


int main(int argc, char* argv[]){
    auto args = argparse::parse<MyArgs>(argc, argv);

    if (!args.lib_type){
        SimpleAmqpExecutor executor = SimpleAmqpExecutor(args);
        bool is_connected = executor.connect();

        if (!args.test_type)
        {
            executor.test_rw();
        }

        else
        {
            executor.test_immediately_rw();
        }
    }
    else 
    {
        RabbitmqcExecutor executor = RabbitmqcExecutor(args);
        bool is_connected = executor.connect();

        if (!args.test_type)
        {
            executor.test_rw();
        }
        
        else
        {
            executor.test_immediately_rw();
        }
    }

    return 0;
}

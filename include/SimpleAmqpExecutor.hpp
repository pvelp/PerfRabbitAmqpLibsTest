#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include "args.hpp"

using namespace AmqpClient;

class SimpleAmqpExecutor{
private:
    Channel::OpenOpts opts;

    std::string WRITE_QUEUE;
    std::string READ_QUEUE;

    int message_size;
    int prefetch_message_count;

    std::vector<std::string> messages;

    uint32_t message_count;
    uint32_t consumer_count;
    uint32_t capacity;

    Envelope::ptr_t envelope;
    Channel::ptr_t channel;
    std::string read_consumer_tag;

public:
    SimpleAmqpExecutor(const MyArgs& args);
    bool connect();
    void test_rw();
    void test_immediately_rw();
};
#pragma once

#include <argparse/argparse.hpp>

struct MyArgs : public argparse::Args{
    std::string& host = kwarg("h", "IP address where rabbitmq was deployed").set_default(std::string("localhost"));
    std::string& login = kwarg("l", "Login").set_default(std::string("guest"));
    std::string& pass = kwarg("p", "Password").set_default(std::string("guest"));
    std::string& read_queue_name = kwarg("i", "Откуда читать (read) queue name").set_default(std::string("read_queue"));
    std::string& write_queue_name = kwarg("o", "Куда писать (write) queue name").set_default(std::string("write_queue"));
    int& prefetch_message_count = kwarg("e", "Prefetch message count").set_default(1);
    int& message_capacity = kwarg("c", "Messages in queue to read").set_default(100000);
    int& message_size = kwarg("s", "Size for each message").set_default(5*1024);
    bool& test_type = flag("f", "If used - start test: read message, write message, send ack");
    bool& lib_type = flag("r", "If used - executor form Rabbitmq-c, else SimpleAmqpClient");
};

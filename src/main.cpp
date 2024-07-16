#include <iostream>
#include <deque>
#include <exception>
#include <argparse/argparse.hpp>
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include "profile.hpp"

using namespace AmqpClient;

struct MyArgs : public argparse::Args{
    std::string& host = kwarg("h,host", "IP address where rabbitmq was deployed").set_default(std::string("localhost"));
    std::string& login = kwarg("l,login", "Login").set_default(std::string("rabbitmq"));
    std::string& pass = kwarg("p,password", "Password").set_default(std::string("rabbitmq"));
    std::string& read_queue_name = kwarg("o,output-queue", "Output (read) queue name").set_default(std::string("read_queue"));
    std::string& write_queue_name = kwarg("i,input-queue", "Input (write) queue name").set_default(std::string("write_queue"));
    int& prefetch_message_count = kwarg("e,prefetch-count", "Prefetch message count").set_default(1);
    int& message_capacity = kwarg("c,message-capacity", "Messages in queue to read").set_default(100000);
    int& message_size = kwarg("s,message-size", "Size for each message").set_default(5*1024);
    bool& test_type = flag("f,immediately", "If used - start test: read message, write message, send ack");
};


int main(int argc, char* argv[]){

    auto args = argparse::parse<MyArgs>(argc, argv);
    
    Channel::OpenOpts opts;
    opts.auth = Channel::OpenOpts::BasicAuth(args.login, args.pass);
    opts.host = args.host;

    const std::string WRITE_QUEUE = args.write_queue_name;
    const std::string READ_QUEUE = args.read_queue_name;
    
    const int message_size = args.message_size;
    const int message_capacity = args.message_capacity;
    const int prefetch_message_count = args.prefetch_message_count;

    std::vector<std::string> messages;

    uint32_t message_count = 0;
    uint32_t consumer_count = 0;
    uint32_t capacity;

    Envelope::ptr_t envelope;
    Channel::ptr_t channel;
    std::string read_consumer_tag;

    try
    {
        channel = AmqpClient::Channel::Open(opts);
        channel->DeclareQueue(WRITE_QUEUE, false, true, false, false);
        channel->DeclareQueueWithCounts(READ_QUEUE, message_count, consumer_count, false, true, false, false);

        std::clog << "Message count: " << message_count << std::endl;
        capacity = message_count;
        messages.reserve(message_count);

        read_consumer_tag = channel->BasicConsume(READ_QUEUE, "", true, false, false, prefetch_message_count);
        std::clog << "Consumer tag: " << read_consumer_tag << std::endl;
    } 
    catch (const std::exception& error)
    {
        std::cout << error.what() << std::endl;
        return 1;
    }

    if (args.test_type){
        if (message_count != 0)    
        {
            LOG_DURATION("READ 1 message and immediately write 1 message, size of 1 msg = " 
            + to_string(message_size) + " byte. Total message count = " + to_string(message_count))
            while (message_count != 0){
                envelope = channel->BasicConsumeMessage(read_consumer_tag);
                channel->BasicPublish("", WRITE_QUEUE, envelope->Message(), true, false);
                channel->BasicAck(envelope);
                message_count--;
            }
        }
    }

    else 
    {
        if (message_count != 0)    
        {
            LOG_DURATION("READ" + to_string(message_count) + "messages, size of 1 msg = 1kb")
            while (message_count != 0){
                envelope = channel->BasicConsumeMessage(read_consumer_tag);
                messages.push_back(std::move(envelope->Message()->Body()));
                channel->BasicAck(envelope);
                message_count--;
            }
        }

        if (capacity != 0)
        {
            LOG_DURATION("WRITE" + to_string(message_count) + "messages, size of 1 msg = 1kb")
            for (size_t i = 0; i < capacity; i++){
                channel->BasicPublish("", WRITE_QUEUE, BasicMessage::Create(messages[i]), true, false);
            }
        }
    }
    return 0;
}

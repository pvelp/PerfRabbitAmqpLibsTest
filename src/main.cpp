#include <iostream>
#include <deque>
#include <exception>
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include "profile.hpp"

constexpr auto WRITE_QUEUE = "write_queue";
constexpr auto READ_QUEUE = "read_queue";
constexpr const int block_size = 1024;

using namespace AmqpClient;


int main(){
    Channel::OpenOpts opts;
    opts.auth = Channel::OpenOpts::BasicAuth("rabbitmq", "rabbitmq");
    opts.host = "localhost";
    std::vector<std::string> messages;
    

    boost::uint32_t message_count = 0;
    boost::uint32_t consumer_count = 0;
    uint32_t capacity;

    Envelope::ptr_t envelope;

    try
    {
        Channel::ptr_t channel = AmqpClient::Channel::Open(opts);
        channel->DeclareQueue(WRITE_QUEUE, false, true, false, false);
        channel->DeclareQueueWithCounts(READ_QUEUE, message_count, consumer_count, false, true, false, false);

        std::clog << "Message count: " << message_count << std::endl;
        capacity = message_count;
        messages.reserve(message_count);

        std::string read_consumer_tag = channel->BasicConsume(READ_QUEUE, "", true, false, false);
        std::clog << "Consumer tag: " << read_consumer_tag << std::endl;

    if (message_count != 0)    
    {
        LOG_DURATION("READ 100 000 messages, size of 1 msg = 1kb")
        while (message_count != 0){
            envelope = channel->BasicConsumeMessage(read_consumer_tag);
            messages.push_back(std::move(envelope->Message()->Body()));
            channel->BasicAck(envelope);
            message_count--;
        }
    }
        std::clog << "[x] Current message_count = " << message_count << std::endl;
        std::clog << "[x] Dequeue size " << messages.size() << " and max size " << messages.max_size() << std::endl;
    
    if (capacity != 0)
    {
        LOG_DURATION("WRITE 100 000 messages, size of 1 msg = 1kb")
         for (size_t i = 0; i < capacity; i++){
             channel->BasicPublish("", WRITE_QUEUE, BasicMessage::Create(messages[i]), true, false);
        }
    }

    }
    catch (const std::exception& error) {
        std::cout << error.what() << std::endl;
    }
    return 0;
}
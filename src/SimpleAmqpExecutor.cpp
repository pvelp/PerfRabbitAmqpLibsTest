#include "SimpleAmqpExecutor.hpp"
#include "profile.hpp"

SimpleAmqpExecutor::SimpleAmqpExecutor(const MyArgs& args):
    WRITE_QUEUE(args.write_queue_name),
    READ_QUEUE(args.read_queue_name),
    message_size(args.message_size),
    capacity(args.message_capacity),
    message_count(0),
    consumer_count(0),
    prefetch_message_count(args.prefetch_message_count)
{
    opts.auth = Channel::OpenOpts::BasicAuth(args.login, args.pass);
    opts.host = args.host;
};


bool SimpleAmqpExecutor::connect() {
    try
    {
        channel = AmqpClient::Channel::Open(opts);
        channel->DeclareQueue(WRITE_QUEUE, false, true, false, false);
        channel->DeclareQueueWithCounts(READ_QUEUE, message_count, consumer_count, false, true, false, false);
    
        std::clog << "Message count: " << message_count << std::endl;
        capacity = message_count;
        messages.reserve(message_count);

        read_consumer_tag = channel->BasicConsume(READ_QUEUE, "", true, false, false, prefetch_message_count);
        return true;
    }
    catch (const std::exception& error)
    {
        std::cout << error.what() << std::endl;
        return false;
    }
}


void SimpleAmqpExecutor::test_rw() {
    if (message_count != 0)
    {
        LOG_DURATION("READ " + to_string(capacity) + " messages, size of 1 msg = " + to_string(message_size) + " byte")
        while (message_count != 0){
            envelope = channel->BasicConsumeMessage(read_consumer_tag);
            messages.push_back(std::move(envelope->Message()->Body()));
            channel->BasicAck(envelope);
            message_count--;
        }
    }

    if (capacity != 0)
    {
        LOG_DURATION("WRITE " + to_string(capacity) + " messages, size of 1 msg = " + to_string(message_size) + " byte")
        for (size_t i = 0; i < capacity; i++){
            channel->BasicPublish("", WRITE_QUEUE, BasicMessage::Create(messages[i]), true, false);
        }
    }
}

void SimpleAmqpExecutor::test_immediately_rw() {
    if (message_count != 0)
    {
        LOG_DURATION("READ 1 message and immediately write 1 message, size of 1 msg = "
                     + to_string(message_size) + " byte. Total message count = " + to_string(capacity))
        while (message_count != 0){
            envelope = channel->BasicConsumeMessage(read_consumer_tag);
            channel->BasicPublish("", WRITE_QUEUE, envelope->Message(), true, false);
            channel->BasicAck(envelope);
            message_count--;
        }
    }
}
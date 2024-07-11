#include <iostream>
#include <exception>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

constexpr auto WRITE_QUEUE = "write_queue";
constexpr auto READ_QUEUE = "read_queue";

using namespace AmqpClient;

void test_write(const Channel::ptr_t& ch){
    for (int i = 1; i < 100000; i++){
        auto message = BasicMessage::Create(std::to_string(i));
        ch->BasicPublish("", READ_QUEUE, message, true, false);
    }
}

int main(){
    Channel::OpenOpts opts;
    opts.auth = Channel::OpenOpts::BasicAuth("rabbitmq", "rabbitmq");
    opts.host = "localhost";
    
    try
    {
        Channel::ptr_t channel = AmqpClient::Channel::Open(opts);
        channel->DeclareQueue(WRITE_QUEUE, false, true, false, false);
        channel->DeclareQueue(READ_QUEUE, false, true, false, false);

        test_write(channel);
        std::string read_consumer_tag = channel->BasicConsume(READ_QUEUE, "", true, false, false);
        std::clog << "Consumer tag: " << read_consumer_tag << std::endl;

        while (true){
            Envelope::ptr_t envelope = channel->BasicConsumeMessage(read_consumer_tag);
            std::string message_payload = envelope->Message()->Body();
            std::clog << "[x] Recieved: " << message_payload << std::endl; 
            channel->BasicAck(envelope);
        }
    }
    catch (const std::exception& error) {
        std::cout << error.what() << std::endl;
    }
    return 0;
}
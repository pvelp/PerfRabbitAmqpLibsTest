#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <rabbitmq-c/framing.h>
#include "args.hpp"

class RabbitmqcExecutor{
private:
    std::string hostname;
    int status;
    std::string password;
    std::string login;
    
    char* exchange_read;
    char* exchange_write;
    char* binding_key_read;
    char* binding_key_write;

    amqp_bytes_t write_queue;
    amqp_bytes_t read_queue;
    int port;

    uint32_t message_count;
    uint32_t consumer_count;
    uint32_t capacity;

    int message_size;
    int prefetch_message_count;


    std::vector<amqp_message_t> messages;
    amqp_connection_state_t conn;
    amqp_socket_t *socket = NULL;
    amqp_rpc_reply_t res;
    amqp_envelope_t envelope;
    amqp_basic_properties_t props;

public:
    RabbitmqcExecutor(const MyArgs& args);
    bool connect();
    void test_rw();
    void test_immediately_rw();
};
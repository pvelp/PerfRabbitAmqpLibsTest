#include "RabbitmqcExecutor.hpp"
#include "profile.hpp"
#include <cstring>

RabbitmqcExecutor::RabbitmqcExecutor(const MyArgs& args):
    message_count(0),
    capacity(args.message_capacity),
    consumer_count(0),
    message_size(args.message_size),
    prefetch_message_count(args.prefetch_message_count),
    port(5672),
    hostname(args.host),
    password(args.pass),
    login(args.login),
    exchange_read("direct"),
    exchange_write("amq.direct"),
    binding_key_read("read"),
    binding_key_write("write")
{
    write_queue = amqp_cstring_bytes(args.write_queue_name.c_str());
    read_queue = amqp_cstring_bytes(args.read_queue_name.c_str());
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG |
                   AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.delivery_mode = 2; /* persistent delivery mode */
}


bool RabbitmqcExecutor::connect(){
    conn = amqp_new_connection();

    socket = amqp_tcp_socket_new(conn);
    if (!socket) { 
        std::clog << "Error: creating TCP socket" << std::endl;
        return false;
    }
    
    status = amqp_socket_open(socket, hostname.c_str(), port);
    if (status) {
        std::clog << "Error: opening TCP socket" << std::endl;
        return false;
     }

    res = amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, login.c_str(), password.c_str());
    if (res.reply_type != AMQP_RESPONSE_NORMAL){
        std::clog << "Error: auth [amqp_login]" << std::endl;
        return false;
    }
    
    amqp_channel_open_ok_t *ok = amqp_channel_open(conn, 1);
    res = amqp_get_rpc_reply(conn);
    if (res.reply_type != AMQP_RESPONSE_NORMAL){
        std::clog << "Error: Opening channel" << std::endl;
        return false;
    }

    amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, 1, read_queue, 0, 1, 0, 0, amqp_empty_table);
    message_count = r->message_count;
    std::clog << "Message count: " << message_count << std::endl;
    messages.reserve(message_count);
    consumer_count = r->consumer_count;
    capacity = message_count;
    res = amqp_get_rpc_reply(conn);
    if (res.reply_type != AMQP_RESPONSE_NORMAL){
        std::clog << "Error: Declaring read queue" << std::endl;
        return false;
    }

    amqp_queue_bind(conn, 1, read_queue, amqp_cstring_bytes(exchange_read),
                  amqp_cstring_bytes(binding_key_read), amqp_empty_table);
    if (res.reply_type != AMQP_RESPONSE_NORMAL){
        std::clog << "Error: Binding read queue" << std::endl;
        return false;
    }

    r = amqp_queue_declare(conn, 1, write_queue, 0, 1, 0, 0, amqp_empty_table);
    res = amqp_get_rpc_reply(conn);
    if (res.reply_type != AMQP_RESPONSE_NORMAL){
        std::clog << "Error: Declaring write queue" << std::endl;
        return false;
    }

    amqp_queue_bind(conn, 1, write_queue, amqp_cstring_bytes(exchange_write),
                  amqp_cstring_bytes(binding_key_write), amqp_empty_table);
    res = amqp_get_rpc_reply(conn);
    if (res.reply_type != AMQP_RESPONSE_NORMAL){
        std::clog << "Error: Binding write queue" << std::endl;
        
    }

    amqp_basic_qos(conn, 1, 0, prefetch_message_count, 0);
    if (res.reply_type != AMQP_RESPONSE_NORMAL){
        std::clog << "Error: Qos read queue" << std::endl;
        return false;
    }

    amqp_basic_consume(conn, 1, read_queue, amqp_empty_bytes, 1, 0, 0, amqp_empty_table);
    res = amqp_get_rpc_reply(conn);
    if (res.reply_type != AMQP_RESPONSE_NORMAL){
        std::clog << "Error: Consuming read queue" << std::endl;
        return false;
    }
    return true;
}


void RabbitmqcExecutor::test_rw(){
    if (message_count != 0) {
        LOG_DURATION("READ " + to_string(capacity) + " messages, size of 1 msg = " + to_string(message_size) + " byte")
        {
            while (message_count != 0){
                amqp_maybe_release_buffers(conn);
                res = amqp_consume_message(conn, &envelope, NULL, 0);
                if (res.reply_type != AMQP_RESPONSE_NORMAL) {
                    std::clog << "reply_type = " << res.reply_type << std::endl;
                    std::clog << "library_error = " << res.library_error << std::endl;
                    std::clog << amqp_error_string(res.library_error) << std::endl;
                    break;
                }    
                messages.push_back(envelope.message);
                amqp_basic_ack(conn, 1, envelope.delivery_tag, 0); 
                message_count--;
            }
        }

        amqp_destroy_envelope(&envelope);
    }

    if (capacity != 0)
    {
        LOG_DURATION("WRITE " + to_string(capacity) + " messages, size of 1 msg = " + to_string(message_size) + " byte")
        for (size_t i = 0; i < capacity; i++){
            amqp_basic_publish(conn, 1, amqp_cstring_bytes(exchange_write), amqp_cstring_bytes(binding_key_write), 1, 0, 0, messages[i].body);
        }
    }
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
}

void RabbitmqcExecutor::test_immediately_rw(){
    if (message_count != 0)
    {
        LOG_DURATION("READ 1 message and immediately write 1 message, size of 1 msg = "
                     + to_string(message_size) + " byte. Total message count = " + to_string(capacity))
        while (message_count != 0){
            amqp_maybe_release_buffers(conn);
            res = amqp_consume_message(conn, &envelope, NULL, 0);
            if (res.reply_type != AMQP_RESPONSE_NORMAL) {
                std::clog << res.reply_type << std::endl;
                std::clog << res.library_error << std::endl;
                std::clog << amqp_error_string(res.library_error) << std::endl;
                break;
            }    
            amqp_basic_publish(conn, 1, amqp_cstring_bytes(exchange_write), amqp_cstring_bytes(binding_key_write), 1, 0, 0, envelope.message.body);
            amqp_basic_ack(conn, 1, envelope.delivery_tag, 0); 
            message_count--;
        }
    }
}
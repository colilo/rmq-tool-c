//
// Created by colilo on 16-11-21.
//

#ifndef AMQP_TOOL_CONNECTION_H
#define AMQP_TOOL_CONNECTION_H

#include <cstddef>
#include "amqp.h"
#include "amqp_framing.h"

class Connection {
    amqp_connection_state_t _conn;
    struct amqp_connection_info _ci;
    int heartbeat;

public:
    Connection(char *url, )
    {
        int status;
        amqp_status_enum res;
        amqp_rpc_reply_t ret;
        amqp_channel_open_ok_t method;
        amqp_socket_t *socket = NULL;
        amqp_default_connection_info(&_ci);

        res = amqp_parse_url(url, &_ci);
        if (res != AMQP_STATUS_OK) {
            std::cout << "Parsing URL " << url << "failed" << std::endl;
        }

        _conn = amqp_new_connection();
        if (_ci.ssl) {
#ifdef WITH_SSL
            socket = amqp_ssl_socket_new(_conn);
            if (!socket) {
                std::cout << "Creating SSL/TLS socket failed" << std::endl;
            }
#endif
        } else {
            socket = amqp_tcp_socket_new(_conn);
            if (!socket) {
                std::cout << "Creating TCP socket (out of memory)" << std::endl;
            }
        }

        status = amqp_socket_open(socket, _ci.host, _ci.port);
        if (status) {
            std::cout << "opening socket to " << _ci.host << ":" << _ci.port << std::endl;
        }

        ret = amqp_login(_conn, _ci.vhost, 0, 131072, _heartbead, AMQP_SASL_METHOD_PLAIN, _ci.user, _ci.password);
        if (ret.reply_type != AMQP_RESPONSE_NORMAL) {
            std::cout << "logging in to AMQP server failed" << std::endl;
        }

        method = amqp_channel_open(_conn, 1);
        if (method == NULL) {
            amqp_rpc_reply_t r = amqp_get_rpc_reply(conn);
            std::cerr << "opening channel failed" << r.reply_type << std::endl;
        }
    }


};


#endif //AMQP_TOOL_CONNECTION_H

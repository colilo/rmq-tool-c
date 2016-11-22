//
// Created by colilo on 16-11-15.
//

#ifndef AMQP_TOOL_CONSUMER_H
#define AMQP_TOOL_CONSUMER_H

#include <string>
#include "ProducerConsumerBase.h"
#include "Parameter.h"
#include "Stats.h"

class Consumer : ProducerConsumerBase {
private:
    ConsumerImpl     _q;
    Channel          _channel;
    string           _id;
    string           _queueName;
    int              _txSize;
    bool             _autoAck;
    int              _multiAckEvery;
    Stats            _stats;
    float            _rateLimit;
    int              _msgLimit;
    long             _timeLimit;
    CountDownLatch   _latch = new CountDownLatch(1);

public:
    Consumer(Channel channel, Parameter param, Stats stats) {

        _channel       = channel;
        _id            = param.getRoutingKey();
        _queueName     = param.getQueueName();
        _rateLimit     = param.getConsumerRateLimit();
        _txSize        = param.getConsumerTxSize();
        _autoAck       = param.getAutoAck();
        _multiAckEvery = param.getMultiAckEvery();
        _stats         = stats;
        _msgLimit      = param.getMinMsgSize();
        _timeLimit     = param.getTimeLimit();
        _latch         = new CountDownLatch(1);
    }


};


#endif //AMQP_TOOL_CONSUMER_H

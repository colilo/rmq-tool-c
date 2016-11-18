//
// Created by colilo on 16-11-15.
//

#ifndef AMQP_TOOL_CONSUMER_H
#define AMQP_TOOL_CONSUMER_H

#include <string>
#include "ProducerConsumerBase.h"
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
    Consumer(Channel channel, String id,
                String queueName, int txSize, bool autoAck,
                int multiAckEvery, Stats stats, float rateLimit, int msgLimit, int timeLimit) {

        _channel       = channel;
        _id            = id;
        _queueName     = queueName;
        _rateLimit     = rateLimit;
        _txSize        = txSize;
        _autoAck       = autoAck;
        _multiAckEvery = multiAckEvery;
        _stats         = stats;
        _msgLimit      = msgLimit;
        _timeLimit     = 1000L * timeLimit;
        _latch         = new CountDownLatch(1);
    }


};


#endif //AMQP_TOOL_CONSUMER_H

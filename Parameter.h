//
// Created by colilo on 16-11-16.
//

#ifndef AMQP_TOOL_PARAMETER_H
#define AMQP_TOOL_PARAMETER_H

#include <string>
#include <vector>
#include "Producer.h"
#include "Consumer.h"

class Parameter {
    private:
        long _confirm = -1;
        long _latencyLimitation = 0;
        int _consumerCount = 1;
        int _producerCount = 1;
        int _consumerTxSize = 0;
        int _producerTxSize = 0;
        int _channelPrefetch = 0;
        int _consumerPrefetch = 0;
        int _minMsgSize = 0;
        int _timeLimit = 0;
        float _producerRateLimit = 0;
        float _consumerRateLimit = 0;
        int _producerMsgCount = 0;
        int _consumerMsgCount = 0;
        std::string _exchangeName = "direct";
        std::string _exchangeType = "direct";
        std::string _queueName = "";
        std::string _routingKey = NULL;
        bool _randomRoutingKey = false;
        std::vector<std::string> _flags;
        int _multiAckEvery = 0;
        bool _autoAck = true;
        bool _autoDelete = false;
        bool _predeclared;

    public:
        void setExchangeType(std::string exchangeType);

        void setExchangeName(std::string exchangeName);

        void setQueueName(std::string queueName);

        void setRoutingKey(std::string routingKey);

        void setRandomRoutingKey(bool randomRoutingKey);

        void setProducerRateLimit(float producerRateLimit);

        void setProducerCount(int producerCount);

        void setConsumerRateLimit(float consumerRateLimit);

        void setConsumerCount(int consumerCount);

        void setProducerTxSize(int producerTxSize);

        void setConsumerTxSize(int consumerTxSize);

        void setConfirm(long confirm);

        void setLatencyLimitation(long latencyLimitation);

        void setAutoAck(bool autoAck);

        void setMultiAckEvery(int multiAckEvery);

        void setChannelPrefetch(int channelPrefetch);

        void setConsumerPrefetch(int consumerPrefetch);

        void setMinMsgSize(int minMsgSize);

        void setTimeLimit(int timeLimit);

        void setProducerMsgCount(int producerMsgCount);

        void setConsumerMsgCount(int consumerMsgCount);

        void setMsgCount(int msgCount);

        void setFlags(std::string flag);

        void setAutoDelete(bool autoDelete);

        void setPredeclared(bool predeclared);

        int getConsumerCount();

        int getProducerCount();

        int getMinMsgSize();

        long getLatencyLimitation();

        std::string getRoutingKey();

        bool getRandomRoutingKey();

        Producer createProducer(Connection connection, Stats stats, std::string id) throws IOException;

        Consumer createConsumer(Connection connection, Stats stats, std::string id) throws IOException;

        bool shouldConfigureQueue();

        std::string configureQueue(Connection connection, std::string id) throws IOException;

    private:
        static bool exchangeExists(Connection connection, final std::string exchangeName) throws IOException;

        static bool queueExists(Connection connection, final std::string queueName) throws IOException;

        static interface Checker{
                public void check(Channel ch) throws IOException;
        }

        static bool exists(Connection connection, Checker checker) throws IOException;
};


#endif //AMQP_TOOL_PARAMETER_H

//
// Created by colilo on 16-11-16.
//

#ifndef AMQP_TOOL_PARAMETER_H
#define AMQP_TOOL_PARAMETER_H

#include <string>
#include <vector>
#include <set>
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
        std::set<std::string> _flags;
        int _multiAckEvery = 0;
        bool _autoAck = true;
        bool _autoDelete = false;
        bool _predeclared;

    public:
        void setExchangeType(std::string exchangeType);
        std::string getExchangeType();

        void setExchangeName(std::string exchangeName);
        std::string getExchangeName();

        void setQueueName(std::string queueName);
        std::string getQueueName();

        void setRoutingKey(std::string routingKey);
        std::string getRoutingKey();

        void setRandomRoutingKey(bool randomRoutingKey);
        bool getRandomRoutingKey();

        void setProducerRateLimit(float producerRateLimit);
        float getProducerRateLimit();

        void setProducerCount(int producerCount);
        int getProducerCount();

        void setConsumerRateLimit(float consumerRateLimit);
        float getConsumerRateLimit();

        void setConsumerCount(int consumerCount);
        int getConsumerCount();

        void setProducerTxSize(int producerTxSize);
        int getProducerTxSize();

        void setConsumerTxSize(int consumerTxSize);
        int getConsumerTxSize();

        void setConfirm(long confirm);
        long getConfirm();

        void setLatencyLimitation(long latencyLimitation);
        long getLatencyLimitation();

        void setAutoAck(bool autoAck);
        bool getAutoAck();

        void setMultiAckEvery(int multiAckEvery);
        int getMultiAckEvery();

        void setChannelPrefetch(int channelPrefetch);
        int getChannelPrefetch();

        void setConsumerPrefetch(int consumerPrefetch);
        int getConsumerPrefetch();

        void setMinMsgSize(int minMsgSize);
        int getMinMsgSize();

        void setTimeLimit(int timeLimit);
        int getTimeLimit();

        void setProducerMsgCount(int producerMsgCount);
        int getProducerMsgCount();

        void setConsumerMsgCount(int consumerMsgCount);
        int getConsumerMsgCount();

        void setMsgCount(int msgCount);

        void setFlags(std::string flag);

        bool hasFlag(std::string flag);

        void setAutoDelete(bool autoDelete);
        bool getAutoDelete();

        void setPredeclared(bool predeclared);
        bool getPredeclared();


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

//
// Created by colilo on 16-11-16.
//

#include <string>
#include "Parameter.h"


void Parameter::setExchangeType(std::string exchangeType) {
    _exchangeType = exchangeType;
}

void Parameter::setExchangeName(std::string exchangeName) {
    _exchangeName = exchangeName;
}

void Parameter::setQueueName(std::string queueName) {
    _queueName = queueName;
}

void Parameter::setRoutingKey(std::string routingKey) {
    _routingKey = routingKey;
}

void Parameter::setRandomRoutingKey(bool randomRoutingKey) {
    _randomRoutingKey = randomRoutingKey;
}

void Parameter::setProducerRateLimit(float producerRateLimit) {
    _producerRateLimit = producerRateLimit;
}

void Parameter::setProducerCount(int producerCount) {
    _producerCount = producerCount;
}

void Parameter::setConsumerRateLimit(float consumerRateLimit) {
    _consumerRateLimit = consumerRateLimit;
}

void Parameter::setConsumerCount(int consumerCount) {
    _consumerCount = consumerCount;
}

void Parameter::setProducerTxSize(int producerTxSize) {
    _producerTxSize = producerTxSize;
}

void Parameter::setConsumerTxSize(int consumerTxSize) {
    _consumerTxSize = consumerTxSize;
}

void Parameter::setConfirm(long confirm) {
    _confirm = confirm;
}

void Parameter::setLatencyLimitation(long latencyLimitation) { _latencyLimitation = latencyLimitation; }

void Parameter::setAutoAck(bool autoAck) {
    _autoAck = autoAck;
}

void Parameter::setMultiAckEvery(int multiAckEvery) {
    _multiAckEvery = multiAckEvery;
}

void Parameter::setChannelPrefetch(int channelPrefetch) {
    _channelPrefetch = channelPrefetch;
}

void Parameter::setConsumerPrefetch(int consumerPrefetch) {
    _consumerPrefetch = consumerPrefetch;
}

void Parameter::setMinMsgSize(int minMsgSize) {
    _minMsgSize = minMsgSize;
}

void Parameter::setTimeLimit(int timeLimit) {
    _timeLimit = timeLimit;
}

void Parameter::setProducerMsgCount(int producerMsgCount) {
    _producerMsgCount = producerMsgCount;
}

void Parameter::setConsumerMsgCount(int consumerMsgCount) {
    _consumerMsgCount = consumerMsgCount;
}

void Parameter::setMsgCount(int msgCount) {
    setProducerMsgCount(msgCount);
    setConsumerMsgCount(msgCount);
}

void Parameter::setFlags(std::string flag) {
    _flags.push_back(flag);
}

void Parameter::setAutoDelete(bool autoDelete) {
    _autoDelete = autoDelete;
}

void Parameter::setPredeclared(bool predeclared) {
    _predeclared = predeclared;
}

int Parameter::getConsumerCount() {
    return _consumerCount;
}

int Parameter::getProducerCount() {
    return _producerCount;
}

int Parameter::getMinMsgSize() {
    return _minMsgSize;
}

long Parameter::getLatencyLimitation()
{
    return _latencyLimitation;
}

std::string Parameter::getRoutingKey() {
    return _routingKey;
}

bool Parameter::getRandomRoutingKey() {
    return _randomRoutingKey;
}

Producer Parameter::createProducer(Connection connection, Stats stats, std::string id) throws IOException {
        Channel channel = connection.createChannel();
        if (producerTxSize > 0) channel.txSelect();
        if (confirm >= 0) channel.confirmSelect();
        if (!predeclared || !exchangeExists(connection, exchangeName)) {
            channel.exchangeDeclare(exchangeName, exchangeType);
        }
        final Producer producer = new Producer(channel, exchangeName, id,
        randomRoutingKey, flags, producerTxSize,
        producerRateLimit, producerMsgCount,
        minMsgSize, timeLimit,
        confirm, stats);
        channel.addReturnListener(producer);
        channel.addConfirmListener(producer);
        return producer;
}

Consumer Parameter::createConsumer(Connection connection, Stats stats, std::string id) throws IOException {
        Channel channel = connection.createChannel();
        if (consumerTxSize > 0) channel.txSelect();
        std::string qName = configureQueue(connection, id);
        if (consumerPrefetch > 0) channel.basicQos(consumerPrefetch);
        if (channelPrefetch > 0) channel.basicQos(channelPrefetch, true);
        return new Consumer(channel, id, qName,
        consumerTxSize, autoAck, multiAckEvery,
        stats, consumerRateLimit, consumerMsgCount, timeLimit);
}

bool Parameter::shouldConfigureQueue() {
    return consumerCount == 0 && !queueName.equals("");
}

std::string Parameter::configureQueue(Connection connection, std::string id) throws IOException {
        Channel channel = connection.createChannel();
        if (!predeclared || !exchangeExists(connection, exchangeName)) {
            channel.exchangeDeclare(exchangeName, exchangeType);
        }
        std::string qName = queueName;
        if (!predeclared || !queueExists(connection, queueName)) {
            qName = channel.queueDeclare(queueName,
                                         flags.contains("persistent"),
                                         false, autoDelete,
                                         null).getQueue();
        }
        channel.queueBind(qName, exchangeName, id);
        channel.abort();
        return qName;
}

private:
static bool Parameter::exchangeExists(Connection connection, final std::string exchangeName) throws IOException {
        return exists(connection, new Checker() {
            public void check(Channel ch) throws IOException {
                    ch.exchangeDeclarePassive(exchangeName);
            }
        });
}

static bool Parameter::queueExists(Connection connection, final std::string queueName) throws IOException {
        return queueName != null && exists(connection, new Checker() {
            public void check(Channel ch) throws IOException {
                    ch.queueDeclarePassive(queueName);
            }
        });
}

static interface Checker {
        public void check(Channel ch) throws IOException;
}

static bool Parameter::exists(Connection connection, Checker checker) throws IOException {
        try {
            Channel ch = connection.createChannel();
            checker.check(ch);
            ch.abort();
            return true;
        }
        catch (IOException e) {
            ShutdownSignalException sse = (ShutdownSignalException) e.getCause();
            if (!sse.isHardError()) {
                AMQP.Channel.Close closeMethod = (AMQP.Channel.Close) sse.getReason();
                if (closeMethod.getReplyCode() == AMQP.NOT_FOUND) {
                    return false;
                }
            }
            throw e;
        }
}
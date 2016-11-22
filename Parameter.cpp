//
// Created by colilo on 16-11-16.
//

#include <string>
#include "Parameter.h"
#include "Connection.h"


void Parameter::setExchangeType(std::string exchangeType) {
    _exchangeType = exchangeType;
}

std::string Parameter::getExchangeType() {
    return _exchangeType;
}

void Parameter::setExchangeName(std::string exchangeName) {
    _exchangeName = exchangeName;
}

std::string Parameter::getExchangeName() {
    return _exchangeName;
}

void Parameter::setQueueName(std::string queueName) {
    _queueName = queueName;
}

std::string Parameter::getQueueName() {
    return _queueName;
}

void Parameter::setRoutingKey(std::string routingKey) {
    _routingKey = routingKey;
}

std::string Parameter::getRoutingKey() {
    return _routingKey;
}

void Parameter::setRandomRoutingKey(bool randomRoutingKey) {
    _randomRoutingKey = randomRoutingKey;
}

bool Parameter::getRandomRoutingKey() {
    return _randomRoutingKey;
}

void Parameter::setProducerRateLimit(float producerRateLimit) {
    _producerRateLimit = producerRateLimit;
}

float Parameter::getProducerRateLimit() {
    return _producerRateLimit;
}

void Parameter::setProducerCount(int producerCount) {
    _producerCount = producerCount;
}

int Parameter::getProducerCount() {
    return _producerCount;
}

void Parameter::setConsumerRateLimit(float consumerRateLimit) {
    _consumerRateLimit = consumerRateLimit;
}

float Parameter::getConsumerRateLimit() {
    return _consumerRateLimit;
}

void Parameter::setConsumerCount(int consumerCount) {
    _consumerCount = consumerCount;
}

int Parameter::getConsumerCount() {
    return _consumerCount;
}

void Parameter::setProducerTxSize(int producerTxSize) {
    _producerTxSize = producerTxSize;
}

int Parameter::getProducerTxSize() {
    return _producerTxSize;
}

void Parameter::setConsumerTxSize(int consumerTxSize) {
    _consumerTxSize = consumerTxSize;
}

int Parameter::getConsumerTxSize() {
    return _consumerTxSize;
}

void Parameter::setConfirm(long confirm) {
    _confirm = confirm;
}

long Parameter::getConfirm() {
    return _confirm;
}

void Parameter::setLatencyLimitation(long latencyLimitation) { _latencyLimitation = latencyLimitation; }

long Parameter::getLatencyLimitation() { return _latencyLimitation; }

void Parameter::setAutoAck(bool autoAck) {
    _autoAck = autoAck;
}

bool Parameter::getAutoAck() {
    return _autoAck;
}

void Parameter::setMultiAckEvery(int multiAckEvery) {
    _multiAckEvery = multiAckEvery;
}

int Parameter::getMultiAckEvery() {
    return _multiAckEvery;
}

void Parameter::setChannelPrefetch(int channelPrefetch) {
    _channelPrefetch = channelPrefetch;
}

int Parameter::getChannelPrefetch() {
    return _channelPrefetch;
}

void Parameter::setConsumerPrefetch(int consumerPrefetch) {
    _consumerPrefetch = consumerPrefetch;
}

int Parameter::getConsumerPrefetch() {
    return _consumerPrefetch;
}

void Parameter::setMinMsgSize(int minMsgSize) {
    _minMsgSize = minMsgSize;
}

int Parameter::getMinMsgSize() {
    return _minMsgSize;
}

void Parameter::setTimeLimit(int timeLimit) {
    _timeLimit = timeLimit;
}

int Parameter::getTimeLimit() {
    return _timeLimit;
}

void Parameter::setProducerMsgCount(int producerMsgCount) {
    _producerMsgCount = producerMsgCount;
}

int Parameter::getProducerMsgCount() {
    return _producerMsgCount;
}

void Parameter::setConsumerMsgCount(int consumerMsgCount) {
    _consumerMsgCount = consumerMsgCount;
}

int Parameter::getConsumerMsgCount() {
    return _consumerMsgCount
}

void Parameter::setMsgCount(int msgCount) {
    setProducerMsgCount(msgCount);
    setConsumerMsgCount(msgCount);
}

void Parameter::setFlags(std::string flag) {
    _flags.insert(flag);
}

bool Parameter::hasFlag(std::string flag) {
    return (_flags.count(flag)==1);
}

void Parameter::setAutoDelete(bool autoDelete) {
    _autoDelete = autoDelete;
}

bool Parameter::getAutoDelete() {
    return _autoDelete;
}

void Parameter::setPredeclared(bool predeclared) {
    _predeclared = predeclared;
}

bool Parameter::getPredeclared() {
    return _predeclared;
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
    return _consumerCount == 0 && _queueName != "";
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
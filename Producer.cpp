//
// Created by colilo on 16-11-15.
//

#include <sys/time.h>
#include <sstream>
#include <cstdlib>
#include "Producer.h"
#include "Stats.h"

void Producer::handleReturn(int replyCode,
                  String replyText,
                  String exchange,
                  String routingKey,
                  AMQP.BasicProperties properties,
                  char *body) throw ()
{
    _stats.handleReturn();
}

void Producer::handleAck(long seqNo, boolean multiple) {
    handleAckNack(seqNo, multiple, false);
}

void Producer::handleNack(long seqNo, boolean multiple) {
    handleAckNack(seqNo, multiple, true);
}

void Producer::handleAckNack(long seqNo, boolean multiple, boolean nack)
{
    int numConfirms = 0;
    if (multiple) {
        SortedSet<Long> confirmed = unconfirmedSet.headSet(seqNo + 1);
        numConfirms += confirmed.size();
        confirmed.clear();
    } else {
        unconfirmedSet.remove(seqNo);
        numConfirms = 1;
    }
    if (nack) {
        _stats.handleNack(numConfirms);
    } else {
        _stats.handleConfirm(numConfirms);
    }

    if (confirmPool != null) {
        for (int i = 0; i < numConfirms; ++i) {
            confirmPool.release();
        }
    }

}

void Producer::run()
{
    long now;
    long startTime;
    startTime = now = System.currentTimeMillis();
    lastStatsTime = startTime;
    msgCount = 0;
    int totalMsgCount = 0;

    try {

        while ((timeLimit == 0 || now < startTime + timeLimit) &&
               (msgLimit == 0 || msgCount < msgLimit)) {
            delay(now);
            if (confirmPool != null) {
                confirmPool.acquire();
            }
            createMessage(totalMsgCount);
            publish();
            totalMsgCount++;
            msgCount++;

            if (txSize != 0 && totalMsgCount % txSize == 0) {
                channel.txCommit();
            }
            stats.handleSend();
            now = System.currentTimeMillis();
        }

    } catch (IOException e) {
        throw new RuntimeException(e);
    } catch (InterruptedException e) {
        throw new RuntimeException (e);
    }
}

void Producer::publish() throw ()
{

        unconfirmedSet.add(channel.getNextPublishSeqNo());
        channel.basicPublish(exchangeName, randomRoutingKey ? UUID.randomUUID().toString() : id,
        mandatory, immediate,
        persistent ? MessageProperties.MINIMAL_PERSISTENT_BASIC : MessageProperties.MINIMAL_BASIC,
        _message);
}

char *Producer::createMessage(int sequenceNumber) throw()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long nano = tv.tv_sec * 1000000ul + tv.tv_usec;

    memcpy(_message, (char *)&sequenceNumber, sizeof(sequenceNumber));
    memcpy(_message + sizeof(sequenceNumber), (char *)&nano, sizeof(nano));
}
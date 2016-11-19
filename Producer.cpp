//
// Created by colilo on 16-11-15.
//

#include "Producer.h"

void Producer::handleReturn(int replyCode,
                  String replyText,
                  String exchange,
                  String routingKey,
                  AMQP.BasicProperties properties,
        byte[] body)
throws IOException {
        stats.handleReturn();
}

void Producer::handleAck(long seqNo, boolean multiple) {
    handleAckNack(seqNo, multiple, false);
}

void Producer::handleNack(long seqNo, boolean multiple) {
    handleAckNack(seqNo, multiple, true);
}

void Producer::handleAckNack(long seqNo, boolean multiple,
                   boolean nack) {
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
        stats.handleNack(numConfirms);
    } else {
        stats.handleConfirm(numConfirms);
    }

    if (confirmPool != null) {
        for (int i = 0; i < numConfirms; ++i) {
            confirmPool.release();
        }
    }

}

void Producer::run() {
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
            publish(createMessage(totalMsgCount));
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

void Producer::publish(std::ostringstream msg) throws IOException {

        unconfirmedSet.add(channel.getNextPublishSeqNo());
        channel.basicPublish(exchangeName, randomRoutingKey ? UUID.randomUUID().toString() : id,
        mandatory, immediate,
        persistent ? MessageProperties.MINIMAL_PERSISTENT_BASIC : MessageProperties.MINIMAL_BASIC,
        msg);
}

std::ostringstream Producer::createMessage(int sequenceNumber) throws IOException
{

        ByteArrayOutputStream acc = new ByteArrayOutputStream();
        DataOutputStream d = new DataOutputStream(acc);
        long nano = System.nanoTime();
        d.writeInt(sequenceNumber);
        d.writeLong(nano);
        d.flush();
        acc.flush();
        byte[] m = acc.toByteArray();
        if (m.length <= message.length) {
            System.arraycopy(m, 0, message, 0, m.length);
            return message;
        } else {
            return m;
        }
}
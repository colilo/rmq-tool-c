//
// Created by colilo on 16-11-15.
//

#include "Consumer.h"

void Consumer::run() {
    try {
        q = new ConsumerImpl(channel);
        channel.basicConsume(queueName, autoAck, q);
        if (timeLimit == 0) {
            latch.await();
        }
        else {
            latch.await(timeLimit, TimeUnit.MILLISECONDS);
        }

    } catch (IOException e) {
        throw new RuntimeException(e);
    } catch (InterruptedException e) {
        throw new RuntimeException(e);
    } catch (ShutdownSignalException e) {
        throw new RuntimeException(e);
    }
}

class ConsumerImpl : DefaultConsumer {
long now;
int totalMsgCount = 0;

public:
    ConsumerImpl(Channel channel) {
super(channel);
lastStatsTime = now = System.currentTimeMillis();
msgCount = 0;
}

@Override
public void handleDelivery(String consumerTag, Envelope envelope, BasicProperties properties, byte[] body) throws IOException {
        totalMsgCount++;
        msgCount++;

        if (msgLimit == 0 || msgCount <= msgLimit) {
            DataInputStream d = new DataInputStream(new ByteArrayInputStream(body));
            d.readInt();
            long msgNano = d.readLong();
            long nano = System.nanoTime();

            if (!autoAck) {
                if (multiAckEvery == 0) {
                    channel.basicAck(envelope.getDeliveryTag(), false);
                } else if (totalMsgCount % multiAckEvery == 0) {
                    channel.basicAck(envelope.getDeliveryTag(), true);
                }
            }

            if (txSize != 0 && totalMsgCount % txSize == 0) {
                channel.txCommit();
            }XIA

                    now = System.currentTimeMillis();

            stats.handleRecv(id.equals(envelope.getRoutingKey()) ? (nano - msgNano) : 0L);
            delay(now);
        }
        if (msgLimit != 0 && msgCount >= msgLimit) { // NB: not quite the inverse of above
            latch.countDown();
        }
}

@Override
public void handleShutdownSignal(String consumerTag, ShutdownSignalException sig) {
    latch.countDown();
}

@Override
public void handleCancel(String consumerTag) throws IOException {
        System.out.println("Consumer cancelled by broker. Re-consuming.");
        channel.basicConsume(queueName, autoAck, q);
}
}
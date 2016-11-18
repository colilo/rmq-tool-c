//
// Created by colilo on 16-11-16.
//

#ifndef AMQP_TOOL_PRODUCERCONSUMERBASE_H
#define AMQP_TOOL_PRODUCERCONSUMERBASE_H


class ProducerConsumerBase {
    protected:
        float rateLimit;
        long  lastStatsTime;
        int   msgCount;

    protected:
        void delay(long now);
};


#endif //AMQP_TOOL_PRODUCERCONSUMERBASE_H

//
// Created by colilo on 16-11-15.
//

#ifndef AMQP_TOOL_STATS_H
#define AMQP_TOOL_STATS_H


class Stats {
    protected:
        long    _interval;
        long    _startTime;
        long    _lastStatsTime;
        int     _sendCountInterval;
        int     _returnCountInterval;
        int     _confirmCountInterval;
        int     _nackCountInterval;
        int     _recvCountInterval;
        int     _sendCountTotal;
        int     _recvCountTotal;
        int     _latencyCountInterval;
        int     _latencyCountTotal;
        long    _minLatency;
        long    _maxLatency;
        long    _cumulativeLatencyInterval;
        long    _cumulativeLatencyTotal;
        long    _latencyLimitation;
        long    _acceptableLatencyCountInterval;
        long    _acceptableLatencyCountTotal;
        long    _elapsedInterval;
        long    _elapsedTotal;

    public:
        Stats(long interval, long latencyLimitation) {
            _interval = interval;
            _latencyLimitation = latencyLimitation; // nanoseconds

            struct timeval tv;

            gettimeofday(&tv, NULL);

            _startTime = tv.tv_sec * 1000000ul + tv.tv_usec;
//            _startTime = System.currentTimeMillis();
            reset(_startTime);
        }

    private:
        void reset(long t);

         void report();

    protected:
        virtual void report(long now);

    public:
        void handleSend();

        void handleReturn();

        void handleConfirm(int numConfirms);

        void handleNack(int numAcks);

        void handleRecv(long latency);
    };


#endif //AMQP_TOOL_STATS_H

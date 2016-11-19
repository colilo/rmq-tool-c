//
// Created by colilo on 16-11-15.
//

#include <sys/time.h>
#include <cstddef>
#include <climits>
#include "Stats.h"

void Stats::reset(long t) {
    _lastStatsTime = t;

    _sendCountInterval = 0;
    _returnCountInterval = 0;
    _confirmCountInterval = 0;
    _nackCountInterval = 0;
    _recvCountInterval = 0;

    _minLatency = LONG_MAX;
    _maxLatency = LONG_MIN;
    _latencyCountInterval = 0;
    _cumulativeLatencyInterval = 0L;
    _acceptableLatencyCountInterval = 0L;
}

void Stats::report() {
//    long now = System.currentTimeMillis();
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long now = tv.tv_sec * 1000000ul + tv.tv_usec;

    _elapsedInterval = now - _lastStatsTime;

    if (_elapsedInterval >= _interval) {
        _elapsedTotal += _elapsedInterval;
        report(now);
        reset(now);
    }

}

void Stats::handleSend() {
    _sendCountInterval++;
    _sendCountTotal++;
    report();
}

void Stats::handleReturn() {
    _returnCountInterval++;
    report();
}

void Stats::handleConfirm(int numConfirms) {
    _confirmCountInterval += numConfirms;
    report();
}

void Stats::handleNack(int numAcks) {
    _nackCountInterval += numAcks;
    report();
}

void Stats::handleRecv(long latency) {
    _recvCountInterval++;
    _recvCountTotal++;
    if (latency > 0) {
        _minLatency = _minLatency > latency ? latency : _minLatency;
        _maxLatency = _maxLatency < latency ? latency : _maxLatency;
        _cumulativeLatencyInterval += latency;
        _cumulativeLatencyTotal += latency;
        _latencyCountInterval++;
        _latencyCountTotal++;

        if (latency < _latencyLimitation) {
            _acceptableLatencyCountInterval++;
            _acceptableLatencyCountTotal++;
        }
    }
    report();
}
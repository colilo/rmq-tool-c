#include <iostream>

#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <list>
#include "Parameter.h"
#include "Stats.h"
#include "OptionHelpInfo.def"
#include "Connection.h"

class PrintStats : Stats {
private:
    bool _sendStatsEnabled;
    bool _recvStatsEnabled;
    bool _returnStatsEnabled;
    bool _confirmStatsEnabled;

public:
    PrintStats(long interval, long latencyLimitation, bool sendStatsEnabled, bool recvStatsEnabled,
               bool returnStatsEnabled, bool confirmStatsEnabled) : Stats(interval, latencyLimitation){
        _sendStatsEnabled = sendStatsEnabled;
        _recvStatsEnabled = recvStatsEnabled;
        _returnStatsEnabled = returnStatsEnabled;
        _confirmStatsEnabled = confirmStatsEnabled;
    }


protected:
    void report(long now) {
        std::ostringstream latencyInfoString;
        std::cout << "time: " << std::fixed << std::setprecision(3) << (now - _startTime) / 1000.0 << "s";
//        System.out.print("time: " + String.format("%6.3f", (now - startTime)/1000.0) + "s");

        showRate("sent",      _sendCountInterval,    _sendStatsEnabled,                         _elapsedInterval);
        showRate("returned",  _returnCountInterval,  _sendStatsEnabled && _returnStatsEnabled,  _elapsedInterval);
        showRate("confirmed", _confirmCountInterval, _sendStatsEnabled && _confirmStatsEnabled, _elapsedInterval);
        showRate("nacked",    _nackCountInterval,    _sendStatsEnabled && _confirmStatsEnabled, _elapsedInterval);
        showRate("received",  _recvCountInterval,    _recvStatsEnabled,                         _elapsedInterval);

        if (_latencyCountInterval > 0)
        {
            latencyInfoString << ", min/avg/max latency: " << _minLatency/1000L << "/"
                              << _cumulativeLatencyInterval / (1000L * _latencyCountInterval) << "/"
                              << _maxLatency/1000L << " ms";
        }
        else
        {
            latencyInfoString << "";
        }

        std::cout << latencyInfoString.str();

//        System.out.print((_latencyCountInterval > 0 ?
//                          ", min/avg/max latency: " +
//                          _minLatency/1000L + "/" +
//                          _cumulativeLatencyInterval / (1000L * _latencyCountInterval) + "/" +
//                          _maxLatency/1000L + " ms" :
//                          ""));
        if (_recvStatsEnabled) {
            std::cout << "latency lower than " << _latencyLimitation / 1000L << "ms is "
                      << _acceptableLatencyCountInterval << ", " << _recvCountInterval << " percent "
                      << _acceptableLatencyCountInterval * 100 / _recvCountInterval << "%, average percent"
                      << _acceptableLatencyCountTotal * 100 / _recvCountTotal;
//            System.out.print("latency lower than " + _latencyLimitation / 1000L + "ms is " + _acceptableLatencyCountInterval + ", " + _recvCountInterval + " percent " + _acceptableLatencyCountInterval * 100 / _recvCountInterval + "%, average percent" + _acceptableLatencyCountTotal * 100 / _recvCountTotal);
        }
        std::cout << std::endl;
//        System.out.println();
    }

private:
    void showRate(std::string descr, long count, bool display, long elapsed) {
        if (display)
        {
            std::cout << ", " << descr << ": " << formatRate(1000.0 * count / elapsed) << " msg/s" << std::endl;
//            System.out.print(", " + descr + ": " + formatRate(1000.0 * count / elapsed) + " msg/s");
        }
    }

public:
    void printFinal()
    {

        struct timeval tv;

        gettimeofday(&tv, NULL);

        unsigned long now = tv.tv_sec * 1000000ul + tv.tv_usec;

        std::cout << "sending rate avg: " << formatRate(_sendCountTotal * 1000.0 / (now - _startTime)) << " msg/s" << std::endl;

        unsigned long elapsed = now - _startTime;

        if (elapsed > 0)
        {
            std::cout << "recving rate avg: " << formatRate(_recvCountTotal * 1000.0 / elapsed) << " msg/s" << std::endl;
        }

//        long now =
//
//        long now = System.currentTimeMillis();
//
//        System.out.println("sending rate avg: " +
//                           formatRate(_sendCountTotal * 1000.0 / (now - _startTime)) +
//                           " msg/s");
//
//        long elapsed = now - _startTime;
//        if (elapsed > 0) {
//            System.out.println("recving rate avg: " +
//                               formatRate(_recvCountTotal * 1000.0 / elapsed) +
//                               " msg/s");
//        }
    }

private:
    static std::string formatRate(double rate) {
        std::ostringstream outstringstream;

        if (rate == 0.0)
            outstringstream << (long)rate;
        else if (rate < 1)
            outstringstream <<std::fixed << std::setprecision(2) << rate;
        else if (rate < 10)
            outstringstream << std::fixed << std::setprecision(1) << rate;
        else
            outstringstream << (long)rate;

        return outstringstream.str();

//        if (rate == 0.0)    return String.format("%d", (long)rate);
//        else if (rate < 1)  return String.format("%1.2f", rate);
//        else if (rate < 10) return String.format("%1.1f", rate);
//        else                return String.format("%d", (long)rate);
    }
};

void usage() {

    char optarg;
    std::cout << "usage: <program>" << std::endl;
    for(optarg = 0; optarg < sizeof(optionHelpInfo); optarg++)
    {
        std::ostringstream outstringstream;
        outstringstream << " -" << char(optarg + '?') << " <arg>";
        std::cout << std::setw(15) << std::left
                  << outstringstream.str() << optionHelpInfo[optarg]
                  << std::endl;
    }

}

int main(int argc, char *argv[])
{

    int optChar;
    int frameMax = 0;
    int heartbeat = 0;
    int samplingInterval = 1;
    std::string curi = "amqp://localhost";
    std::string puri = "amqp://localhost";
    Parameter param;

    while ((optChar = getopt(argc, argv, "t:e:u:k:Ki:r:R:x:y:m:n:c:l:aA:Q:q:s:z:C:D:f:M:b:ph:H:")) != -1)
    {
        switch (optChar)
        {
            case 't':
                param.setExchangeType(std::string(optarg));
                break;
            case 'e':
                param.setExchangeName(std::string(optarg));
                break;
            case 'u':
                param.setQueueName(std::string(optarg));
                break;
            case 'k':
                param.setRoutingKey(std::string(optarg));
                break;
            case 'K':
                param.setRandomRoutingKey(true);
                break;
            case 'i':
                samplingInterval = std::strtol(std::string(optarg).c_str(), 0, 10);
                break;
            case 'r':
                param.setProducerRateLimit(std::strtod(std::string(optarg).c_str(), 0));
                break;
            case 'R':
                param.setConsumerRateLimit(std::strtod(std::string(optarg).c_str(), 0));
                break;
            case 'x':
                param.setProducerCount(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 'y':
                param.setConsumerCount(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 'm':
                param.setProducerTxSize(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 'n':
                param.setConsumerTxSize(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 'c':
                param.setConfirm(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 'l':
                param.setLatencyLimitation(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 'a':
                param.setAutoAck(true);
                break;
            case 'A':
                param.setMultiAckEvery(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 'Q':
                param.setChannelPrefetch(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 'q':
                param.setConsumerPrefetch(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 's':
                param.setMinMsgSize(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 'z':
                param.setTimeLimit(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 'C':
                param.setProducerMsgCount(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 'D':
                param.setConsumerMsgCount(std::strtol(std::string(optarg).c_str(), 0, 10));
                break;
            case 'f':
                param.setFlags(std::string(optarg));
                break;
            case 'M':
                frameMax = std::strtol(std::string(optarg).c_str(), 0, 10);
                break;
            case 'b':
                heartbeat = std::strtol(std::string(optarg).c_str(), 0, 10);
                break;
            case 'p':
                param.setPredeclared(true);
                break;
            case 'h':
                curi = std::string(optarg);
                break;
            case 'H':
                puri = std::string(optarg);
                break;
            case '?':
                if (optopt != '?')
                {
                    std::cout << "Options -" << optopt << " should speify " << optionHelpInfo[optopt - 'A'] << std::endl;
                }
                else
                {
                    usage();
                }

                break;
            default:
                abort();
        }
    }

    PrintStats stats = PrintStats(samplingInterval, param.getLatencyLimitation(), param.getProducerCount() > 0,
                                  param.getConsumerCount() > 0, (param.hasFlag(std::string("mandatory")) || param.hasFlag(std::string("immediate"))),
                                  (param.getConfirm() != -1));
    pthread_t ctid[1000];
    pthread_t ptid[1000];
    std::vector<Connection> cConnectionList;
    std::vector<Connection> pConnectionList;
    for(int i = 0; i < param.getConsumerCount(); i++) {
        Connection conn = new Connection(curi, );
        cConnectionList.push_back(conn);
        Consumer c = Consumer(channel, param, stats);
        pthread_create(ctid[i], 0, func, (void *)&conn);
    }

    for(int i = 0; i < param.getProducerCount(); i++) {
        Connection conn = new Connection(puri, );
        pConnectionList.push_back(conn);
        Producer p = Producer(channel, param, stats);
        pthread_create(ptid[i], 0, func, (void *)&conn);
    }


    for(int i = 0; i < param.getConsumerCount(); i++) {
        pthread_join(ctid[i], cret);
        cConnectionList.
    }

    for(int i = 0; i < param.getProducerCount(); i++) {
        pthread_join(ptid[i], pret);
    }






    return 0;
}
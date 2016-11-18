//
// Created by colilo on 16-11-16.
//

#include <unistd.h>
#include "ProducerConsumerBase.h"

void ProducerConsumerBase::delay(long now)
{
    long elapsed = now - lastStatsTime;
    //example: rateLimit is 5000 msg/s,
    //10 ms have elapsed, we have sent 200 messages
    //the 200 msgs we have actually sent should have taken us
    //200 * 1000 / 5000 = 40 ms. So we pause for 40ms - 10ms
    long pause = (long) (rateLimit == 0.0f ? 0.0f : (msgCount * 1000.0 / rateLimit - elapsed));
    if (pause > 0)
    {
        usleep(pause);
    }
}
}
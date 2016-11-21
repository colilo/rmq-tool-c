cmake_minimum_required(VERSION 2.6)
project(amqp_tool)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_library(RABBITMQ_LIBRARY
             NAMES rabbitmq)
find_path(RABBITMQ_INCLUDES include)
# set(SOURCE_FILES main.cpp Producer.cpp Producer.h Consumer.cpp Consumer.h Stats.cpp Stats.h Parameter.cpp Parameter.h ProducerConsumerBase.cpp ProducerConsumerBase.h)
set(SOURCE_FILES main.cpp Stats.cpp Stats.h Parameter.cpp Parameter.h Connection.cpp Connection.h)
add_executable(amqp_tool ${SOURCE_FILES} Connection.cpp Connection.h)
# target_link_libraries(amqp_tool librabbitmq/librabbitmq.so.4.2.0)

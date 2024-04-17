#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <string>
#include <functional>
#include <queue>
#include "logger.hpp"

class RawChannel
{
public:
    virtual bool push(std::string serial_data) = 0;
    virtual std::string pop() = 0;
};

template <class T>
class Channel : public RawChannel
{
public:
    struct config
    {
        std::function<T(std::string)> m_serializer;
        std::function<std::string(T)> m_deserializer;
    };

private:
    config m_config;
    std::queue<T> m_queue;

public:
    Channel(config config);
    virtual bool push(std::string serial_data);
    virtual std::string pop();
};

template <class T>
Channel<T>::Channel(config config)
{
    m_config.m_serializer = config.m_serializer;
    m_config.m_deserializer = config.m_deserializer;
}

template <class T>
bool Channel<T>::push(std::string serial_data)
{
    try
    {
        m_queue.push(m_config.m_deserializer(serial_data));
        return true;
    }
    catch(const std::exception& e)
    {
        LOG(ERROR, "Failed to push to channel");
        return false;
    }
    
}

template <class T>
std::string Channel<T>::pop()
{
    std::string ret = m_config.m_serializer(m_queue.front());
    m_queue.pop();
    return ret;
}

#endif
#include "channel.hpp"
#include "logger.hpp"

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
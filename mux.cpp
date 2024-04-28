#include "mux.hpp"

Mux::Mux(const config &config)
{
    m_config.m_input_channels = config.m_input_channels;
    m_config.m_output_channels = config.m_output_channels;
}

bool Mux::poll()
{
    bool ret = false;
    for (auto &it: m_config.m_input_channels)
    {
        ret = ret | it->poll();
        if (ret)
        {
            break;
        }
    }
    return ret;
}

void Mux::output()
{
    for (auto &it: m_config.m_input_channels)
    {
        if (it->poll())
        {
            std::string output = it->pop();
            for (auto &ot: m_config.m_output_channels)
            {
                ot->push(output);
            }
            break;
        }
    }
}
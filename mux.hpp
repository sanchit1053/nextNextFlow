#ifndef _MUX_H_
#define _MUX_H_

#include "channel.hpp"
#include <vector>
#include <string>

class Mux
{
public:
    struct config
    {
        std::vector<RawChannel *> m_input_channels;
        std::vector<RawChannel *> m_output_channels;
    };
private:
    config m_config;

public:
    Mux(const config &config);
    bool poll();
    void output();
};

#endif
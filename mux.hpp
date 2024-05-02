#ifndef _MUX_H_
#define _MUX_H_

#include "channel.hpp"
#include <vector>

/**
 * @class Mux
 * @brief Class that directly connects channels with each other without any process in between, Moves data from all input channels to all output channels
 *
 */
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

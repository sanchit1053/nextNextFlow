#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include "channel.hpp"
#include <vector>
#include <string>
#include <memory>

/**
 * The script of the container must treat the input and output as files
 * 
*/
class Container
{
public:
    struct config
    {
        std::string m_image_name;
        std::string m_name;
        std::vector<std::string> m_input_names;
        std::string m_output_name;
        std::string m_script;
        std::vector<RawChannel *> m_input_channels;
        RawChannel *m_output_channel;
    };

private:
    config m_config;

public:
    Container(const config &config);
    void run();
    ~Container();
};

#endif
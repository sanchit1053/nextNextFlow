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
        std::vector<std::string> m_output_names;
        std::string m_script;
        std::vector<RawChannel *> m_input_channels;
        std::vector<RawChannel *> m_output_channels;
        std::unordered_map<pid_t, Container *> *m_output_mapping;
    };

private:
    config m_config;
    bool m_output_ready;

public:
    Container(const config &config);
    bool poll();
    bool poll_output();
    pid_t run();
    void output_ready();
    void output();
    ~Container();
};

#endif
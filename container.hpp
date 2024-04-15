#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include "channel.hpp"
#include <vector>
#include <string>

/**
 * The script of the container must treat the input and output as files
 * 
*/
class Container
{
private:
    std::string m_image_name;
    std::string m_name;
    std::vector<std::string> m_input_names;
    std::string m_output_name;
    std::string m_script;

public:
    Container(const std::string &image_name, const std::string &name, const std::vector<std::string> &input_names, const std::string &output_name, const std::string &script);
    void run(std::vector<Channel> &input_channels, Channel &output_channel);
    ~Container();
};

#endif
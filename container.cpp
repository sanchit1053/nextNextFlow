#include "container.hpp"
#include "logger.hpp"
#include <fmt/core.h>
#include <stdlib.h>
#include <fstream>
#include <filesystem>

Container::Container(const config &config)
{
    m_config.m_image_name = config.m_image_name;
    m_config.m_name = config.m_name;
    m_config.m_input_names = config.m_input_names;
    m_config.m_output_name = config.m_output_name;
    m_config.m_script = config.m_script;
    m_config.m_input_channels = config.m_input_channels;
    m_config.m_output_channel = config.m_output_channel;
    if (m_config.m_input_channels.size() != m_config.m_input_names.size())
    {
        LOG(ERROR, "Incorrect input length");
    }
    std::string f = fmt::format("docker run --name {0} -d {1}", m_config.m_name, m_config.m_image_name);
    LOG(DEBUG, f);
    system(f.c_str());
}

void Container::run()
{
    for (int i = 0; i < (int)m_config.m_input_names.size(); i++)
    {
        std::string data = m_config.m_input_channels[i]->pop();
        std::ofstream fout;
        fout.open("temp");
        fout << data;
        fout.close();
        system(fmt::format("docker cp temp {}:/{}", m_config.m_name, m_config.m_input_names[i]).c_str());
    }
    
    std::ofstream fout;
    fout.open("temp");
    fout.close();
    system(fmt::format("docker cp temp {}:/{}", m_config.m_name, m_config.m_output_name).c_str());
    
    fout.open("temp");
    fout << m_config.m_script;
    fout.close();
    system(fmt::format("docker cp temp {}:/script.sh", m_config.m_name).c_str());
    
    std::filesystem::remove("temp");

    system(fmt::format("docker exec {} chmod 755 /script.sh", m_config.m_name).c_str());
    system(fmt::format("docker exec {} /script.sh", m_config.m_name).c_str());
}

Container::~Container()
{
    system(fmt::format("docker stop {}", m_config.m_name).c_str());
    system(fmt::format("docker rm {}", m_config.m_name).c_str());
}

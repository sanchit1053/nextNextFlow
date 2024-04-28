#include "container.hpp"
#include "logger.hpp"
#include <fmt/core.h>
#include <stdlib.h>
#include <unistd.h>
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
    m_config.m_output_mapping = config.m_output_mapping;
    if (m_config.m_input_channels.size() != m_config.m_input_names.size())
    {
        LOG(ERROR, "Incorrect input length");
    }
    m_output_ready = false;
    std::string f = fmt::format("docker run --name {0} -d {1}", m_config.m_name, m_config.m_image_name);
    LOG(DEBUG, f);
    system(f.c_str());
}

bool Container::poll()
{
    bool ret = true;
    for (auto &it : m_config.m_input_channels)
    {
        ret = ret & it->poll();
        if (!ret)
        {
            break;
        }
    }
    return ret;
}

bool Container::poll_output()
{
    return m_output_ready;
}

pid_t Container::run()
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
    pid_t child = fork();
    if (child == 0)
    {
        system(fmt::format("docker exec {} /script.sh", m_config.m_name).c_str());
        exit(0);
    }
    else
    {
        (*m_config.m_output_mapping)[child] = this;
    }
    return child;
}

void Container::output_ready()
{
    m_output_ready = true;
}

void Container::output()
{
    system(fmt::format("docker cp {}:{} scratch/", m_config.m_name, m_config.m_output_name).c_str());

    std::ifstream fin;
    fin.open(fmt::format("scratch/{}",m_config.m_output_name));
    std::stringstream data;
    data << fin.rdbuf();
    if (!m_config.m_output_channel->push(data.str()))
    {
        LOG(ERROR, "Failed to push data");
    }
    fin.close();
    std::filesystem::remove(fmt::format("scratch/{}",m_config.m_output_name));
    m_output_ready = false;
}

Container::~Container()
{
    system(fmt::format("docker stop {}", m_config.m_name).c_str());
    system(fmt::format("docker rm {}", m_config.m_name).c_str());
}

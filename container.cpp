#include "container.hpp"
#include "logger.hpp"
#include <fmt/core.h>
#include <stdlib.h>
#include <fstream>
#include <filesystem>

Container::Container(const std::string &image_name, const std::string &name, const std::vector<std::string> &input_names, const std::string &output_name, const std::string &script):
m_image_name(image_name),
m_name(name)
{
    m_input_names = input_names;
    m_output_name = output_name;
    m_script = script;
    std::string f = fmt::format("docker run --name {0} -d {1}", m_name, m_image_name);
    LOG(DEBUG, f);
    system(f.c_str());
}

void Container::run(std::vector<Channel> &input_channels, Channel &output_channel)
{
    if (input_channels.size() != m_input_names.size())
    {
        LOG(ERROR, "Incorrect input length");
    }
    for (int i = 0; i < (int)m_input_names.size(); i++)
    {
        std::string data = input_channels[i].get_data();
        std::ofstream fout;
        fout.open("temp");
        fout << data;
        fout.close();
        system(fmt::format("docker cp temp {}:/{}", m_name, m_input_names[i]).c_str());
    }
    
    std::ofstream fout;
    fout.open("temp");
    fout.close();
    system(fmt::format("docker cp temp {}:/{}", m_name, m_output_name).c_str());
    
    fout.open("temp");
    fout << m_script;
    fout.close();
    system(fmt::format("docker cp temp {}:/script.sh", m_name).c_str());
    
    std::filesystem::remove("temp");

    system(fmt::format("docker exec {} chmod 755 /script.sh", m_name).c_str());
    system(fmt::format("docker exec {} /script.sh", m_name).c_str());
}

Container::~Container()
{
    system(fmt::format("docker stop {}", m_name).c_str());
    system(fmt::format("docker rm {}", m_name).c_str());
}

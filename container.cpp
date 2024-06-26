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
    m_config.m_output_names = config.m_output_names;
    m_config.m_script = config.m_script;
    m_config.m_input_channels = config.m_input_channels;
    m_config.m_output_channels = config.m_output_channels;
    m_config.m_output_mapping = config.m_output_mapping;
    if (m_config.m_input_channels.size() != m_config.m_input_names.size())
    {
        LOG(ERROR, "Incorrect input length");
    }
    m_output_ready = false;
    std::ofstream fout;
    fout.open(fmt::format("storage/{0}", m_config.m_name));
    fout.close();
	
	LOG(DEBUG, fmt::format("Created container file {}", m_config.m_name));
	// run docker with mount point given and specify the name of container and image used
    std::string f = fmt::format("docker run --mount type=bind,source=\"{0}/storage/{1}\",target=/disk --name {1} -d {2}", std::filesystem::current_path().string(), m_config.m_name, m_config.m_image_name);
    LOG(DEBUG, f);
    system(f.c_str());
}

/**
 * @brief Check if container is available to be run (all inputs channels are active)
 *
 */
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

/**
 * @brief check if output ready
 *
 */
bool Container::poll_output()
{
    return m_output_ready;
}

pid_t Container::run()
{
	// copy the input channels into the container
    for (int i = 0; i < (int)m_config.m_input_names.size(); i++)
    {
        std::string data = m_config.m_input_channels[i]->pop();
        std::ofstream fout;
        fout.open("temp");
        fout << data;
        fout.close();
        system(fmt::format("docker cp temp {}:/{}", m_config.m_name, m_config.m_input_names[i]).c_str());
    }

	// copy the output channels into the container
    for (int i = 0; i < (int)m_config.m_output_names.size(); i++)
    {
        std::ofstream fout;
        fout.open("temp");
        fout.close();
        system(fmt::format("docker cp temp {}:/{}", m_config.m_name, m_config.m_output_names[i]).c_str());
    }

	// move the workflow (script to run) into docker
    std::ofstream fout;
    fout.open("temp");
    fout << m_config.m_script;
    fout.close();
    system(fmt::format("docker cp temp {}:/script.sh", m_config.m_name).c_str());

    std::filesystem::remove("temp");

    system(fmt::format("docker exec {} chmod 755 /script.sh", m_config.m_name).c_str());
    pid_t child = fork();
    if (child == 0)
    {
		// execute the script in another thread
        system(fmt::format("docker exec {} /script.sh", m_config.m_name).c_str());
        exit(0);
    }
    else
    {
        (*m_config.m_output_mapping)[child] = this;
    }
    return child;
}

// script is finished running
void Container::output_ready()
{
    m_output_ready = true;
}

// put the data in docker to the output channels
void Container::output()
{
    for (int i = 0; i < (int)m_config.m_output_names.size(); i++)
    {
        system(fmt::format("docker cp {}:{} scratch/", m_config.m_name, m_config.m_output_names[i]).c_str());

        std::ifstream fin;
        fin.open(fmt::format("scratch/{}", m_config.m_output_names[i]));
        std::stringstream data;
        data << fin.rdbuf();
        if (data.str().size() > 0)
        {
            if (!m_config.m_output_channels[i]->push(data.str()))
            {
                LOG(ERROR, "Failed to push data");
            }
        }
        fin.close();
        std::filesystem::remove(fmt::format("scratch/{}", m_config.m_output_names[i]));
    }
    m_output_ready = false;
}

// clean the doker by stopping and remove the container
Container::~Container()
{
    system(fmt::format("docker stop {}", m_config.m_name).c_str());
    system(fmt::format("docker rm {}", m_config.m_name).c_str());
}

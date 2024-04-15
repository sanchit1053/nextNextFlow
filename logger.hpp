#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>

#define DEBUG "DEBUG"
#define INFO "INFO"
#define WARN "WARN"
#define ERROR "ERROR"

#define LOG(level, msg) std::cerr << "[" << level << "]" << msg << std::endl

#endif
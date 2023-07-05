#pragma once
#include <string>
void error(const char* _message);
void error_at(const char* _raw_input, char *_location, std::string _fmt, ...);
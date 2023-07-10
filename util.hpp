#pragma once
#include <string>
void error(const char* _message);
void error_at(const char* _raw_input, char *_location, const char *_fmt, ...);
std::string get_TK_type(int _code);
std::string get_ND_type(int _code);
int print_node(Node *_node, int _id, std::ofstream *_file);
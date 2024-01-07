#pragma once

#include <string>

bool filter(const std::string &filepath);
void copy_file(const std::string &source, const std::string &destination);
void copy_directory(const std::string &source, const std::string &destination);
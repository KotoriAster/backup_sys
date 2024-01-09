#pragma once

#include "LZ77.hpp"
#include <iostream>
#include <limits.h>
#include <map>
#include <regex>
#include <string>
#include <sys/stat.h>
#include <vector>

#define reg_file 1
#define dir_file 2
#define sym_file 4
#define blk_file 8
#define chr_file 16
#define fifo_file 32
#define all_file 63

const std::map<char, int> flag_map = {{'r', reg_file}, {'d', dir_file},
                                      {'s', sym_file}, {'b', blk_file},
                                      {'c', chr_file}, {'f', fifo_file}};

struct file_filter {

  __off_t file_size_limit = LONG_MAX;
  int file_type_limit = all_file;
  std::regex name_pattern = std::regex(".*");
  struct file_time_bound {
    time_t start_time = 0;
    time_t end_time = LONG_MAX;
  } file_time_limit;

  inline bool filter(const std::string &filepath) const {
    // Add your filtering logic here. For example, to only copy files that are
    // smaller than a certain size:
    struct stat st;
    if (lstat(filepath.c_str(), &st))
      return false;

    if (st.st_size >= file_size_limit)
      return false;

    if (st.st_mtime < file_time_limit.start_time ||
        st.st_mtime > file_time_limit.end_time)
      return false;

    if (!std::regex_match(filepath, name_pattern))
      return false;

    if (file_type_limit & reg_file && S_ISREG(st.st_mode))
      return true;

    if (file_type_limit & dir_file && S_ISDIR(st.st_mode))
      return true;

    if (file_type_limit & sym_file && S_ISLNK(st.st_mode))
      return true;

    if (file_type_limit & blk_file && S_ISBLK(st.st_mode))
      return true;

    if (file_type_limit & chr_file && S_ISCHR(st.st_mode))
      return true;

    if (file_type_limit & fifo_file && S_ISFIFO(st.st_mode))
      return true;

    return false;
  }
};

class file_backuper {

private:
  struct copy_context {
    std::map<ino_t, std::string> copied_inodes;
    std::map<std::string, std::string> copied_files;
  } ctx;

public:
  compressor_lz77 fb_compressor;
  file_backuper() {}
  file_backuper(compressor_lz77 compressor) : fb_compressor(compressor) {}
  void copy_file(const std::string &source, const std::string &destination);
  void copy_directory(const std::string &source, const std::string &destination,
                      const file_filter &para_filter);
};

inline void set_args(file_backuper &fb, file_filter &filter,
                     std::vector<std::string> &args) {
  if (!(args[2] == "-") && std::regex_match(args[2], std::regex("^[0-9]+$"))) {
    filter.file_size_limit = std::stol(args[2]);
  }

  if (!(args[3] == "-") &&
      std::regex_match(args[3], std::regex("^[rdsbcf-]+$"))) {
    filter.file_type_limit = 0;
    for (char &c : args[3]) {
      if (flag_map.count(c)) {
        filter.file_type_limit |= flag_map.at(c);
      }
    }
  }

  if (!(args[4] == "-")) {
    filter.name_pattern = std::regex(args[4]);
  }

  if (!(args[5] == "-") && !(args[6] == "-")) {
    struct tm start_time, end_time;
    std::string start_time_string = args[5];
    std::string end_time_string = args[6];
    if (!strptime(start_time_string.c_str(), "%Y-%m-%d-%H:%M:%S", &start_time))
      std::cerr << "Error parsing start time\n";
    if (!strptime(end_time_string.c_str(), "%Y-%m-%d-%H:%M:%S", &end_time))
      std::cerr << "Error parsing end time\n";
    filter.file_time_limit = {mktime(&start_time), mktime(&end_time)};
  }

  compressor_lz77 compressor;
  if (!(args[7] == "-")) {
    compressor.compression_filter = std::regex(args[7]);
  }

  if (!(args[8] == "-") && std::regex_match(args[8], std::regex("^[0-9]+$"))) {
    compressor.window_size = std::stoi(args[8]);
  }

  if (!(args[9] == "-") && std::regex_match(args[9], std::regex("^[0-9]+$"))) {
    compressor.compress = std::stoi(args[9]);
  }

  fb.fb_compressor = compressor;
}

inline void args_printer(std::vector<std::string> &args) {
  std::cout << "args: ";
  std::cout << "\t source file: " << args[0]
            << "\t destination file: " << args[1];
  std::cout << "\n\t filter: ";
  std::cout << "\n\t\t size: " << args[2];
  std::cout << "\n\t\t type: " << args[3];
  std::cout << "\n\t\t pattern: " << args[4];
  std::cout << "\n\t\t start_time: " << args[5];
  std::cout << "\n\t\t end_time: " << args[6];
  std::cout << "\n\t compression: ";
  std::cout << "\n\t\t pattern: " << args[7];
  std::cout << "\n\t\t windows size: " << args[8];
  std::cout << "\n\t\t IsCompress: " << args[9];
}
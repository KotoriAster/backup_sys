#pragma once

#include <map>
#include <string>
#include <sys/stat.h>

#define reg_file 1
#define dir_file 2
#define sym_file 4
#define blk_file 8
#define chr_file 16

struct file_filter {

  int file_size_limit;
  int file_type_limit;
  int file_time_limit;

  inline bool filter(const std::string &filepath) const {
    // Add your filtering logic here. For example, to only copy files that are
    // smaller than a certain size:
    struct stat st;
    if (stat(filepath.c_str(), &st))
      return false;

    if (st.st_size >= file_size_limit)
      return false;

    if (file_type_limit & reg_file && !S_ISREG(st.st_mode))
      return false;

    if (file_type_limit & dir_file && !S_ISDIR(st.st_mode))
      return false;

    if (file_type_limit & sym_file && !S_ISLNK(st.st_mode))
      return false;

    if (file_type_limit & blk_file && !S_ISBLK(st.st_mode))
      return false;

    if (file_type_limit & chr_file && !S_ISCHR(st.st_mode))
      return false;

    return true;
  }
};

class file_backuper {

private:
  struct copy_context {
    std::map<ino_t, std::string> copied_inodes;
    std::map<std::string, std::string> copied_files;
  } ctx;

public:
  void copy_file(const std::string &source, const std::string &destination);
  void copy_directory(const std::string &source, const std::string &destination,
                      const file_filter &para_filter);
};
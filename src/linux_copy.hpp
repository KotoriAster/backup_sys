#pragma once

#include <limits.h>
#include <map>
#include <string>
#include <sys/stat.h>

#define reg_file 1
#define dir_file 2
#define sym_file 4
#define blk_file 8
#define chr_file 16
#define fifo_file 32
#define all_file 63

struct file_filter {

  int file_size_limit;
  int file_type_limit = all_file;
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
  void copy_file(const std::string &source, const std::string &destination);
  void copy_directory(const std::string &source, const std::string &destination,
                      const file_filter &para_filter);
};
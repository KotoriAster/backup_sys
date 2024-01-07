#include "linux_copy.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

void file_backuper::copy_file(const std::string &source,
                              const std::string &destination) {
  struct stat st;
  if (stat(source.c_str(), &st) != 0) {
    std::cerr << "Error stating file\n";
    return;
  }
  /*
  if (S_ISLNK(st.st_mode)) {
    // This is a symbolic link, so copy it as is
    char buffer[8192];
    ssize_t bytes = readlink(source.c_str(), buffer, sizeof(buffer));
    if (bytes == -1) {
      std::cerr << "Error reading symbolic link\n";
      return;
    }
    buffer[bytes] = '\0';
    std::string s_buffer = buffer;
    symlink(buffer, destination.c_str());
    return;
  }
  */

  auto it = ctx.copied_inodes.find(st.st_ino);
  if (it != ctx.copied_inodes.end()) {
    // This file has already been copied, so create a hard link
    if (link(it->second.c_str(), destination.c_str()) == -1) {
      std::cerr << "Error creating hard link\n";
    }
  } else {
    // This file has not been copied yet, so copy it and add it to the map
    char buffer[8192];
    int source_fd = open(source.c_str(), O_RDONLY);
    int dest_fd =
        open(destination.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if (source_fd < 0 || dest_fd < 0) {
      std::cerr << "Error opening file\n";
      return;
    }

    ssize_t bytes;
    while ((bytes = read(source_fd, buffer, sizeof(buffer))) > 0) {
      write(dest_fd, buffer, bytes);
    }

    close(source_fd);
    close(dest_fd);

    ctx.copied_inodes[st.st_ino] = destination;
  }
}

void file_backuper::copy_directory(const std::string &source,
                                   const std::string &destination,
                                   const file_filter &para_filter) {
  DIR *dir = opendir(source.c_str());
  if (dir) {
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_DIR) {
        if (std::string(entry->d_name) != "." &&
            std::string(entry->d_name) != "..") {
          std::string new_source = source + "/" + entry->d_name;
          std::string new_destination = destination + "/" + entry->d_name;
          mkdir(new_destination.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
          copy_directory(new_source, new_destination, para_filter);
        }
      } else {
        std::string source_file = source + "/" + entry->d_name;
        std::string destination_file = destination + "/" + entry->d_name;
        if (!para_filter.filter(source_file)) {
          continue;
        }
        copy_file(source_file, destination_file);
      }
    }
    closedir(dir);
  }
}

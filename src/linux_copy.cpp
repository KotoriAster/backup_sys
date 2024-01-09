#include "linux_copy.hpp"
#include "LZ77.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

void file_backuper::copy_file(const std::string &source,
                              const std::string &destination) {
  struct stat st;
  if (lstat(source.c_str(), &st) != 0) {
    std::cerr << "Error stating file\n";
    return;
  }

  if (S_ISLNK(st.st_mode)) {
    // This is a symbolic link, so copy it as is
    char buffer[8192];
    ssize_t bytes = readlink(source.c_str(), buffer, sizeof(buffer));
    if (bytes == -1) {
      std::cerr << "Error reading symbolic link\n";
      return;
    }
    buffer[bytes] = '\0';
    symlink(buffer, destination.c_str());
    return;
  }

  if (S_ISFIFO(st.st_mode)) {
    // This is a named pipe, so create a new named pipe at the destination
    if (mkfifo(destination.c_str(), st.st_mode) == -1) {
      std::cerr << "Error creating named pipe\n";
      return;
    }
    return;
  }

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
      std::cerr << "Error opening file\n"
                << "source:" << source << " dest:" << destination << "\n";
      return;
    }

    ssize_t bytes;
    while ((bytes = read(source_fd, buffer, sizeof(buffer))) > 0) {
      write(dest_fd, buffer, bytes);
    }

    close(source_fd);
    close(dest_fd);

    // Preserve permissions
    if (chmod(destination.c_str(), st.st_mode) == -1) {
      std::cerr << "Error setting permissions\n";
    }

    // Preserve timestamps
    struct timeval times[2] = {{st.st_atime, 0}, {st.st_mtime, 0}};
    if (utimes(destination.c_str(), times) == -1) {
      std::cerr << "Error setting timestamps\n";
    }

    if (chown(destination.c_str(), st.st_uid, st.st_gid) == -1) {
      std::cerr << "Error setting owner and group\n";
    }

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
          __mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR;
          std::string new_source = source + "/" + entry->d_name;
          std::string new_destination = destination + "/" + entry->d_name;
          struct stat st;
          if (lstat(source.c_str(), &st) == 0) {
            mode = st.st_mode;
          } else {
            std::cerr << "Error stating directory\n";
          }
          mkdir(new_destination.c_str(), mode);
          copy_directory(new_source, new_destination, para_filter);
        }
      } else {
        std::string source_file = source + "/" + entry->d_name;
        std::string destination_file = destination + "/" + entry->d_name;
        if (!para_filter.filter(source_file)) {
          continue;
        }
        if (!fb_compressor.filter(entry->d_name))
          copy_file(source_file, destination_file);
        else
          fb_compressor.compressor_switch(source_file, destination_file);
      }
    }
    closedir(dir);
  }
}

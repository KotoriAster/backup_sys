#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

bool filter(const std::string &filepath) {
  // Add your filtering logic here. For example, to only copy files that are
  // smaller than a certain size:
  struct stat st;
  if (stat(filepath.c_str(), &st) == 0 && st.st_size <= 2048) {
    return true;
  }
  return false;
}

void copy_file(const std::string &source, const std::string &destination) {
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
}

void copy_directory(const std::string &source, const std::string &destination) {
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
          copy_directory(new_source, new_destination);
        }
      } else {
        std::string source_file = source + "/" + entry->d_name;
        std::string destination_file = destination + "/" + entry->d_name;
        if (!filter(source_file)) {
          continue;
        }
        copy_file(source_file, destination_file);
      }
    }
    closedir(dir);
  }
}

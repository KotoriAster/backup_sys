#include "linux_copy.hpp"

#include <ctime>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#ifdef backer
class backer {

  struct fileinfo {
    std::time_t last_modified;
    std::vector<std::string> duplicates;
  };

private:
  std::string metafilepath;
  std::map<std::string, fileinfo> backuphistory;

public:
  backer(const std::string &metafilepath) { this->metafilepath = metafilepath; }

  bool copyfile(const std::string &sourcePath,
                const std::string &destinationPath) {
    std::ifstream sourceFile(sourcePath, std::ios::binary);
    std::ofstream destinationFile(destinationPath, std::ios::binary);

    if (!sourceFile || !destinationFile) {
      return false;
    }

    destinationFile << sourceFile.rdbuf();
    history_register(sourcePath, destinationPath);

    return true;
  }

  void history_register(const std::string &path, const std::string &des_path) {

    std::time_t last_modified = std::time(nullptr);
    if (backuphistory.find(path) == backuphistory.end()) {
      fileinfo fi;
      backuphistory[path] = fi;
    }
    backuphistory[path].last_modified = last_modified;
    backuphistory[path].duplicates.push_back(des_path);
  }

  bool backup_file(const std::string &path, std::string des_path = "") {
    if (des_path.empty()) {
      std::string des_path = path + ".backup";
    }
    return copyfile(path, des_path);
  }
};
#endif

int main() {

  std::string source_string = "./testcases/1";
  std::string destination_string = "./testcases/t1";
  file_filter filter{2048, reg_file};
  file_backuper fb1 = file_backuper();
  fb1.copy_directory(source_string, destination_string, filter);
  return 0;
}

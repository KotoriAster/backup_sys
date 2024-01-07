#include "linux_copy.hpp"

#include <ctime>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

int main() {

  std::string source_string = "./testcases/1";
  std::string destination_string = "./testcases/t1";
  file_filter filter{2048, fifo_file | reg_file | sym_file};
  file_backuper fb1 = file_backuper();
  fb1.copy_directory(source_string, destination_string, filter);
  return 0;
}

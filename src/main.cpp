#include "LZ77.hpp"
#include "linux_copy.hpp"

#include <ctime>
#include <fstream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

int main() {
#define t2
#ifdef t1
  std::string source_string = "./testcases/1";
  std::string destination_string = "./testcases/t1";

  struct tm start_time, end_time;
  std::string start_time_string = "2024-01-01-00:00:00";
  std::string end_time_string = "2024-01-09-00:00:00";
  strptime(start_time_string.c_str(), "%Y-%m-%d-%H:%M:%S", &start_time);
  strptime(end_time_string.c_str(), "%Y-%m-%d-%H:%M:%S", &end_time);
  file_filter filter{2048,
                     reg_file,
                     std::regex(".*"),
                     {mktime(&start_time), mktime(&end_time)}};
  file_backuper fb1 = file_backuper();
  fb1.copy_directory(source_string, destination_string, filter);
  return 0;
#endif

#ifdef t2
  std::string source_string = "./testcases/1/simpletext";
  std::string destination_string = "./testcases/t2/simpletext.lz77";

  compressor_lz77 compressor = {std::regex(".*"), 1 << 10};
  compressor.cl_compress_file(source_string, destination_string);
  compressor.cl_decompress_file(destination_string, "testcases/t2/simpletext");
#endif
}

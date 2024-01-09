#include "LZ77.hpp"
#include "linux_copy.hpp"

#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

int main(int argc, char **argv) {

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

#ifdef t3
  std::string source_string = "testcases/t3";
  std::string destination_string = "testcases/t4";

  compressor_lz77 compressor = {std::regex("^[a-z]+.lz77$"), 1 << 10, false};
  file_backuper fb3 = file_backuper(compressor);
  fb3.copy_directory(source_string, destination_string, file_filter());
#endif

  if (argc < 10) {
    std::cerr << "Usage: " << argv[0]
              << " <source> <destination> <file_size_limit> <file_type_limit: "
                 "rdsbcf> "
                 "<name_pattern> <start_time> <end_time> <compression_filter> "
                 "<window_size> <compress>\n";
    return 1;
  }
  std::vector<std::string> args(argv + 1, argv + argc);
  std::string source_string = args[0];
  std::string destination_string = args[1];
  file_filter filter;
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

  file_backuper fb = file_backuper(compressor);
  fb.copy_directory(source_string, destination_string, filter);
  return 0;
}

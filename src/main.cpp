#include "LZ77.hpp"
#include "linux_copy.hpp"

#include <cstdio>
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

  file_backuper fb;
  file_filter filter;
  set_args(fb, filter, args);
  int a = 0;
  while (a != -1) {
    int p;
    std::string para;
    std::cout << "\n position to change in args:\n";
    scanf("%d", &p);
    while (p > -1 && p < 10) {
      std::cin >> para;
      args.at(p) = para;
      args_printer(args);
      std::cout << "\n next position to change in args:\n";
      scanf("%d", &p);
    }
    args_printer(args);
    std::cout << "\n Is the args right? 1 for yes, 0 for no\n";
    scanf("%d", &a);
    if (a) {
      set_args(fb, filter, args);
      fb.copy_directory(args[0], args[1], filter);
    }
    std::cout << "\n continue? -1 to exit\n";
    scanf("%d", &a);
  }
  return 0;
}

#define CATCH_CONFIG_MAIN
#include "LZ77.hpp"
#include "catch_amalgamated.hpp"
#include "linux_copy.hpp"

#include <ctime>
#include <fstream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

TEST_CASE("filter test") {

  SECTION("final filter") {
    std::string source_string = "testcases/1";
    std::string destination_string = "testcases/t1";

    struct tm start_time, end_time;
    std::string start_time_string = "2024-01-01-00:00:00";
    std::string end_time_string = "2024-01-09-00:00:00";
    strptime(start_time_string.c_str(), "%Y-%m-%d-%H:%M:%S", &start_time);
    strptime(end_time_string.c_str(), "%Y-%m-%d-%H:%M:%S", &end_time);
    file_filter filter{2048,
                       fifo_file | reg_file | sym_file,
                       std::regex(".*"),
                       {mktime(&start_time), mktime(&end_time)}};
    file_backuper fb1 = file_backuper();
    fb1.copy_directory(source_string, destination_string, filter);
  }
}

TEST_CASE("compressor_integration") {

  SECTION("initial test") {
    std::string source_string = "testcases/1/simpletext";
    std::string destination_string = "testcases/t2/simpletext.lz77";

    compressor_lz77 compressor = {std::regex(".*"), 1 << 10};
    compressor.cl_compress_file(source_string, destination_string);
    compressor.cl_decompress_file(destination_string,
                                  "testcases/t2/simpletext");
  }
}
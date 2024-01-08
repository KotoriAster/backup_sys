////
// @file test.cc
// @brief
// 采用catch2作为单元测试方案，需要一个main函数，这里定义。
//
// @author niexw
// @email xiaowen.nie.cn@gmail.com
//
#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "linux_copy.hpp"

#include <ctime>
#include <fstream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

int theAnswer() { return 6 * 9; }

TEST_CASE("Life, the universe and everything", "[42][theAnswer]") {
  REQUIRE(theAnswer() == 42);
}

TEST_CASE("") {

  std::string source_string = "./testcases/1";
  std::string destination_string = "./testcases/t1";

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
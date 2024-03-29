
#pragma once

#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <vector>
struct LZ77Token {
  int offset;
  int length;
  char next;
};

std::vector<char> lz77_decode(const std::vector<LZ77Token> &input);
std::vector<LZ77Token> lz77_encode(const std::vector<char> &input,
                                   int window_size);
void decompress_file(const std::string &in_filename,
                     const std::string &out_filename);
void compress_file(const std::string &in_filename,
                   const std::string &out_filename, int window_size);

struct compressor_lz77 {
  std::regex compression_filter = std::regex("^$");
  int window_size = 1 << 10;
  bool compress = true;
  inline void cl_compress_file(const std::string &in_filename,
                               const std::string &out_filename) {
    compress_file(in_filename, out_filename, window_size);
  }

  inline void cl_decompress_file(const std::string &in_filename,
                                 const std::string &out_filename) {
    decompress_file(in_filename, out_filename);
  }

  inline bool filter(const std::string &filename) {
    return std::regex_match(filename, compression_filter);
  }

  inline void compressor_switch(const std::string &in_filename,
                                const std::string &out_filename) {
    if (compress) {
      cl_compress_file(in_filename, out_filename + ".lz77");
    } else {
      if (std::regex_match(out_filename, std::regex(".*.lz77$"))) {
        std::string out_filename2 =
            out_filename.substr(0, out_filename.size() - 5);
        cl_decompress_file(in_filename, out_filename2);
      } else {
        std::cerr << "Error: file extension not recognized\n";
      }
    }
  }
};
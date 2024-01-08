#include "LZ77.hpp"

std::vector<char> lz77_decode(const std::vector<LZ77Token> &input) {
  std::vector<char> output;
  for (const auto &token : input) {
    if (token.length > 0) {
      int start = output.size() - token.offset;
      for (int i = 0; i < token.length; ++i) {
        output.push_back(output[start + i]);
      }
    }
    output.push_back(token.next);
  }
  return output;
}

void decompress_file(const std::string &in_filename,
                     const std::string &out_filename) {
  std::ifstream file(in_filename, std::ios::binary);
  std::vector<LZ77Token> buffer;

  LZ77Token token;
  while (file.read(reinterpret_cast<char *>(&token), sizeof(LZ77Token))) {
    buffer.push_back(token);
  }
  auto decoded = lz77_decode(buffer);

  std::ofstream out_file(out_filename, std::ios::binary);
  for (const auto &byte : decoded) {
    out_file.write(&byte, sizeof(byte));
  }
}

std::vector<LZ77Token> lz77_encode(const std::vector<char> &input,
                                   int window_size) {
  std::vector<LZ77Token> output;
  for (int i = 0; i < input.size(); ++i) {
    int match_distance = 0;
    int match_length = 0;
    for (int j = std::max(0, i - window_size); j < i; ++j) {
      int k = 0;
      while (input[j + k] == input[i + k] && j + k < i &&
             i + k < input.size()) {
        k++;
      }
      if (k > match_length) {
        match_distance = i - j;
        match_length = k;
      }
    }
    output.push_back({match_distance, match_length, input[i + match_length]});
    i += match_length;
  }
  return output;
}

void compress_file(const std::string &in_filename,
                   const std::string &out_filename, int window_size) {
  std::ifstream file(in_filename, std::ios::binary);
  std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});

  auto encoded = lz77_encode(buffer, window_size);

  std::ofstream out_file(out_filename, std::ios::binary);
  for (const auto &token : encoded) {
    out_file.write(reinterpret_cast<const char *>(&token), sizeof(token));
  }
}
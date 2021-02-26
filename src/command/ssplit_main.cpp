#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <fcntl.h>
#include <memory>
#include <sys/mman.h>
#include <cassert>
#include "ssplit.h"
#include "CLI11.hpp"

using namespace ug::ssplit;
using splitmode=SentenceStream::splitmode;

splitmode mode; // split mode: input is one {sentence|paragraph} per line or wrapped text
uint parctr=1; // count paragraphs (if requested via --with-line-numbers)
bool with_line_numbers=false;

// Split a chunk of text.
void ssplit_chunk(SentenceSplitter const& ssplit, string_view data) {
  string_view snt;
  SentenceStream buf(data.data(), data.size(), ssplit, mode);
  uint linectr=1; // count lines (if requested via --with-line-numbers)
  if (mode == splitmode::one_sentence_per_line) {
    while (buf >> snt) {
      if (with_line_numbers)
        printf("%u ", linectr++);
      std::cout.write(snt.data(), snt.size());
      std::cout.write("\n", 1);
    }
  }
  else {
    uint firstpar = parctr; // first paragraph in this function call
    while (buf >> snt) {
      if (linectr == 0) { // beginning of paragraph
        if (parctr > firstpar) {
          printf("\n"); // mark paragraph boundary
        }
        if (snt.size() == 0) { // empty paragraph; should happen only in one par per line
          assert(mode == splitmode::one_paragraph_per_line);
          if (with_line_numbers) {
            printf("%u.%u", parctr++, linectr); // output the empty paragraph
          }
          printf("\n");
          continue;
        }
      }
      if (snt.size()) {
        if (with_line_numbers) { // print par.line number
          printf("%u.%u ", parctr, linectr++);
        }
        if (mode == splitmode::wrapped_text) {
          // bit more expensive, but we need to remove line breaks within the paragraph
          single_line(std::cout, snt, "\n");
        }
        else { // one paragraph per line text
          std::cout.write(snt.data(), snt.size());
          std::cout.write("\n", 1);
        }
      }
      else {
        linectr = 0;
        ++parctr;
      }
    }
  }
}

// Memory-map and sentence-split a file that can be memory-mapped
// (cannot be compressed) This is faster than the stream-based
// splitting, because it doesn't need to copy the input data. It is
// also the only way to test whether sentence and paragraph
// recognition works properly in SentenceStream, because in
// stream-based mode this is done outside of the sentence splitter.
//
// @param filename: file to be mapped
// @param ssplit: sentence splitter
// @param mode: split mode
// @param number_lines: number lines per paragraph (for debugging
void mmap_and_split(std::string& filename, SentenceSplitter const& ssplit) {
  char* data;
  int fd = open(filename.c_str(), O_RDONLY);
  struct stat sb;
  fstat(fd,&sb);
  data = reinterpret_cast<char*>(mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
  string_view input(data, sb.st_size);
  ssplit_chunk(ssplit, input);
}

// Split an istream.
void ssplit_stream(std::istream& in, SentenceSplitter const& ssplit) {
  std::string chunk;
  if (mode == splitmode::one_sentence_per_line) {
    uint32_t linectr=1;
    while (getline(in, chunk)) {
      if (with_line_numbers)
        printf("%u ", linectr++);
      if (chunk.size())
        std::cout.write(&chunk[0], chunk.size());
      printf("\n");
    }
  }
  else if (mode == splitmode::one_paragraph_per_line) {
    while (getline(in, chunk)) {
      if (parctr > 1) { // not the first paragraph
        printf("\n");
      }
      ssplit_chunk(ssplit, chunk);
      ++parctr;
    }
  }
  else { // wrapped text
    std::string paragraph, line;
    paragraph.reserve(10000); // that's an arbitrary buffer size
    while (getline(in, paragraph)){
      if (parctr > 1) { // not the first paragraph
        printf("\n");
      }
      // skip empty lines:
      while(paragraph.empty() && getline(std::cin, paragraph));
      paragraph += "\n"; // first line
      while(getline(in, line) && !line.empty()) // add lines
        paragraph += line+"\n";
      ssplit_chunk(ssplit, paragraph);
      paragraph.clear();
      ++parctr;
    }
  }
}

int main(int argc, char const* argv[]) {
  std::string prefix_file;    // prefix file to be loaded by SentenceSplitter
  std::string modespec {"w"}; // split mode specficication
  std::string input_file;     // file to ssplit (cannot be compressed)
  CLI::App app{"Sentence Splitter"};
  app.footer("\nIf no input file is given, ssplit reads from stdin. "
             "Input files are memory-mapped (decompression is NOT supported!) "
             "for faster processing.\n");
  app.add_option("input", input_file, "input file.")
    ->check(CLI::ExistingFile);
  app.add_option("-m,--mode", modespec,
                 "Split mode: s (one sentence per line), p (one paragraph per line), "
                 "w (wrapped text).", "w")
    ->default_str(modespec)
    ->check(CLI::IsMember({"w","s","p"}));
  app.add_flag("-n, --with-line-numbers", with_line_numbers,
               "print paragraph and line numbers (for debugging)");
  app.add_option("-p,--prefix-file", prefix_file,
                 "File with nonbreaking prefixes.")
    ->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);

  // map mode characters to proper enumerated type
  mode = (modespec == "w" ? splitmode::wrapped_text :
          modespec == "p" ? splitmode::one_paragraph_per_line :
          splitmode::one_sentence_per_line);

  ug::ssplit::SentenceSplitter ssplit(prefix_file);

  if (input_file.size()) {
    // can be mapped (no support for gzip/bzip at this point)
    mmap_and_split(input_file, ssplit);
  }
  else {
    ssplit_stream(std::cin, ssplit);
  }
  exit(0);
}

// Test program that memory-maps a file and splits it into sentences.
// Written primarily to test one_sentence_per_line mode.
#include <pcrecpp.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <sys/mman.h>
#include <fcntl.h>
#include "ssplit.h"
#include "CLI11.hpp"
#include "regex.h"

using namespace ug::ssplit;
using splitmode=SentenceStream::splitmode;

void ssplit_chunk(char const* data, size_t datasize,
                  SentenceSplitter const& ssplit,
                  splitmode const& mode_){
  absl::string_view  snt;
  SentenceStream buf(data, datasize, ssplit, mode_);
  int linectr=0;
  while (buf >> snt)
  {
    // print sequence number of sentence in the respective paragraph.
    // This makes it a bit easer to find multi-sentence paragraphs during
    // debugging.
    // continue;
    if (snt.size())
      printf("%d ",linectr++);
    else
      linectr = 0;
    if (mode_ == splitmode::wrapped_text) {
      single_line(std::cout, snt);
    }
    else {
      std::cout.write(snt.data(), snt.size());
    }
    std::cout.write("\n", 1);
  }
}

int main(int argc, char const* argv[]) {
  splitmode mode = splitmode::wrapped_text; // split mode
  std::string modespec ="w";
  std::string prefix_file, input_file;

  CLI::App app{"Sentence Splitter"};
  app.add_option("-p,--prefix-file", prefix_file,
                 "File with nonbreaking prefixes.")
    ->check(CLI::ExistingFile);
  app.add_option("input", input_file, "input file.")
    ->check(CLI::ExistingFile);
  app.add_option("-m,--mode", modespec,
                 "Split mode: s (one sentence per line), p (one paragraph per line), "
                 "w (wrapped text).")
    ->default_str(modespec)
    ->check(CLI::IsMember({"w","s","p"}));
  CLI11_PARSE(app,argc,argv);

  mode = (modespec == "w" ? splitmode::wrapped_text :
          modespec == "p" ? splitmode::one_paragraph_per_line :
          splitmode::one_sentence_per_line);

  char* data;
  int fd = open(input_file.c_str(), O_RDONLY);
  struct stat sb;
  fstat(fd,&sb);
  data = reinterpret_cast<char*>(mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));

  absl::string_view DATA(data, sb.st_size);
  ug::ssplit::SentenceSplitter ssplit(prefix_file);
  ssplit_chunk(data, sb.st_size, ssplit, mode);
  exit(0);
}

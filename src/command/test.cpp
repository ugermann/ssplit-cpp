#include <pcrecpp.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include "ssplit.h"
#include "CLI11.hpp"

class Countdown{
  int x;
public:
  Countdown(int hi) : x(hi) {}
  bool operator>>(int& foo){
    if (x) { foo = x--; return true; }
    else return false;
  }
};

using namespace ug::ssplit;
using splitmode=SentenceStream::splitmode;

void ssplit_chunk(std::string& chunk,
                  SentenceSplitter const& ssplit,
                  splitmode const& mode_){
  // std::cout << "CHUNK\n" << chunk << std::endl;
  std::string snt;
  SentenceStream buf(chunk, ssplit, mode_);
  while (buf >> snt) {
    std::cout << snt << std::endl;
  }
  std::cout << std::endl;
}

int main(int argc, char const* argv[]) {

  std::string prefix_file;
  bool oppl=false,testmode=false; // one paragraph per line

  CLI::App app{"Sentence Splitter"};
  app.add_option("-p,--prefix-file", prefix_file, "Path to prefix file.")
    ->check(CLI::ExistingFile);
  app.add_flag("-P,--oppl", oppl,
               "Input is one paragraph per line (default: wrapped text)");
  app.add_flag("-t", testmode, "Test mode for wrapped text");
  CLI11_PARSE(app,argc,argv);

  ug::ssplit::SentenceSplitter ssplit(prefix_file);
  std::string line,chunk;
  if (oppl){
    while (getline(std::cin,chunk)){
      ssplit_chunk(chunk, ssplit, splitmode::one_paragraph_per_line);
    }
  }
  else if (testmode) {
    // read entire input all at once, so that we can test paragraph detection
    // in the sentence splitter
    std::string text;
    text.reserve(10000);
    for(getline(std::cin, text); getline(std::cin, line);){
      text += "\n";
      text += line;
    }
    text += "\n";
    ssplit_chunk(text, ssplit, splitmode::wrapped_text);
  }
  else {
    // read and one paragraph at a time
    std::string paragraph;
    paragraph.reserve(10000);
    while (getline(std::cin, paragraph)){
      while(paragraph.empty() && getline(std::cin, paragraph)); // skip empty lines
      paragraph += "\n";
      while(getline(std::cin,line) && !line.empty())
        paragraph += line+"\n";
      ssplit_chunk(paragraph, ssplit, splitmode::wrapped_text);
    }
  }
  exit(0);
}

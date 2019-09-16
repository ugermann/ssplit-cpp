#include <pcrecpp.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include "ssplit.h"

class Countdown{
  int x;
public:
  Countdown(int hi) : x(hi) {}
  bool operator>>(int& foo){
    if (x) { foo = x--; return true; }
    else return false;
  }
};

int main(int argc, char const* argv[]) {
  std::string prefix_file = argc > 1 ? argv[1] : "";
  ug::ssplit::SentenceSplitter ssplit(prefix_file);
  std::string line,text,snt;
  while (getline(std::cin,line)){
    ug::ssplit::SentenceStream buf(line,ssplit);
    while (buf >> snt) {
      std::cout << snt << "\n" << std::endl;
    }
  }
}

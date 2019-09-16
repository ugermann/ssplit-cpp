#include<fstream>
#include<iostream>
#include "ssplit.h"
namespace ug{
namespace ssplit{

using namespace pcrecpp;

void
SentenceSplitter::
read_prefix_file_(std::string const& fname){
  using namespace std;
  RE pat("([^#].*)(?:[\\s]+(\\#NUMERIC_ONLY\\#))?", UTF8());
  ifstream pfile(fname);
  string line, prefix, tag;
  while (getline(pfile,line)){
    if (pat.FullMatch(line,&prefix,&tag))
      prefix_type_[prefix] = tag.empty() ? 1 : 2;
  }
}

SentenceSplitter::
SentenceSplitter(){
  // End of sentence:
  RE_Options options = UTF8();
  options.set_multiline(true).set_dotall(true).set_dollar_endonly(true);
  eos.reset(new RE("([^\\s].*?([^\\s]*?)([.?!]+)['\")\\]\\p{Pf}]*)(?:\\s+|$)"
                   "(?=(['\"(\\[¿¡\n\\p{Pi}]*)[\\s]*\\p{Lu}|$)", options));
  // Abbreviations:
  abbrev.reset(new RE("(?:\\p{L}\\.)+\\p{L}",UTF8()));

  // Starts with digits (for NUMERIC_ONLY prefixes)
  digits.reset(new RE("^[\\p{Nd}\\p{Nl}]",UTF8()));
}

SentenceSplitter::
SentenceSplitter(std::string const& prefix_file)
  : SentenceSplitter() {
  if (prefix_file.size()){
    std::cerr << prefix_file << std::endl;

    read_prefix_file_(prefix_file);
  }
}

StringPiece
SentenceSplitter::
operator()(StringPiece* rest) const {
  StringPiece snt,punct,inipunct;
  std::string prefix; // string because we need it for map lookup

  // keep track of where we started
  char const* snt_start = rest->data();

  while (eos->Consume(rest, &snt, &prefix, &punct,&inipunct)){
    // std::cout << "SNT: " << snt << std::endl;
    // std::cout << "PRE: " << prefix << std::endl;
    // std::cout << "INI: " << inipunct << std::endl;
    // std::cout << "RST: " << rest << std::endl;
    if (punct == "." && inipunct.empty() && !abbrev->FullMatch(prefix)) {
      auto m = prefix_type_.find(prefix);
      if (m != prefix_type_.end()){ // known prefix
        if (m->second == 1 || digits->PartialMatch(*rest))
          continue;
      }
    }
    break;
  }
  if (rest->data() == snt_start){ // no sentence boundary detected
    snt = *rest;
    rest->clear();
  }
  else {
    snt.set(snt_start,rest->data()-snt_start);
  }
  return snt;
}

SentenceStream::
SentenceStream(std::string const& text, SentenceSplitter& splitter)
  : rest_(text), splitter_(splitter) {

}

bool
SentenceStream::
operator>>(std::string& snt){
  StringPiece s;
  (*this) >> s;
  if (!s.data()) return false;
  snt = std::string(s.data(),s.size());
  return true;
};

bool
SentenceStream::
operator>>(StringPiece& snt){
  if (!rest_.data()){ return false; }
  snt = splitter_(&rest_);
  return snt.data() != NULL;
};

}}

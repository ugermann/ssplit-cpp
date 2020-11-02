#include<fstream>
#include<iostream>
#include "ssplit.h"
namespace ug{
namespace ssplit{

using namespace pcrecpp;

void
SentenceSplitter::
load(std::string const& fname){
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
  eos.reset(new RE("\\s*([^\\s].*?([^\\s]*?)([.?!]+)['\")\\]\\p{Pf}]*)(?:\\s+|$)"
                   "(?=(['\"(\\[¿¡\\r\\n\\p{Pi}]*)[\\s]*\\p{Lu}|$)", options));
  // Abbreviations:
  abbrev.reset(new RE("(?:\\p{L}\\.)+\\p{L}",UTF8()));

  // Starts with digits (for NUMERIC_ONLY prefixes)
  digits.reset(new RE("^[\\p{Nd}\\p{Nl}]",UTF8()));
}

SentenceSplitter::
SentenceSplitter(std::string const& prefix_file)
  : SentenceSplitter() {
  if (prefix_file.size()){
    load(prefix_file);
  }
}

StringPiece
SentenceSplitter::
operator()(StringPiece* rest) const {
  StringPiece snt,punct,inipunct;
  std::string prefix; // string because we need it for map lookup

  // keep track of where we started
  char const* snt_start = rest->data();

  while (eos->Consume(rest, &snt, &prefix, &punct, &inipunct)){
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
SentenceStream(StringPiece const& text,
               SentenceSplitter const& splitter,
               splitmode const& mode)
  : rest_(text),
    mode_(mode),
    splitter_(splitter),
    line_pattern_("(.*?)(?:\n|$)",UTF8().set_multiline(true)),
    paragraph_pattern_("(.*?)(?:(?:\\r?\\n){2,}|$)",
                       UTF8()
                       .set_dotall(true)
                       .set_dollar_endonly(true)){
  // If sentence splitting is to be performed, get the first paragraph
  if (mode == splitmode::one_paragraph_per_line){
    line_pattern_.Consume(&rest_, &paragraph_);
    // std::cerr << "P " << paragraph_ << std::endl;
  }
  else if (mode == splitmode::wrapped_text){
    paragraph_pattern_.Consume(&rest_, &paragraph_);
    // std::cerr << "p " << paragraph_ << std::endl;
  }
}

bool
SentenceStream::
operator>>(StringPiece& snt){
  // std::cerr << paragraph_.size() << " " << rest_.size() << std::endl;
  if (paragraph_.size() == 0 && rest_.size() == 0){
    return false;
  }

  if (mode_ == splitmode::one_sentence_per_line){
    line_pattern_.Consume(&rest_, &snt);
  }
  else if (paragraph_.size() == 0){
    // no more sentences in this paragraph; progress to the next line
    snt = paragraph_; // we will return an empty string to indicate end of paragraph
    if (mode_ == splitmode::one_paragraph_per_line){
      if (!line_pattern_.Consume(&rest_, &paragraph_)){
        paragraph_ = rest_;
        rest_.clear();
      }
    }
    else{ // wrapped text
      if (!paragraph_pattern_.Consume(&rest_, &paragraph_)){
        paragraph_ = rest_;
        rest_.clear();
      }
    }
    // std::cerr << "p " << paragraph_ << std::endl;
  }
  else{
    snt = splitter_(&paragraph_);
  }
  // std::cerr << "s " << snt << std::endl;
  // std::cerr << snt.size() << " "
  //           << paragraph_.size() << " "
  //           << rest_.size() << std::endl;
  return true;
};

bool
SentenceStream::
operator>>(std::string& snt){
  static RE linebreak("[ \\t]*\\r*\\n[ \\t]*", UTF8().set_multiline(true));
  StringPiece s;
  if (!((*this) >> s))
    return false;
  snt = std::string(s.data(),s.size());
  if (mode_ == splitmode::wrapped_text)
    linebreak.GlobalReplace(" ", &snt);
  return true;
};

}}

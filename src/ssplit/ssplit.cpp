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
SentenceSplitter(){}

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
  static RE eos("\\s*([^\\s].*?([^\\s]*?)([.?!]+)['\")\\]\\p{Pf}]*)"
                "(?:\\s+|\\[[\\p{Nd}\\p{Nl}]+\\]|$)" // second part: footnotes
                "(?=(['\"(\\[¿¡\\r\\n\\p{Pi}])|\\s*\\p{Lu}|$)",
                UTF8().set_multiline(true).set_dotall(true).set_dollar_endonly(true));
  static RE abbrev = RE("(?:\\p{L}\\.)+\\p{L}",UTF8());
  static RE digits = RE("^[\\p{Nd}\\p{Nl}]",UTF8());

  StringPiece snt, punct, inipunct;
  std::string prefix; // Defined as string because we need it for map lookup

  // keep track of where we started
  char const* snt_start = rest->data();

  bool success;
  while ((success = eos.Consume(rest, &snt, &prefix, &punct, &inipunct))){
    /*
      std::cout << "SNT: " << snt << std::endl;
      std::cout << "PRE: " << prefix << std::endl;
      std::cout << "INI: " << inipunct << std::endl;
      std::cout << "RST: " << rest << std::endl;
    */
    if (punct == "." && inipunct.empty() && !abbrev.FullMatch(prefix)) {
      auto m = prefix_type_.find(prefix);
      if (m != prefix_type_.end()){ // known prefix
        if (m->second == 1 || digits.PartialMatch(*rest)) {
          continue;
        }
      }
    }
    break;
  }
  if (!success){ // no sentence boundary detected
    snt = *rest;
    rest->clear();
  }
  else {
    snt.set(snt_start, rest->data() - snt_start);
  }
  return snt;
}

// readLine gets pointers to start and stop of data instead of
// a StringPiece to be able to proccess chunks of data that
// exceed the size that a StringPiece can store (int32_t).
StringPiece readLine(const char** start, const char* const stop) {
  StringPiece line;
  if (*start == stop) { // no more data
    return line;
  }
  const char* c = *start;
  while (c < stop && *c != '\n') ++c;  // skip to next EOL
  const char* d = c;
  while (d-- > *start && *d == '\r'); // trim potential CR
  line.set(*start, ++d - *start);
  *start = (c == stop ? c : c + 1);
  return line;
}

// readParagraph gets pointers to start and stop of data instead of
// a StringPiece to be able to proccess chunks of data that
// exceed the size that a StringPiece can store (int32_t).
StringPiece readParagraph(const char** start, const char* const stop) {
  StringPiece par;
  if (*start == stop) { // no more data
    return par;
  }
  const char* c = *start;
  const char* d;
  do {
    while (c < stop && *c != '\n') ++c; // skip to next EOL
    d = c;
    while (d++ < stop && (*d == '\n' || *d == '\r'));
  } while (d < stop && d-c == 1);
  const char* e = c;
  while (e-- > *start && *e == '\r');
  par.set(*start, ++e - *start);
  *start = (d == stop ? d : d + 1);
  return par;
}

SentenceStream::
SentenceStream(StringPiece const& text,
               SentenceSplitter const& splitter,
               splitmode const& mode)
  : SentenceStream(text.data(), text.size(), splitter, mode){}

SentenceStream::
SentenceStream(char const* data, size_t datasize,
               SentenceSplitter const& splitter,
               splitmode const& mode)
  : cursor_(data), stop_(data + datasize), mode_(mode), splitter_(splitter)
{
  if (mode == splitmode::one_paragraph_per_line){
    paragraph_ = readLine(&cursor_, stop_);
  }
  else if (mode == splitmode::wrapped_text){
    paragraph_ = readParagraph(&cursor_, stop_);
  }
}

bool
SentenceStream::
operator>>(StringPiece& snt){
  if (paragraph_.size() == 0 && cursor_ == stop_){
    // we have reached the end of the data:
    return false;
  }

  if (mode_ == splitmode::one_sentence_per_line){
    snt = readLine(&cursor_, stop_);
  }
  else if (paragraph_.size() == 0){
    // No more sentences in this paragraph.
    // Read the next paragraph but for this call return
    // and empty sentence to indicate the end of this paragraph.
    snt.clear();
    if (mode_ == splitmode::one_paragraph_per_line){
      paragraph_ = readLine(&cursor_, stop_);
    }
    else{ // wrapped text
      paragraph_ = readParagraph(&cursor_, stop_);
    }
  }
  else {
    snt = splitter_(&paragraph_);
  }
  return true;
};

bool
SentenceStream::
operator>>(std::string& snt){
  static RE linebreak("\\s*\\n\\s*", UTF8().set_multiline(true));
  StringPiece s;
  if (!((*this) >> s))
    return false;
  snt = std::string(s.data(),s.size());
  if (mode_ == splitmode::wrapped_text)
    linebreak.GlobalReplace(" ", &snt);
  return true;
};

}}

#pragma once
#include<memory>
#include<pcrecpp.h>
#include<string>
#include<unordered_map>

namespace ug{
namespace ssplit{
class SentenceSplitter{
  std::unordered_map<std::string,int> prefix_type_;
  std::unique_ptr<pcrecpp::RE> eos,abbrev,digits;
public:
  SentenceSplitter();
  SentenceSplitter(std::string const& prefix_file);
  void load(std::string const& fname);

  // Find next sentence boundary, return StringPiece for next sentence,
  // advance rest to reflect the rest of the text.
  pcrecpp::StringPiece operator()(pcrecpp::StringPiece* rest) const;
}; // end of class SentenceSplitter

class SentenceStream{
public:
  enum class splitmode { one_sentence_per_line, one_paragraph_per_line, wrapped_text };
private:
  pcrecpp::StringPiece rest_, paragraph_;
  splitmode mode_;
  SentenceSplitter const& splitter_;
  pcrecpp::RE line_pattern_, paragraph_pattern_;
public:
  SentenceStream(std::string const& text, SentenceSplitter const& splitter, splitmode const& mode);
  bool operator>>(std::string& snt);
  bool operator>>(pcrecpp::StringPiece& snt);
};

}}// end of namespaces

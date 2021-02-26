#pragma once
#include<memory>
#include<string>
#include<unordered_map>
// @TODO: check which C++ version we are using ...
//        ... maybe use C17 standard types for C17 and upwards?
#include "typedefs.h"

namespace ug{
namespace ssplit{

class SentenceSplitter{
  prefix_map_t prefix_type_;

  // Return the prefix class of a prefix.
  // 0: not a prefix
  // 1: prefix
  // 2: prefix only in front of numbers
  int get_prefix_class(string_view piece) const;

public:
  SentenceSplitter();
  SentenceSplitter(std::string const& prefix_file);
  void load(std::string const& fname);

  // Find next sentence boundary, return StringPiece for next sentence,
  // advance rest to reflect the rest of the text.
  string_view operator()(string_view* rest) const;
}; // end of class SentenceSplitter

class SentenceStream{
public:
  enum class splitmode { one_sentence_per_line, one_paragraph_per_line, wrapped_text };

private:
  char const* cursor_;
  char const* const stop_;
  string_view paragraph_;
  splitmode mode_;
  SentenceSplitter const& splitter_;
  std::string error_message_; // holds error message if UTF8 validation fails
  int status_; // holds prce2 error code
public:
  // @param text text to be split into sentences
  // @param splitter the actual sentence splitter
  // @param mode the split mode (one sentence/paragraph per line, wrapped text)
  // @param verify utf8?
  SentenceStream(string_view text,
                 SentenceSplitter const& splitter,
                 splitmode mode, bool verify_utf8=true);

  // @param data start of data
  // @param datasize size of data
  // @param splitter the actual sentence splitter
  // @param mode the split mode (one sentence/paragraph per line, wrapped text)
  // @param verify utf8?
  SentenceStream(char const* data, size_t datasize,
                 SentenceSplitter const& splitter,
                 splitmode mode, bool verify_utf8=true);

  //  bool OK() const; // return true if UTF8 verification succeeded
  int status() const; // return status (pcre2 error code)
  std::string const& error_message() const;
  bool operator>>(std::string& snt);
  // bool operator>>(pcrecpp::StringPiece& snt);
  bool operator>>(string_view& snt);

};

// Auxiliary function to print a chunk of text as a single line,
// replacing line breaks by blanks. This is faster than doing a
// global replacement in a string first.
std::ostream&
single_line(std::ostream& out, // destination stream
            string_view span, // text span to be printed in a single line
            string_view end = "", // stuff to put at end of line
            bool validate_utf = false); // do we need to validate UTF8?

// Auxiliary function to stiore a chunk of text as a single line,
// replacing line breaks by blanks.
std::string&
single_line(std::string& snt, // destination stream
            string_view span, // text span to be printed in a single line
            string_view end = "", // stuff to put at end of line
            bool validate_utf = false); // do we need to validate UTF8?


}}// end of namespaces

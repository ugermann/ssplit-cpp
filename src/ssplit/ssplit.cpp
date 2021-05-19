#include <fstream>
#include <iostream>
#include <sstream>
#include "ssplit.h"
#include "regex.h"
#include <cassert>


namespace {
// Anonymous namespace

// Creating an input stream from constant memory
// https://stackoverflow.com/a/13059195/4565794

#include <streambuf>
#include <istream>

struct membuf: std::streambuf {
    membuf(char const* base, size_t size) {
        char* p(const_cast<char*>(base));
        this->setg(p, p, p + size);
    }
};
struct imemstream: virtual membuf, std::istream {
    imemstream(char const* base, size_t size)
        : membuf(base, size)
        , std::istream(static_cast<std::streambuf*>(this)) {
    }
};


}

namespace ug{
namespace ssplit{



// Load a prefix file
void
SentenceSplitter::
load(std::string const& fname){
  std::ifstream pfile(fname);
  loadFromStream(pfile);
}

void SentenceSplitter::loadFromStream(std::istream &stream){
  prefix_type_.clear();
  Regex pat("([^#\\s]*)\\s*(?:(#\\s*NUMERIC_ONLY\\s*#))?", PCRE2_UTF);
  Match M(pat);
  std::string line, prefix, tag;
  while (getline(stream, line)) {
    if (pat.find(line, &M) > 0) {
      auto m1 = M[1];
      if (m1.size()) {
        std::string foo(m1.data(), m1.size());
        prefix_type_[foo] = M[2].size() ? 2 : 1;
      }
    }
  }
  // for (auto m: prefix_type_) {
  //   std::cout << m.first << " " << m.second << std::endl;
  // }
}

void SentenceSplitter::loadFromSerialized(const string_view buffer){
  imemstream stream(buffer.data(), buffer.size());
  loadFromStream(stream);
}

SentenceSplitter::SentenceSplitter(){}

SentenceSplitter::
SentenceSplitter(std::string const& prefix_file)
  : SentenceSplitter() {
  if (prefix_file.size()){
    load(prefix_file);
  }
}

// Auxiliary function to print a chunk of text as a single line,
// replacing line breaks by blanks. This is faster than doing a
// global replacement in a string first.
std::ostream&
single_line(std::ostream& out,   // destination stream
            string_view span,    // text span to be printed in a single line
            string_view end,     // stuff to put at end of line
            bool validate_utf) { // do we need to validate UTF8?
  static Regex P("^\\s*(.*)\\R+\\s*", PCRE2_UTF);
  thread_local static Match M(P);
  int success = P.consume(&span, &M, validate_utf ? 0 : PCRE2_NO_UTF_CHECK);
  while (success > 0) {
    auto m = M[1];
    out.write(m.data(), m.size());
    out.write(" ", 1);
    success = P.consume(&span, &M, PCRE2_NO_UTF_CHECK);
  }
  out.write(span.data(), span.size());
  out.write(end.data(), end.size());
  return out;
}

// Auxiliary function to stiore a chunk of text as a single line in a string,
// replacing line breaks by blanks.
std::string&
single_line(std::string& snt,    // destination stream
            string_view span,    // text span to be printed in a single line
            string_view end,     // stuff to put at end of line
            bool validate_utf) { // do we need to validate UTF8?
  static Regex P("^\\s*(.*)\\R+\\s*", PCRE2_UTF);
  thread_local static Match M(P);
  int success = P.consume(&span, &M, validate_utf ? 0 : PCRE2_NO_UTF_CHECK);
  snt.reserve(span.size());
  snt.clear();
  while (success > 0) {
    auto m = M[1];
    snt.append(m.data(), m.size());
    snt += ' ';
    success = P.consume(&span, &M, PCRE2_NO_UTF_CHECK);
  }
  snt.append(span.data(), span.size());
  snt.append(end.data(), end.size());
  return snt;
}

// return the prefix class of a prefix
// 0: not a prefix
// 1: prefix
// 2: prefix only in front of numbers
int
SentenceSplitter::
get_prefix_class(string_view piece) const {
  static Regex foo(".*\\s([^\\s]*)", PCRE2_DOTALL);
  static Match M(foo);
  if (foo.consume(&piece, &M, PCRE2_NO_UTF_CHECK) > 0) {
    piece = M[1];
  }
  auto m = prefix_type_.find(piece);
  // for debugging:
  // std::cout << piece << " " << (m == prefix_type_.end() ? 0 : m->second) << std::endl;
  return m == prefix_type_.end() ? 0 : m->second;
}

string_view
SentenceSplitter::
operator()(string_view* rest) const {
  // IMPORTANT: this operater does not do any UTF validation. If there's
  // broken UTF8 in the input, the operator may hang or crash. Doing
  // UTF8 validation is infeasible for long strings.
  // cf. http://www.pcre.org/current/doc/html/pcre2unicode.html

  static Regex Whitespace_RE("\\s*", PCRE2_UTF|PCRE2_DOTALL|PCRE2_NEWLINE_ANY);

  // The chunker is the first step in sentence splitting.
  // It identifies candidate split points.
  static Regex Chunker_RE("\\s*" // whitespace
                          "([^.?!]*)" // prefix: anything up to the potential EOS marker
                          "([.?!]++)" // the potential EOS marker
                          "(" // open group for trailing matter
                          "['\")\\]’”\\p{Pf}]*" // any "trailing matter"
                          "(?:\\[[\\p{Nd}]+[\\p{Nd},\\s]*[\\p{Nd}]\\])?" // footnote?
                          "['\")\\]’”\\p{Pf}]*" // any more "trailing matter"
                          ")" // close group for trailing matter
                          "(\\s*)" // whitespace after
                          "(?=" // start look-ahead
                          "([^\\s\\p{L}\\p{N}\\p{M}\\p{S}]*)" // sentence-initial punct.
                          "\\s*" // whitespace
                          "([\\p{L}\\p{M}\\p{N}]*)" // leading letters or digits
                          ")" // close look-ahead
                          , PCRE2_UTF|PCRE2_DOTALL|PCRE2_NEWLINE_ANY);

  // The following patterns are used to make heuristic decisions once a
  // potential split point has been identified.
  static const Regex lowercase("\\p{M}*\\p{Ll}", PCRE2_NO_UTF_CHECK);
  static Regex uppercase("\\p{M}*[\\p{Lu}\\p{Lt}]", PCRE2_NO_UTF_CHECK);
  static Regex digit("[\\p{Nd}\\p{Nl}]", PCRE2_NO_UTF_CHECK);

  // We need these to store match results:
  thread_local static Match Whitespace_M(Whitespace_RE);
  thread_local static Match Chunker_M(Chunker_RE);
  thread_local static Match lowercase_M(lowercase);
  thread_local static Match uppercase_M(uppercase);
  thread_local static Match digit_M(digit);

  int success; /* stores the return value of pcre2_match() which is
                * called in Regex::find() / Regex::consume() */

  string_view snt; // this will eventually be our return value

  Whitespace_RE.consume(rest, &Whitespace_M, PCRE2_NO_UTF_CHECK);
  const char* snt_start = rest->data();
  const char* snt_end = rest->data() + rest->size();
  while((success = Chunker_RE.consume(rest, &Chunker_M, PCRE2_NO_UTF_CHECK)) > 0) {
    auto whole_match      = Chunker_M[0];
    auto prefix           = Chunker_M[1];
    auto punct            = Chunker_M[2]; // punctuation
    auto tail             = Chunker_M[3]; // trailing punctuation
    auto whitespace_after = Chunker_M[4]; // whitespace after
    auto inipunct         = Chunker_M[5]; // following symbols (not letters/digits)
    auto following_symbol = Chunker_M[6]; // first letter or digit after whitespace

    /* FOR DEBUGGING
    std::cout << prefix << "|"
              << punct << "|"
              << tail << "|"
              << whitespace_after <<"|"
              << inipunct << "|"
              << following_symbol << std::endl;
    */

    if (whitespace_after.size() == 0) {
      // this candidate is not followed by whitespace
      continue;
    }
    else if (lowercase.find(following_symbol, &lowercase_M, 0, PCRE2_ANCHORED) > 0) {
      // followed by lower case
      continue;
    }
    else if (uppercase.find(following_symbol, &uppercase_M, 0, PCRE2_ANCHORED) > 0) {
      // followed by uppercase
      if (punct == "." && get_prefix_class(prefix) != 0) // preceded by nonbreaking prefix
        continue;
      if (punct.size() == 1 && *snt_end == '.') // preceded by abbreviation a.b.c
        continue;
    }
    else if (digit.find(following_symbol, &digit_M, 0, PCRE2_ANCHORED) > 0) {
      // std::cout << "Digit" << std::endl;
      // followed by digit
      if (punct == "." && get_prefix_class(prefix) == 2) // preceded by nonbreaking prefix
        continue;
    }
    else {
      // check for in-text ellipsis "[...]"
      if (punct == "..."
          && (punct.data() - whole_match.data() > 1) // not at the beginning
          && tail == "]" && *(punct.data()-1) == '[') {
        continue;
      }
    }
    snt_end = whitespace_after.data(); // set end of sentence span
    break;
  }
  snt = string_view(snt_start, snt_end - snt_start);
  if (success < 1) {
    // Remove trailing whitespace:
    static Regex rtrim("(.*[^\\s])\\s*", PCRE2_NO_UTF_CHECK|PCRE2_DOTALL);
    thread_local static Match M(rtrim);
    if (rtrim.consume(&snt, &M, PCRE2_NO_UTF_CHECK) > 0) {
      snt = M[1];
    }
    *rest = string_view();
  }
  // if (success < -1) {
  //   PCRE2_UCHAR buffer[256];
  //   pcre2_get_error_message(success, buffer, sizeof(buffer));
  //   printf("%s\n", buffer);
  // }
  return snt;
}

// readLine gets pointers to start and stop of data instead of
// a string_view to be able to proccess chunks of data that
// exceed the size that a string_view can store (apparently int32_t).
// @TODO: verify: this dates back to working with pcrecpp::StringPiece.
// Update: apparently not true any more for absl::string_view!
string_view
readLine(const char** start, const char* const stop) {
  string_view line;
  if (*start == stop) { // no more data
    return line;
  }
  const char* c = *start;
  while (c < stop && *c != '\n') ++c;  // skip to next EOL
  const char* d = c;
  while (d-- > *start && *d == '\r'); // trim potential CR
  line = string_view(*start, ++d - *start);
  *start = (c == stop ? c : c + 1);
  return line;
}

// readParagraph gets pointers to start and stop of data instead of
// a string_view to be able to proccess chunks of data that
// exceed the size that a string_view can store (apparently int32_t).
// @TODO: verify: this dates back to working with pcrecpp::StringPiece.
string_view
readParagraph(const char** start, const char* const stop) {
  string_view par;
  if (*start == stop) { // no more data
    return par;
  }
  const char* c = *start;
  const char* d;
  do {
    while (c < stop && *c != '\n') ++c; // skip to next EOL
    d = c++;
    while (d < stop && (*d == '\n' || *d == '\r')) ++d;
  } while (d < stop && d == c);

  const char* e = --c;
  while (e-- > *start && *e == '\r');
  par = string_view(*start, ++e - *start);
  *start = (d < stop ? d : stop);
  return par;
}

SentenceStream::
SentenceStream(string_view text,
               SentenceSplitter const& splitter,
               splitmode mode,
               bool verify_utf8)
  : SentenceStream(text.data(), text.size(), splitter, mode, verify_utf8) {}

SentenceStream::
SentenceStream(char const* data, size_t datasize,
               SentenceSplitter const& splitter,
               splitmode mode,
               bool verify_utf8)
  : cursor_(data), stop_(data + datasize), mode_(mode), splitter_(splitter)
{
  static Regex R(".*",PCRE2_UTF);
  thread_local static Match M(R);

  if (verify_utf8) {
    // pre-flight verification: make sure it's well-formed UTF8
    int success = R.find(string_view(data,datasize), &M);
    if (success < 0) {
      auto offset = pcre2_get_startchar(M.match_data);
      PCRE2_UCHAR buffer[256];
      pcre2_get_error_message(success, buffer, sizeof(buffer));
      std::ostringstream msg;
      msg << "Invalid UTF at position " << offset << ": " << buffer;
      error_message_ = msg.str();
    }
  }
  if (mode == splitmode::one_paragraph_per_line){
    paragraph_ = readLine(&cursor_, stop_);
  }
  else if (mode == splitmode::wrapped_text){
    paragraph_ = readParagraph(&cursor_, stop_);
  }
}

const std::string&
SentenceStream::
error_message() const {
  return error_message_;
}

bool
SentenceStream::
operator>>(string_view& snt){

  if (error_message_.size())
    return false;

  if (paragraph_.size() == 0 && cursor_ == stop_) {
    // We have reached the end of the data.
    return false;
  }

  if (mode_ == splitmode::one_sentence_per_line){
    snt = readLine(&cursor_, stop_);
  }
  else if (paragraph_.size() == 0){
    // No more sentences in this paragraph.
    // Read the next paragraph but for this call return
    // and empty sentence to indicate the end of this paragraph.
    snt = string_view();
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
  string_view s;
  if (!((*this) >> s))
    return false;
  single_line(snt, s, "", false);
  return true;
};

}}

#pragma once
#include <stdio.h>
#include <string.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include "ssplit_typedefs.h"

// inspired by https://github.com/luvit/pcre2/blob/master/src/pcre2demo.c

namespace ug {
namespace ssplit {

class Match;

class Regex {
  std::string pattern_string_;
  uint32_t option_bits_;
  uint32_t newline_;

  PCRE2_SPTR pattern_;   /* PCRE2_SPTR is a pointer to unsigned code units of
                          * the appropriate width (8, 16, or 32 bits). */
  PCRE2_SPTR name_table_;

  PCRE2_SIZE error_offset_;
  int error_number_;
  pcre2_code* const re_;
public:
  ~Regex();

  Regex(std::string pattern, // pattern to be compiled
        uint32_t options, // pcre2 options for regex compilation
        uint32_t jit_options = PCRE2_JIT_COMPLETE); // options for jit compilation

  int find(string_view subj, // the string (view) agains we are matching
           Match* M,               // where to store the results of the match
           size_t start = 0,       // where to start searching in the string
           uint32_t options=0      // search options
           ) const;

  int consume(string_view* subj, // the string (view) agains we are matching
              Match* M,               // where to store the results of the match
              uint32_t options=0      // search options
              ) const;

  pcre2_code const* get_pcre2_code() const; // return compiled regex
  std::string get_error_message() const; // return error message
  bool OK() const; // return true if pattern compiled successfully, false otherwise
};

class Match {
public:
  pcre2_match_data* const match_data; // stores matching offsets
  char const* data{nullptr};          // beginning of subject text span
  int num_matched_groups{0};
  string_view operator[](int i) const;
  Match(pcre2_code const* re);
  Match(Regex const& re);
  ~Match();
};

}} // end of namespace

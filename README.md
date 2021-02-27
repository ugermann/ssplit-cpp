# ssplit-cpp
This is an approximate reimplementation of the sentence splitter from the Moses toolkit.

- Currently doesn't support CJK character sets.
- requires the pcre2 libraries
  On Ubuntu, `sudo apt-get install libpcre2` should do the trick
- The (non-breaking) prefix files included were originally copied from the Moses toolkit
  but may have been edited there or here since they were copied.

## Build instructions
```
mkdir build
cd build
cmake ..
make -j
```
This produces an executable `ssplit`.

## Usage

### Command line:

run `ssplit -h` for usage instructions.

### In Code (Example)

```
#include "ssplit.h"

...

std::string prefix_file = "path/to/moses-style/prefix-file";
ug::ssplit::SentenceSplitter ssplit(prefix_file);

...

std::string chunk_of_text = "Sentence one. Sentence two.\nSentence three. Sentence four.";
ug::ssplit::splitmode mode = ug::ssplit::splitmode::one_paragraph_per_line;
ug::sssplit::SentenceStream sentence_stream(chunk_of_text, ssplit, mode);
std::string_view snt;

while(sentence_stream >> snt) { // false means end of chunk
  if (snt.size() == 0) {
    // empty string_view means end of paragraph except in one_sentence_per_line mode,
    // which just returns one line (minus leading and training whitespace) at a time
    // For one_paragraph_per_line each empty paragraph results in snt.size() == 0
    // twice in a row: first the em
    ...
  }
  else { // this is the next non-empty paragraph
    ...
  }
}
```

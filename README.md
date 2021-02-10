# ssplit-cpp
This is an approximate reimplementation of the sentence splitter from the Moses toolkit.

- Currently doesn't support CJK character sets.
- requires the pcre2 libraries
  On Ubuntu, `sudo apt-get install libpcre2` should do the trick
  
## Build instructions
```
mkdir build
cd build
cmake ..
make -j
```
This produces an executable `ssplit`.

## Usage

```
cat <text with one paragraph per line> | ssplit -p <path to nonbreaking_prefix file> -m<mode>
```

where `mode` is one of the following letters:

- s: input is one sentence per line, output is one sentence per line
- p: input is one paragraph per line, which will be split into sentences. Output is one sentence per line, with a blank line between paragraphs
- w: input is wrapped text, with paragraphes separated by one or more bank lines. The output is the same as in "p" mode








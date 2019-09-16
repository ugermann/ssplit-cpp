# ssplit-cpp
This is an approximate reimplementation of the sentence splitter from the Moses toolkit.

- Currently doesn't support CJK character sets.
- requires the pcrecpp libraries
  On Ubuntu, `sudo apt-get install libpcre3 and libpcre3-cpp` should do the trick
  
## Build instructions
```
mkdir build
cd build
cmake ..
make -j
```
This produces an executable `ssplit`.

## Usage

cat \<text with one paragraph per line\> | ssplit \<path to nonbreaking_prefix file\>






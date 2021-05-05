#include "CLI11.hpp"
#include "ssplit.h"

#include <fstream>
#include <sstream>

int main(int argc, char *argv[]) {
  // CLI configurations
  using splitmode = ug::ssplit::SentenceStream::splitmode;
  splitmode mode = splitmode::wrapped_text; // split mode
  std::string modespec = "w";
  std::string prefix_file, input_file, expected_output_file;

  CLI::App app{"Sentence Splitter"};
  app.add_option("-p,--prefix-file", prefix_file,
                 "File with nonbreaking prefixes.")
      ->check(CLI::ExistingFile);
  app.add_option("-i,--input", input_file, "Path to input file.")
      ->check(CLI::ExistingFile);
  app.add_option("-e,--expected", expected_output_file, "Path to input file.")
      ->check(CLI::ExistingFile);
  app.add_option("-m,--mode", modespec,
                 "Split mode: "
                 "s (one sentence per line), "
                 "p (one paragraph per line), "
                 "w (wrapped text).")
      ->default_str(modespec)
      ->check(CLI::IsMember({"w", "s", "p"}));

  bool debug{false};
  app.add_option("-d,--debug", debug);

  CLI11_PARSE(app, argc, argv);

  mode = (modespec == "w" ? splitmode::wrapped_text
                          : modespec == "p" ? splitmode::one_paragraph_per_line
                                            : splitmode::one_sentence_per_line);

  ug::ssplit::SentenceSplitter splitter(prefix_file);

  std::ifstream input_stream(input_file);
  if (!input_stream) {
    std::cerr << "Fatal: unable to open input_file " << input_file;
    std::cerr << std::endl;
    return 1;
  }

  std::ifstream expected_stream(expected_output_file);
  if (!expected_stream) {
    std::cerr << "Fatal: unable to open expected output file"
              << expected_output_file;
    std::cerr << std::endl;
    return 1;
  }

  // Read a large blob of text from input-file
  std::ostringstream buffer;
  buffer << input_stream.rdbuf();
  std::string text = buffer.str();

  std::ostringstream expected_buffer;
  expected_buffer << expected_stream.rdbuf();

  if (debug) {
    std::cout << "---- Input:\n" << text;
    std::cout << "\n---- Expected:\n" << expected_buffer.str();
  }

  // Convert to a string_view, and retain string.
  ug::ssplit::string_view text_view(text);
  ug::ssplit::SentenceStream sentence_stream(text_view, splitter, mode);

  const char *p = text_view.data();

  int line{0};
  ug::ssplit::string_view sentence;
  std::ostringstream output_buffer;

  while (sentence_stream >> sentence) {
    if (sentence.size()) {
      ++line;

      // Print non-sentence part.
      ug::ssplit::string_view pre(p, sentence.data() - p);
      output_buffer << pre;

      // Annotate extracted sentence.
      output_buffer << "<" << line << ">" << sentence << "</" << line << ">";

      // Increment pointer to end of current sentence.
      p = sentence.data() + sentence.size();
    }
  }

  // Print remaining part of text.
  const char *end = text_view.data() + text_view.size();
  ug::ssplit::string_view tail(p, end - p);
  if (tail.size()) {
    output_buffer << tail;
  }

  if (debug) {
    std::cout << "\n--- Generated:\n";
  }
  std::cout << output_buffer.str();

  int success = (expected_buffer.str() == output_buffer.str());
  if (!success) {
    std::cout << "Output != Expected\n";
    return 1;
  }
  return 0;
}

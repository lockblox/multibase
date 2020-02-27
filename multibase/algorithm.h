#pragma once

#include <string>

namespace multibase {

class algorithm {
 public:
  virtual ~algorithm() = default;

  /** Resets the algorithm */
  virtual void reset() {}

  /** Returns the input size required to decode a single block */
  virtual std::size_t block_size() { return 0; }

  /** Returns the size of a processed block */
  virtual std::size_t output_size() { return 0; }

  /** Processes an input block returning any intermediate result */
  virtual std::string process(std::string_view input) = 0;

  /** Returns the final output */
  virtual std::string final() { return std::string(); }
};

}  // namespace multibase

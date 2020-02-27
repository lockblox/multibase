#pragma once

#include <multibase/algorithm.h>

namespace multibase {

class identity_algorithm {
 public:
  class encoder : public algorithm {
   public:
    std::string process(std::string_view input) override;
  };

  class decoder : public algorithm {
   public:
    std::string process(std::string_view input) override;
  };
};
}  // namespace multibase

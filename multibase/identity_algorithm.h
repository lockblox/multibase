#pragma once

#include <multibase/codec.h>

namespace multibase {

class identity_algorithm {
 public:
  class encoder : public codec::algorithm {
   public:
    std::string process(std::string_view input) override;
  };

  class decoder : public codec::algorithm {
   public:
    std::string process(std::string_view input) override;
  };
};
}  // namespace multibase

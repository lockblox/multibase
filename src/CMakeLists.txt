target_sources(
  libmultibase
  PRIVATE multibase/basic_algorithm.cpp
          multibase/encoding.cpp
          multibase/codec.cpp
          multibase/encoding_case.cpp
          multibase/encoding_metadata.cpp
          multibase/encoding_traits.cpp
          multibase/log.cpp)

target_sources(multibase PRIVATE multibase/main.cpp)
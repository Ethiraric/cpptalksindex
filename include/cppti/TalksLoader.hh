#ifndef CPPTI_TALKSLOADER
#define CPPTI_TALKSLOADER

#include <istream>
#include <string_view>

#include <cppti/Expected.hpp>
#include <cppti/Talk.hh>

namespace cppti
{

class TalksLoader
{
public:
  static Expected<Talks> loadFrom(std::string_view file);
  static Expected<Talks> loadFrom(std::istream& ifs);
};
}

#endif /* !CPPTI_TALKSLOADER */

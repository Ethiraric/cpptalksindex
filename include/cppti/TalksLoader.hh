#ifndef CPPTI_TALKSLOADER
#define CPPTI_TALKSLOADER

#include <istream>
#include <string>
#include <string_view>
#include <vector>

#include <cppti/Expected.hpp>

namespace cppti
{
struct Talk
{
  std::vector<std::string> speakers;
  std::string title;
  std::string conference;
  int64_t year;
  std::string link;
};
using Talks = std::vector<Talk>;

class TalksLoader
{
public:
  static Expected<Talks> loadFrom(std::string_view file);
  static Expected<Talks> loadFrom(std::istream& ifs);
};
}

#endif /* !CPPTI_TALKSLOADER */

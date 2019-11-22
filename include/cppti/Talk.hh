#ifndef CPPTI_TALK
#define CPPTI_TALK

#include <string>
#include <vector>

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
}

#endif /* !CPPTI_TALK */

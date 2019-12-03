#ifndef CPPTI_TALK
#define CPPTI_TALK

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace cppti
{
struct Talk
{
  std::vector<std::string> speakers;
  std::string title;
  std::string conference;
  int64_t year;
  std::string link;
  std::vector<std::string> tags;
};
using Talks = std::vector<Talk>;
}

namespace nlohmann
{
void from_json(json const& j, cppti::Talk& x);
void to_json(json& j, cppti::Talk const& x);

inline void from_json(json const& j, cppti::Talk& x)
{
  x.speakers = j.at("speakers").get<std::vector<std::string>>();
  x.title = j.at("title").get<std::string>();
  x.conference = j.at("conference").get<std::string>();
  x.year = j.at("year").get<int64_t>();
  x.link = j.at("link").get<std::string>();
  x.tags = j.at("tags").get<std::vector<std::string>>();
}

inline void to_json(json& j, cppti::Talk const& x)
{
  j = json::object();
  j["speakers"] = x.speakers;
  j["title"] = x.title;
  j["conference"] = x.conference;
  j["year"] = x.year;
  j["link"] = x.link;
  j["tags"] = x.tags;
}
}

#endif /* !CPPTI_TALK */

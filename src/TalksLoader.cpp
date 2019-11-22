#include <cppti/TalksLoader.hh>

#include <fstream>

#include <nlohmann/json.hpp>

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

namespace cppti
{
Expected<Talks> TalksLoader::loadFrom(std::string_view file)
{
  auto ifs = std::ifstream{std::string{file}};
  if (!ifs)
    return tl::unexpected{
        ErrContext{ErrCode::DB_FAILED_TO_OPEN, "Failed to open database."}};
  return loadFrom(ifs);
}

Expected<Talks> TalksLoader::loadFrom(std::istream& ifs)
{
  auto json = nlohmann::json{};
  auto talks = Talks{};

  try
  {
    ifs >> json;
    talks = json.get<Talks>();
  }
  catch (nlohmann::json::exception const& e)
  {
    return tl::unexpected{ErrContext{ErrCode::DB_INVALID_FORMAT, e.what()}};
  }
  return talks;
}
}

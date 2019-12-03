#include <cppti/TalksLoader.hh>

#include <fstream>

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

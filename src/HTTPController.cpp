#include <cppti/HTTPController.hh>

#include <vector>

namespace cppti
{
namespace
{
std::vector<std::string_view> splitTags(std::string_view tags)
{
  if (tags.empty())
    return {};

  auto ret = std::vector<std::string_view>{};
  auto it = tags.begin();
  auto find_it = std::find(tags.begin(), tags.end(), ',');
  while (find_it != tags.end())
  {
    ret.emplace_back(&*it, find_it - it);
    it = find_it + 1;
    find_it = std::find(it, tags.end(), ',');
  }
  ret.emplace_back(&*it, find_it - it);
  return ret;
}
}

HTTPController::HTTPController(TalksDB const& db)
  : talksdb{db}, server{std::make_unique<httplib::Server>()}
{
  this->server->Get(R"(/api/talks)", [this](auto const& req, auto& res) {
    return this->routeTalks(req, res);
  });
}

void HTTPController::listen(std::string_view host, std::uint16_t port)
{
  this->server->listen(host.data(), port);
}

void HTTPController::routeTalks(Request const& req, Response& res)
{
  auto const speaker = req.get_param_value("speaker");
  auto const conference = req.get_param_value("conference");
  auto const raw_tags = req.get_param_value("tags");
  auto const tags = splitTags(raw_tags);
  auto const rawyear = req.get_param_value("year");
  auto const year = rawyear.empty() ? 0 : std::stoi(rawyear);
  auto const& talks = this->talksdb.get().get(speaker, conference, tags, year);

  nlohmann::json const json = talks;
  res.set_content(json.dump(), "application/json");
}
}

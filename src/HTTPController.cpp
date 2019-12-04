#include <cppti/HTTPController.hh>

#include <algorithm>
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
  this->server->Get(R"(/api/speakers)", [this](auto const& req, auto& res) {
    return this->routeSpeakers(req, res);
  });
  this->server->Get(R"(/api/conferences)", [this](auto const& req, auto& res) {
    return this->routeConferences(req, res);
  });
  this->server->Get(R"(/api/tags)", [this](auto const& req, auto& res) {
    return this->routeTags(req, res);
  });
  this->server->Get(R"(/api/years)", [this](auto const& req, auto& res) {
    return this->routeYears(req, res);
  });
  this->server->set_base_dir("./www");
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
  if (!std::all_of(rawyear.begin(), rawyear.end(), [&](char c) {
        return std::isdigit(c);
      }))
    return;
  auto const year = rawyear.empty() ? 0 : std::stoi(rawyear);
  auto const& talks = this->talksdb.get().get(speaker, conference, tags, year);

  nlohmann::json const json = talks;
  res.set_content(json.dump(), "application/json");
}

void HTTPController::routeSpeakers(Request const&, Response& res)
{
  auto const speakers = this->talksdb.get().getSpeakerList();
  nlohmann::json const json = speakers;
  res.set_content(json.dump(), "application/json");
}

void HTTPController::routeConferences(Request const&, Response& res)
{
  auto const conferences = this->talksdb.get().getConferenceList();
  nlohmann::json const json = conferences;
  res.set_content(json.dump(), "application/json");
}

void HTTPController::routeTags(Request const&, Response& res)
{
  auto const tags = this->talksdb.get().getTagList();
  nlohmann::json const json = tags;
  res.set_content(json.dump(), "application/json");
}

void HTTPController::routeYears(Request const&, Response& res)
{
  auto const years = this->talksdb.get().getYearList();
  nlohmann::json const json = years;
  res.set_content(json.dump(), "application/json");
}
}

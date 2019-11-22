#include <cppti/TalksDB.hh>

#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/take.hpp>

namespace cppti
{
namespace
{
std::string toConfIndex(std::string_view confname, int64_t year)
{
  return std::string{confname} + std::to_string(year);
}
}

TalksDB::TalksDB(std::vector<Talk>&& ptalks) : talks{std::move(ptalks)}
{
}

void TalksDB::index()
{
  this->by_speaker.clear();
  for (auto const& talk : this->talks)
  {
    // Index by speaker
    for (auto const& speaker : talk.speakers)
      this->by_speaker[speaker].emplace_back(talk);

    // Index by conference
    this->by_conference[toConfIndex(talk.conference, talk.year)].emplace_back(
        talk);
  }
}

std::vector<std::reference_wrapper<Talk const>> TalksDB::get(
    std::string_view speaker,
    std::string_view conference,
    int64_t year,
    int64_t maxresults) const
{
  auto ret = std::vector<std::reference_wrapper<Talk const>>{};
  if (!speaker.empty())
  {
    auto const speakerit = this->by_speaker.find(std::string{speaker});
    if (speakerit == this->by_speaker.end())
      return {};
    auto range = speakerit->second |
                 ranges::views::filter([&](Talk const& talk) {
                   return conference.empty() || talk.conference == conference;
                 }) |
                 ranges::views::filter([&](Talk const& talk) {
                   return year == 0 || talk.year == year;
                 }) |
                 ranges::views::take(maxresults);
    ranges::for_each(range, [&](auto const& talk) { ret.emplace_back(talk); });
  }
  else
  {
    auto const confit = this->by_conference.find(std::string{conference});
    if (confit == this->by_conference.end())
      return {};
    auto range = confit->second | ranges::views::filter([&](Talk const& talk) {
                   return year == 0 || talk.year == year;
                 }) |
                 ranges::views::take(maxresults);
    ranges::for_each(range, [&](auto const& talk) { ret.emplace_back(talk); });
  }
  return ret;
}
}

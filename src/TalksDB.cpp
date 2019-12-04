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

auto getConferenceFilter(std::string_view conference) noexcept
{
  return [=](Talk const& talk) {
    return conference.empty() || talk.conference == conference;
  };
}

auto getTagsFilter(gsl::span<std::string_view const> tags) noexcept
{
  return [=](Talk const& talk) {
    for (auto const tag : tags)
      if (std::find(talk.tags.begin(), talk.tags.end(), tag) == talk.tags.end())
        return false;
    return true;
  };
}

auto getYearFilter(int64_t year) noexcept
{
  return [=](Talk const& talk) { return year == 0 || talk.year == year; };
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

    // Index by tag
    for (auto const& tag : talk.tags)
      this->by_tag[tag].emplace_back(talk);
  }
}

std::vector<std::reference_wrapper<Talk const>> TalksDB::get(
    std::string_view speaker,
    std::string_view conference,
    gsl::span<std::string_view const> tags,
    int64_t year,
    int64_t maxresults) const
{
  auto filter_year_and_convert = [&](auto range) {
    auto ret = std::vector<std::reference_wrapper<Talk const>>{};
    auto filtered_range = range | ranges::views::filter(getYearFilter(year)) |
                          ranges::views::take(maxresults);
    ranges::for_each(filtered_range,
                     [&](auto const& talk) { ret.emplace_back(talk); });
    return ret;
  };

  if (!speaker.empty())
  {
    auto const speakerit = this->by_speaker.find(std::string{speaker});
    if (speakerit == this->by_speaker.end())
      return {};
    auto range = speakerit->second |
                 ranges::views::filter(getConferenceFilter(conference)) |
                 ranges::views::filter(getTagsFilter(tags));
    return filter_year_and_convert(range);
  }
  else if (!conference.empty())
  {
    auto const confit = this->by_conference.find(std::string{conference});
    if (confit == this->by_conference.end())
      return {};
    auto range = confit->second | ranges::views::filter(getTagsFilter(tags));
    return filter_year_and_convert(range);
  }
  else if (!tags.empty())
  {
    auto const tagit = this->by_tag.find(std::string{tags[0]});
    if (tagit == this->by_tag.end())
      return {};
    auto range =
        tagit->second | ranges::views::filter(getTagsFilter(tags.subspan(1)));
    return filter_year_and_convert(range);
  }

  // If none of `speaker`, `conference` and `tags` are specified, there are too
  // much result to filter. Save the planet, save CPU cycles.
  return {};
}

std::vector<std::string> TalksDB::getSpeakerList() const
{
  auto ret = std::vector<std::string>{};
  for (auto const& [speaker, _] : this->by_speaker)
    ret.emplace_back(speaker);
  return ret;
}

std::vector<std::string> TalksDB::getConferenceList() const
{
  auto ret = std::vector<std::string>{};
  for (auto const& [conference, _] : this->by_conference)
    ret.emplace_back(conference);
  return ret;
}

std::vector<std::string> TalksDB::getTagList() const
{
  auto ret = std::vector<std::string>{};
  for (auto const& [tag, _] : this->by_tag)
    ret.emplace_back(tag);
  return ret;
}
}

#include <cppti/TalksDB.hh>

#include <set>

#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/take.hpp>

#include <cppti/Utils.hh>

namespace cppti
{
namespace
{
auto getConferenceFilter(std::string_view conference) noexcept
{
  return [=](TalksDB::TalkRef const& talk) {
    return conference.empty() || talk.conference == conference;
  };
}

auto getTagsFilter(gsl::span<std::string_view const> tags) noexcept
{
  return [=](TalksDB::TalkRef const& talk) {
    for (auto const tag : tags)
      if (std::find(talk.tags.begin(), talk.tags.end(), tag) == talk.tags.end())
        return false;
    return true;
  };
}

auto getYearFilter(int64_t year) noexcept
{
  return [=](TalksDB::TalkRef const& talk) {
    return year == 0 || talk.year == year;
  };
}

template <typename T, typename GetterFunction>
std::vector<T> listAllUnique(std::vector<Talk> const& talks,
                             GetterFunction get_attr)
{
  auto ret = std::vector<T>{};
  auto set = std::set<T>{};
  for (auto const& talk : talks)
  {
    if constexpr (std::is_same_v<
                      std::invoke_result_t<GetterFunction, Talk const&>,
                      std::vector<T>>)
      // Emplace each element if attribute is a vector.
      for (auto const& attr : get_attr(talk))
        set.emplace(attr);
    else
      // Otherwise just emplace the attribute.
      set.emplace(get_attr(talk));
  }
  ret.reserve(set.size());
  for (auto const& item : set)
    ret.emplace_back(item);
  return ret;
}
}

TalksDB::TalksDB(std::vector<Talk>&& ptalks)
{
  this->index(std::move(ptalks));
}

void TalksDB::index(std::vector<Talk>&& ptalks)
{
  for (auto&& speaker : listAllUnique<std::string>(
           ptalks, [&](auto const& talk) { return talk.speakers; }))
  {
    auto id = speaker;
    toSnakeCase(id);
    this->speakers.emplace_back(Speaker{std::move(id), std::move(speaker)});
  }
  this->conferences = listAllUnique<std::string>(
      ptalks, [&](auto const& talk) { return talk.conference; });
  this->tags = listAllUnique<std::string>(
      ptalks, [&](auto const& talk) { return talk.tags; });
  this->years = listAllUnique<std::int64_t>(
      ptalks, [&](auto const& talk) { return talk.year; });

  this->by_speaker.clear();
  this->by_conference.clear();
  this->by_tag.clear();

  this->talks.reserve(ptalks.size());
  for (auto const& talk : ptalks)
  {
    auto const& talkref = this->talks.emplace_back(this->talkRefFromTalk(talk));
    // Index by speaker
    for (auto const& speaker : talk.speakers)
    {
      auto id = speaker;
      toSnakeCase(id);
      this->by_speaker[id].emplace_back(talkref);
    }

    // Index by conference
    this->by_conference[talk.conference].emplace_back(talkref);

    // Index by tag
    for (auto const& tag : talk.tags)
      this->by_tag[tag].emplace_back(talkref);
  }
}

std::vector<std::reference_wrapper<TalksDB::TalkRef const>> TalksDB::get(
    std::string_view speaker,
    std::string_view conference,
    gsl::span<std::string_view const> filtertags,
    int64_t year,
    int64_t maxresults) const
{
  auto filter_year_and_convert = [&](auto range) {
    auto ret = std::vector<std::reference_wrapper<TalkRef const>>{};
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
                 ranges::views::filter(getTagsFilter(filtertags));
    return filter_year_and_convert(range);
  }
  else if (!conference.empty())
  {
    auto const confit = this->by_conference.find(std::string{conference});
    if (confit == this->by_conference.end())
      return {};
    auto range =
        confit->second | ranges::views::filter(getTagsFilter(filtertags));
    return filter_year_and_convert(range);
  }
  else if (!filtertags.empty())
  {
    auto const tagit = this->by_tag.find(std::string{filtertags[0]});
    if (tagit == this->by_tag.end())
      return {};
    auto range = tagit->second |
                 ranges::views::filter(getTagsFilter(filtertags.subspan(1)));
    return filter_year_and_convert(range);
  }

  // If none of `speaker`, `conference` and `filtertags` are specified, there
  // are too much result to filter. Save the planet, save CPU cycles.
  return {};
}

std::vector<TalksDB::Speaker> const& TalksDB::getSpeakerList() const
{
  return this->speakers;
}

std::vector<std::string> const& TalksDB::getConferenceList() const
{
  return this->conferences;
}

std::vector<std::string> const& TalksDB::getTagList() const
{
  return this->tags;
}

std::vector<std::int64_t> const& TalksDB::getYearList() const
{
  return this->years;
}

TalksDB::TalkRef TalksDB::talkRefFromTalk(Talk const& talk)
{
  auto ret = TalkRef{};

  ret.speakers.reserve(talk.speakers.size());
  for (auto const& speaker : talk.speakers)
  {
    auto id = speaker;
    toSnakeCase(id);
    auto const& index_speaker = *std::lower_bound(
        this->speakers.begin(),
        this->speakers.end(),
        id,
        [&](auto const& hay, auto const& needle) { return hay.id < needle; });
    ret.speakers.emplace_back(
        TalkRef::Speaker{index_speaker.id, index_speaker.display_name});
  }

  ret.conference = *std::lower_bound(
      this->conferences.begin(), this->conferences.end(), talk.conference);

  ret.tags.reserve(talk.tags.size());
  for (auto const& tag : talk.tags)
    ret.tags.emplace_back(
        *std::lower_bound(this->tags.begin(), this->tags.end(), tag));

  ret.title = talk.title;
  ret.year = talk.year;
  ret.link = talk.link;
  return ret;
}
}

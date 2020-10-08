#ifndef CPPTI_TALKSDB
#define CPPTI_TALKSDB

#include <functional>
#include <optional>
#include <unordered_map>

#include <gsl/span>

#include <cppti/Talk.hh>

namespace cppti
{
class TalksDB
{
public:
  struct TalkRef
  {
    struct Speaker
    {
      std::string_view id;
      std::string_view display_name;
    };

    std::vector<Speaker> speakers;
    std::string title;
    std::optional<std::string> slides;
    std::string_view conference;
    int64_t year;
    std::string link;
    std::vector<std::string_view> tags;
  };

  struct Speaker
  {
    std::string id;
    std::string display_name;
  };

  explicit TalksDB(std::vector<Talk>&& ptalks);
  TalksDB(TalksDB const& b) noexcept = delete;
  TalksDB(TalksDB&& b) noexcept = default;
  ~TalksDB() noexcept = default;

  TalksDB& operator=(TalksDB const& rhs) noexcept = delete;
  TalksDB& operator=(TalksDB&& rhs) noexcept = default;

  void index(std::vector<Talk>&& ptalks);

  std::vector<std::reference_wrapper<TalkRef const>> get(
      std::string_view speaker = {},
      std::string_view conference = {},
      gsl::span<std::string_view const> tags = {},
      int64_t year = 0,
      int64_t maxresults = 50) const;

  std::vector<Speaker> const& getSpeakerList() const;
  std::vector<std::string> const& getConferenceList() const;
  std::vector<std::string> const& getTagList() const;
  std::vector<std::int64_t> const& getYearList() const;

private:
  TalkRef talkRefFromTalk(Talk const& talk);

  std::vector<TalkRef> talks;
  std::unordered_map<std::string,
                     std::vector<std::reference_wrapper<TalkRef const>>>
      by_speaker;
  std::unordered_map<std::string,
                     std::vector<std::reference_wrapper<TalkRef const>>>
      by_conference;
  std::unordered_map<std::string,
                     std::vector<std::reference_wrapper<TalkRef const>>>
      by_tag;

  std::vector<Speaker> speakers;
  std::vector<std::string> conferences;
  std::vector<std::string> tags;
  std::vector<std::int64_t> years;
};
}

namespace nlohmann
{
void to_json(json& j, cppti::TalksDB::TalkRef::Speaker const& x);
void to_json(json& j, cppti::TalksDB::Speaker const& x);
void to_json(json& j, cppti::TalksDB::TalkRef const& x);

inline void to_json(json& j, cppti::TalksDB::TalkRef::Speaker const& x)
{
  j = json::object();
  j["id"] = x.id;
  j["display_name"] = x.display_name;
}

inline void to_json(json& j, cppti::TalksDB::Speaker const& x)
{
  j = json::object();
  j["id"] = x.id;
  j["display_name"] = x.display_name;
}

inline void to_json(json& j, cppti::TalksDB::TalkRef const& x)
{
  j = json::object();
  j["speakers"] = x.speakers;
  j["title"] = x.title;
  if (x.slides)
    j["slides"] = *x.slides;
  j["conference"] = x.conference;
  j["year"] = x.year;
  j["link"] = x.link;
  j["tags"] = x.tags;
}
}

#endif /* !CPPTI_TALKSDB */

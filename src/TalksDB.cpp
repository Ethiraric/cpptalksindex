#include <cppti/TalksDB.hh>

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
    this->by_conference[toConfIndex(talk.conference, talk.year)].emplace_back(talk);
  }
}
}

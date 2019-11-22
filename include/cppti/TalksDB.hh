#ifndef CPPTI_TALKSDB
#define CPPTI_TALKSDB

#include <functional>
#include <unordered_map>

#include <cppti/Talk.hh>

namespace cppti
{
class TalksDB
{
public:
  explicit TalksDB(std::vector<Talk>&& ptalks);
  TalksDB(TalksDB const& b) noexcept = delete;
  TalksDB(TalksDB&& b) noexcept = default;
  ~TalksDB() noexcept = default;

  TalksDB& operator=(TalksDB const& rhs) noexcept = delete;
  TalksDB& operator=(TalksDB&& rhs) noexcept = default;

  void index();

private:
  std::vector<Talk> talks;
  std::unordered_map<std::string,
                     std::vector<std::reference_wrapper<Talk const>>>
      by_speaker;
  std::unordered_map<std::string,
                     std::vector<std::reference_wrapper<Talk const>>>
      by_conference;
};
}

#endif /* !CPPTI_TALKSDB */

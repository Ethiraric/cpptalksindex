#include <iostream>

#include <cppti/TalksDB.hh>
#include <cppti/TalksLoader.hh>

namespace
{
template <typename T>
using Expected = cppti::Expected<T>;
using Talk = cppti::Talk;
using TalksDB = cppti::TalksDB;

Expected<TalksDB> buildDB(std::vector<Talk> talks)
{
  auto db = TalksDB{std::move(talks)};
  db.index();
  return db;
}
}

int main(int, char const* const* argv)
{
  return cppti::TalksLoader::loadFrom(argv[1])
      .and_then(buildDB)
      .and_then([&](auto db) -> Expected<int> {
        std::string speaker;
        std::getline(std::cin, speaker, ',');
        std::string conference;
        std::getline(std::cin, conference, ',');
        int64_t year{0};
        std::cin >> year;
        std::cout << "Looking for: " << speaker << "," << conference << ","
                  << year << '\n';
        auto const res = db.get(speaker, conference, {}, year);
        for (auto const& talk : res)
          std::cout << "\t[" << talk.get().conference << "] "
                    << talk.get().speakers[0] << " - " << talk.get().title
                    << '\n';
        return 0;
      })
      .value_or(1);
}

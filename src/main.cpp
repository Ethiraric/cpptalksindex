#include <iostream>

#include <cppti/TalksLoader.hh>
#include <cppti/TalksDB.hh>

int main(int, char const* const* argv)
{
  return cppti::TalksLoader::loadFrom(argv[1])
      .and_then([&](auto talks) -> cppti::Expected<int> {
        auto db = cppti::TalksDB{std::move(talks)};
        db.index();
        return 0;
      })
      .value_or(1);
}

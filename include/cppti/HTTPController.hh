#ifndef CPPTI_HTTPCONTROLLER_HH_
#define CPPTI_HTTPCONTROLLER_HH_

#include <memory>

#include <httplib.h>

#include <cppti/TalksDB.hh>

namespace cppti
{
class HTTPController
{
public:
  explicit HTTPController(TalksDB const& db);
  HTTPController(HTTPController const& b) noexcept = delete;
  HTTPController(HTTPController&& b) noexcept = default;
  ~HTTPController() noexcept = default;

  HTTPController& operator=(HTTPController const& rhs) noexcept = delete;
  HTTPController& operator=(HTTPController&& rhs) noexcept = default;

  void listen(std::string_view host, std::uint16_t port);

private:
  using Request = httplib::Request;
  using Response = httplib::Response;

  void routeTalks(Request const& req, Response& res);
  void routeSpeakers(Request const& req, Response& res);
  void routeConferences(Request const& req, Response& res);
  void routeTags(Request const& req, Response& res);
  void routeYears(Request const& req, Response& res);
  void routeFilters(Request const& req, Response& res);

  std::reference_wrapper<TalksDB const> talksdb;
  std::unique_ptr<httplib::Server> server;
};
}

#endif /* !CPPTI_HTTPCONTROLLER_HH_ */

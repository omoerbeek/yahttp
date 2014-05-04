#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include "yahttp/yahttp.hpp"
#include "yahttp/router.hpp"

using namespace boost;

class RouteTargetHandler {
public:
  static std::map<std::string, bool> routes;

  static bool Handler(YaHTTP::Request *req, YaHTTP::Response *resp) {
    std::cout << "Hello, got " << req->routeName << std::endl;
    routes[req->routeName] = true;
  }
} rth;

std::map<std::string, bool> RouteTargetHandler::routes;

struct RouterFixture {
  RouterFixture() { 
    BOOST_TEST_MESSAGE("Setup router");
    YaHTTP::Router::Get("/test/<object>/<attribute>.<format>", rth.Handler, "object_attribute_format_get");
    YaHTTP::Router::Map("patch", "/test/<object>/<attribute>.<format>", rth.Handler, "object_attribute_format_update");
    YaHTTP::Router::Get("/test", rth.Handler, "test_index");
    YaHTTP::Router::Get("/", rth.Handler, "root_path");

    // reset routes to false
    BOOST_FOREACH(YaHTTP::TRoute route, YaHTTP::Router::GetRoutes()) {
#ifdef HAVE_CXX11
      rth.routes[std::get<3>(route)] = false;
#else
      rth.routes[route.get<3>()] = false;
#endif
    }

    // print routes
    YaHTTP::Router::PrintRoutes(std::cout);
  };

};

BOOST_FIXTURE_TEST_SUITE( test_router, RouterFixture )

BOOST_AUTO_TEST_CASE( test_router_basic ) {
  // setup request
  YaHTTP::Request req;
  YaHTTP::Response resp;
  req.setup("get", "http://test.org/");
  
  BOOST_CHECK(YaHTTP::Router::Route(&req, &resp));

  // check if it was hit
  BOOST_CHECK(rth.routes["root_path"]);
};
 
}
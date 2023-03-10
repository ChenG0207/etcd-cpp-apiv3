#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <chrono>
#include <thread>

#include "etcd/Watcher.hpp"
#include "etcd/SyncClient.hpp"

static const std::string etcd_url = etcdv3::detail::resolve_etcd_endpoints("http://127.0.0.1:2379");

static int watcher_called = 0;

void printResponse(etcd::Response const & resp)
{
  if (resp.error_code()) {
    std::cout << resp.error_code() << ": " << resp.error_message() << std::endl;
  }
  else {
    std::cout << resp.action() << " " << resp.value().as_string() << std::endl;
    std::cout << "Previous value: " << resp.prev_value().as_string() << std::endl;

    std::cout << "Events size: " << resp.events().size() << std::endl;
    for (auto const &ev: resp.events()) {
      if (ev.prev_kv().key().find("/leader") == 0 || ev.kv().key().find("/leader") == 0) {
        return;
      }
      std::cout << "Value change in events: " << static_cast<int>(ev.event_type())
                << ", prev kv = " << ev.prev_kv().key() << " -> " << ev.prev_kv().as_string()
                << ", kv = " << ev.kv().key() << " -> " << ev.kv().as_string()
                << std::endl;
    }
  }
  std::cout << "print response called" << std::endl;
  ++watcher_called;
}

TEST_CASE("create watcher")
{
  etcd::SyncClient etcd(etcd_url);
  etcd.rmdir("/test", true);

  watcher_called = 0;
  {
    etcd::Watcher watcher(etcd_url, "/test", printResponse, true);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    etcd.set("/test/key", "42");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    etcd.set("/test/key", "43");
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
  CHECK(2 == watcher_called);
  etcd.rmdir("/test", true);
}

TEST_CASE("watch with correct prefix")
{
  etcd::SyncClient etcd(etcd_url);
  etcd.rmdir("/test", true);

  watcher_called = 0;
  etcd::Watcher watcher(etcd_url, "/test/key_prefix", printResponse, true);

  {
    etcd.set("/test/key1", "42");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    CHECK(0 == watcher_called);
  }
  {
    etcd.set("/test/key_prefix", "42");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    CHECK(1 == watcher_called);
  }
  {
    etcd.set("/test/key_prefix1", "42");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    CHECK(2 == watcher_called);
  }
  {
    etcd.set("/test/key_prefiy", "42");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    CHECK(2 == watcher_called);
  }
  {
    etcd.set("/test/key1", "42");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    CHECK(2 == watcher_called);
  }

  etcd.rmdir("/test", true);
}

TEST_CASE("create watcher with cancel")
{
  etcd::SyncClient etcd(etcd_url);
  etcd.rmdir("/test", true);

  watcher_called = 0;
  etcd::Watcher watcher(etcd_url, "/test", printResponse, true);
  std::this_thread::sleep_for(std::chrono::seconds(5));
  etcd.set("/test/key", "42");
  etcd.set("/test/key", "43");
  etcd.rm("/test/key");
  etcd.set("/test/key", "44");
  std::this_thread::sleep_for(std::chrono::seconds(5));
  CHECK(4 == watcher_called);
  watcher.Cancel();
  etcd.set("/test/key", "50");
  etcd.set("/test/key", "51");
  std::this_thread::sleep_for(std::chrono::seconds(5));
  CHECK(4 == watcher_called);

  etcd.rmdir("/test", true);
}

TEST_CASE("create watcher on ranges with cancel")
{
  etcd::SyncClient etcd(etcd_url);
  etcd.rmdir("/test", true);

  watcher_called = 0;
  etcd::Watcher watcher(etcd_url, "/test/key1", "/test/key5", printResponse);
  std::this_thread::sleep_for(std::chrono::seconds(5));
  etcd.set("/test/key1", "42");
  etcd.set("/test/key2", "43");
  etcd.rm("/test/key1");
  etcd.set("/test/key5", "44");
  std::this_thread::sleep_for(std::chrono::seconds(5));
  CHECK(3 == watcher_called);
  watcher.Cancel();
  etcd.set("/test/key3", "50");
  etcd.set("/test/key4", "51");
  std::this_thread::sleep_for(std::chrono::seconds(5));
  CHECK(3 == watcher_called);

  etcd.rmdir("/test", true);
}

TEST_CASE("watch should exit normally")
{
  // cancel immediately after start watch.
  etcd::Watcher watcher(etcd_url, "/test", printResponse, true);
  watcher.Cancel();
}

TEST_CASE("watch should can be cancelled repeatedly")
{
  // cancel immediately after start watch.
  etcd::Watcher watcher(etcd_url, "/test", printResponse, true);
  std::vector<std::thread> threads(10);
  for (size_t i = 0; i < 10; ++i) {
    threads[i] = std::thread([&]() {
      watcher.Cancel();
    });
  }
  for (size_t i = 0; i < 10; ++i) {
    threads[i].join();
  }
}

// TEST_CASE("request cancellation")
// {
//   etcd::Client etcd(etcd_url);
//   etcd.set("/test/key1", "42").wait();

//   pplx::task<etcd::Response> res = etcd.watch("/test/key1");
//   CHECK(!res.is_done());

//   etcd.cancel_operations();

//   std::this_thread::sleep_for(std::chrono::seconds(1));
//   REQUIRE(res.is_done());
//   try {
//     res.wait();
//   }
//   catch(pplx::task_canceled const & ex) {
//     std::cout << "pplx::task_canceled: " << ex.what() << "\n";
//   }
//   catch(std::exception const & ex) {
//     std::cout << "std::exception: " << ex.what() << "\n";
//   }
// }

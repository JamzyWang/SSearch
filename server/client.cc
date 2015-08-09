#include "gen-cpp/ImageServer.h"

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

#include <iostream>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using boost::shared_ptr;

int main(int argc, char **argv) {
  shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
  shared_ptr<TTransport> transport(new TFramedTransport(socket));
  shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

  ImageServerClient client(protocol);

  std::vector<std::string> sketches = {
    "sketches/0.png",
    "sketches/1.png",
    "sketches/2.png",
    "sketches/4.png",
    "sketches/6.png",
    "sketches/7.png",
    "sketches/8.png",
    "sketches/9.png",
    "sketches/10.png",
    "sketches/11.png",
    "sketches/12.png",
    "sketches/13.png",
    "sketches/14.png",
    "sketches/20.png",
    "sketches/21.png",
    "sketches/23.png",
    "sketches/24.png",
    "sketches/25.png",
    "sketches/26.png",
    "sketches/27.png",
    "sketches/28.png",
    "sketches/30.png",
    "sketches/33.png",
    "sketches/34.png",
    "sketches/38.png",
    "sketches/42.png",
    "sketches/43.png",
    "sketches/45.png",
    "sketches/46.png",
    "sketches/47.png",
    "sketches/48.png"
  };

  transport->open();

  client.reload_func("calc_distance_gf");

  std::cout << "var results={};" << std::endl;;

  for (auto it = sketches.begin(); it != sketches.end(); ++it) {
    std::vector<Pair> result;

    std::cerr << "query for " << *it << std::endl;
    client.query_result(result, *it);

    std::cout << "results['" << *it << "'] = [";
    int len = std::min((size_t)100, result.size());
    for (int i = 0; i < len; ++i)
      std::cout << "['"
                << result[i].filename
                << "', "
                << result[i].result
                << "], ";

    std::cout << "];" << std::endl;
  }

  transport->close();
}

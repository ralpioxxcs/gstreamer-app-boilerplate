#include "consumer.h"
#include "producer.h"

#include <iostream>
#include <memory>

#define WIDTH 1280
#define HEIGHT 720
#define CHANNEL 3

int main() {
  // initialize the GStreamer library
  int argc = 0;
  GError *err{nullptr};
  if (!gst_init_check(&argc, nullptr, &err)) {
    std::cerr << "failed to initialize Gstreamer library (err: " << err->message << ")\n";
    exit(1);
  }

  Producer producer;
  Consumer consumer(WIDTH*HEIGHT*CHANNEL);

  // start producer pipeline
  producer.start();

  // start consumer pipeline
  consumer.start();

  // allocate data array
  //std::shared_ptr<unsigned char*> data = std::make_shared<unsigned char*>(WIDTH * HEIGHT * CHANNEL);
  unsigned char* data = new unsigned char[WIDTH*HEIGHT*CHANNEL];

  while (true) {
    producer.produce(&data);

    // feed to consumer
    consumer.consume(&data);
  }

  producer.stop();

  return 0;
} 
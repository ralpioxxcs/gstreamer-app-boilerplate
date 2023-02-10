#pragma once

#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>
#include <gst/gst.h>
#include <gst/pbutils/pbutils.h>

class Producer {
 public:
  Producer();

  void start();
  void stop();
  void produce(unsigned char **dataPtr);

 private:
  GstElement *m_pipeline;
  GstAppSink *m_appsink;
};
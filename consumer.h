#pragma once

#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>
#include <gst/gst.h>
#include <gst/pbutils/pbutils.h>

class Consumer {
 public:
  Consumer(int size);

  void start();
  void stop();
  void consume(unsigned char **dataPtr);

 private:
  GstElement *m_pipeline;
  GstAppSrc *m_appsrc;

  int m_size;
};
#include "producer.h"

#include <iostream>

Producer::Producer() {
  GError *err{nullptr};
  std::string pipelineStr = "videotestsrc ! video/x-raw, width=1280, height=720, format=RGB ! videoconvert ! appsink name=mysink";
  std::cout << "producer pipeline: " << pipelineStr << std::endl;

  m_pipeline = gst_parse_launch(pipelineStr.c_str(), &err);
  if (err != nullptr) {
    std::cerr << "failed to construct pipeline (err: " << err->message << ")\n";
    exit(1);
  }

  GstPipeline *pl = GST_PIPELINE(m_pipeline);
  if (!pl) {
    std::cerr << "failed to construct pipeline (err: " << err->message << ")\n";
    exit(1);
  }

  m_appsink = GST_APP_SINK(gst_bin_get_by_name(GST_BIN(m_pipeline), "mysink"));
}

void Producer::start() {
  auto result = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
  if (result == GST_STATE_CHANGE_FAILURE) {
    std::cout << "failed to set pipeline state to PLAYING (result: "
              << std::string(gst_element_state_change_return_get_name(result))
              << ")\n";
    exit(1);
  }
}

void Producer::stop() {
  auto result = gst_element_set_state(m_pipeline, GST_STATE_NULL);
  if (result == GST_STATE_CHANGE_FAILURE) {
    std::cout << "failed to set pipeline state to NULL (result: "
              << std::string(gst_element_state_change_return_get_name(result))
              << ")\n";
    exit(1);
  }
}

void Producer::produce(unsigned char **dataPtr) {
  GstSample *sample;
  GstBuffer *buffer;
  GstMapInfo map;

  sample = gst_app_sink_pull_sample(GST_APP_SINK(m_appsink));
  if (!sample) {
    return;
  }

  buffer = gst_sample_get_buffer(sample);
  if (!buffer) {
    return;
  }

  if (!gst_buffer_map(buffer, &map, GST_MAP_READ)) {
    return;
  }

  const void *gstData = map.data;
  const gsize gstSize = map.size;
  auto size = gstSize;
  if (!gstData) {
    gst_sample_unref(sample);
    return;
  }

  memcpy(*dataPtr, gstData, size);

  // free gst buffer
  gst_buffer_unmap(buffer, &map);
  gst_sample_unref(sample);

  return;
}
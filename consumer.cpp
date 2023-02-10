#include "consumer.h"

#include <iostream>

Consumer::Consumer(int size) : m_size(size) {
  GError *err{nullptr};
  std::string pipelineStr = "appsrc name=mysrc ! videoconvert ! autovideosink";
  //std::string pipelineStr = "appsrc name=mysrc ! videoconvert  ! x264enc ! qtmux ! filesink location=test.mp4";
  std::cout << "consumer pipeline: " << pipelineStr << std::endl;

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
  m_appsrc = GST_APP_SRC(gst_bin_get_by_name(GST_BIN(m_pipeline), "mysrc"));

  std::string caps("video/x-raw, width=1280, height=720, format=RGB");

  // set the capability on the appsrc 
  gst_app_src_set_caps(GST_APP_SRC(m_appsrc), gst_caps_from_string(caps.c_str()));
}

void Consumer::start() {
  auto result = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
  if (result == GST_STATE_CHANGE_FAILURE) {
    std::cout << "failed to set pipeline state to PLAYING (result: "
              << std::string(gst_element_state_change_return_get_name(result))
              << ")\n";
    exit(1);
  }
}

void Consumer::stop() {
  auto result = gst_element_set_state(m_pipeline, GST_STATE_NULL);
  if (result == GST_STATE_CHANGE_FAILURE) {
    std::cout << "failed to set pipeline state to NULL (result: "
              << std::string(gst_element_state_change_return_get_name(result))
              << ")\n";
    exit(1);
  }
}

void Consumer::consume(unsigned char **dataPtr) {
  GstFlowReturn fret;

  GstBuffer *gstBuffer = gst_buffer_new_allocate(nullptr, m_size, nullptr);

  // map buffer for writing (image)
  GstMapInfo map;
  if (gst_buffer_map(gstBuffer, &map, GST_MAP_WRITE)) {
    if (map.size != m_size) {
      gst_buffer_unref(gstBuffer);
      return;
    }
    memcpy(map.data, *dataPtr, m_size);
    gst_buffer_unmap(gstBuffer, &map);
  } else {
    gst_buffer_unref(gstBuffer);
    return;
  }

  g_signal_emit_by_name(m_appsrc, "push-buffer", gstBuffer, &fret);
  gst_buffer_unref(gstBuffer);
  if (fret != GST_FLOW_OK) {
    std::cerr << "video appsrc pushed buffer abnormally (result: " << gst_flow_get_name(fret) << ")\n";
  }

  return;
}
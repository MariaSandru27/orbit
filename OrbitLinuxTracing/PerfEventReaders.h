// Copyright (c) 2020 The Orbit Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ORBIT_LINUX_TRACING_PERF_EVENT_READERS_H_
#define ORBIT_LINUX_TRACING_PERF_EVENT_READERS_H_

#include "PerfEvent.h"
#include "PerfEventRingBuffer.h"

namespace LinuxTracing {

// Helper functions for reads from a perf_event_open ring buffer that require
// more complex operations than simply copying an entire perf_event_open record.
pid_t ReadMmapRecordPid(PerfEventRingBuffer* ring_buffer);

uint64_t ReadSampleRecordStreamId(PerfEventRingBuffer* ring_buffer);

pid_t ReadSampleRecordPid(PerfEventRingBuffer* ring_buffer);

std::unique_ptr<StackSamplePerfEvent> ConsumeStackSamplePerfEvent(
    PerfEventRingBuffer* ring_buffer, const perf_event_header& header);

std::unique_ptr<CallchainSamplePerfEvent> ConsumeCallchainSamplePerfEvent(
    PerfEventRingBuffer* ring_buffer, const perf_event_header& header);

std::unique_ptr<RawSamplePerfEvent> ConsumeRawSamplePerfEvent(
    PerfEventRingBuffer* ring_buffer, const perf_event_header& header);

template <typename T>
std::unique_ptr<T> ConsumeTracepointPerfEvent(PerfEventRingBuffer* ring_buffer,
                                              const perf_event_header& header) {
  DCHECK(header.size ==
         sizeof(T::ring_buffer_record) + sizeof(T::tracepoint_data));
  auto event = std::make_unique<T>();
  event->ring_buffer_record.header = header;
  ring_buffer->ReadValueAtOffset(&event->ring_buffer_record, 0);
  ring_buffer->ReadValueAtOffset(&event->tracepoint_data,
                                 sizeof(event->ring_buffer_record));
  ring_buffer->SkipRecord(header);
  return event;
}

}  // namespace LinuxTracing

#endif  // ORBIT_LINUX_TRACING_PERF_EVENT_READERS_H_

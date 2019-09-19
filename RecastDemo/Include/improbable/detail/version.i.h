// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#ifndef WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_VERSION_I_H
#define WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_VERSION_I_H
#include <improbable/worker.h>
#include <cstdint>
#include <string>

namespace worker {
inline std::uint32_t ApiVersion() {
  return detail::internal::Worker_ApiVersion();
}

inline std::string ApiVersionStr() {
  return detail::internal::Worker_ApiVersionStr();
}
}  // namespace worker

#endif  // WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_VERSION_I_H

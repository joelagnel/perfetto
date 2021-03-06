/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// For bazel build.
#include "perfetto/base/build_config.h"
#if PERFETTO_BUILDFLAG(PERFETTO_TP_JSON_IMPORT)

#include "src/trace_processor/importers/json/json_trace_utils.h"

#include <json/value.h>
#include <limits>

namespace perfetto {
namespace trace_processor {
namespace json {
namespace {

int64_t TimeUnitToNs(TimeUnit unit) {
  return static_cast<int64_t>(unit);
}

}  // namespace

base::Optional<int64_t> CoerceToTs(TimeUnit unit, const Json::Value& value) {
  switch (static_cast<size_t>(value.type())) {
    case Json::realValue:
      return static_cast<int64_t>(value.asDouble() * TimeUnitToNs(unit));
    case Json::uintValue:
    case Json::intValue:
      return value.asInt64() * TimeUnitToNs(unit);
    case Json::stringValue: {
      std::string s = value.asString();
      char* end;
      int64_t n = strtoll(s.c_str(), &end, 10);
      if (end != s.data() + s.size())
        return base::nullopt;
      return n * TimeUnitToNs(unit);
    }
    default:
      return base::nullopt;
  }
}

base::Optional<int64_t> CoerceToInt64(const Json::Value& value) {
  switch (static_cast<size_t>(value.type())) {
    case Json::realValue:
    case Json::uintValue:
      return static_cast<int64_t>(value.asUInt64());
    case Json::intValue:
      return value.asInt64();
    case Json::stringValue: {
      std::string s = value.asString();
      char* end;
      int64_t n = strtoll(s.c_str(), &end, 10);
      if (end != s.data() + s.size())
        return base::nullopt;
      return n;
    }
    default:
      return base::nullopt;
  }
}

base::Optional<uint32_t> CoerceToUint32(const Json::Value& value) {
  base::Optional<int64_t> result = CoerceToInt64(value);
  if (!result.has_value())
    return base::nullopt;
  int64_t n = result.value();
  if (n < 0 || n > std::numeric_limits<uint32_t>::max())
    return base::nullopt;
  return static_cast<uint32_t>(n);
}

}  // namespace json
}  // namespace trace_processor
}  // namespace perfetto

#endif  // PERFETTO_BUILDFLAG(PERFETTO_TP_JSON_IMPORT)

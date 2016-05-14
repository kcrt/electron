// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/common/api/atom_api_native_image.h"

#include <string>
#include <vector>

#include "atom/common/node_includes.h"
#include "base/process/process_metrics.h"
#include "native_mate/dictionary.h"

namespace {
  
v8::Local<v8::Value> GetProcessMetrics(v8::Isolate* isolate) {
  mate::Dictionary dict = mate::Dictionary::CreateEmpty(isolate);
  std::unique_ptr<base::ProcessMetrics> metrics(
    base::ProcessMetrics::CreateCurrentProcessMetrics());
    
  dict.Set("workingSetSize", (double)metrics->GetWorkingSetSize());
  dict.Set("peakWorkingSetSize", (double)metrics->GetPeakWorkingSetSize());
  
  size_t private_bytes, shared_bytes;
  if (metrics->GetMemoryBytes(&private_bytes, &shared_bytes)) {
    dict.set("privateBytes", (double)private_bytes);
    dict.set("sharedBytes", (double)shared_bytes);
  }
  
  return dict.GetHandle();
}

v8::Local<v8::Value> GetSystemMemoryInfo(v8::Isolate* isolate, mate::Arguments* args) {
  mate::Dictionary dict = mate::Dictionary::CreateEmpty(isolate);
  base::SystemMemoryInfoKB memInfo;
  
  if (!base::GetSystemMemoryInfo(&memInfo)) {
    args->ThrowError("Unable to retrieve system memory information");
    return v8::Undefined(isolate);
  }
  
  dict.set("total", memInfo.total);
  dict.set("free", memInfo.free);
  
  // NB: These return bogus values on OS X
#if !DEFINED(OS_MACOSX)
  dict.set("swapTotal", memInfo.swap_total);
  dict.set("swapFree", memInfo.swap_free);
#endif

  return dict.GetHandle();
}

void Initialize(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context, void* priv) {
  mate::Dictionary dict(context->GetIsolate(), exports);
  dict.SetMethod("getProcessMetrics", &GetProcessMetrics);
  dict.SetMethod("getSystemMemoryInfo", &GetSystemMemoryInfo);
}

}  // namespace

NODE_MODULE_CONTEXT_AWARE_BUILTIN(atom_common_process_stats, Initialize)

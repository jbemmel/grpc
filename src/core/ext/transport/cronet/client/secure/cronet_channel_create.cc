/*
 *
 * Copyright 2016 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <grpc/impl/codegen/port_platform.h>

#include <stdio.h>
#include <string.h>

#include <grpc/support/alloc.h>
#include <grpc/support/log.h>

#include "src/core/ext/transport/cronet/client/secure/cronet_channel_create.h"
#include "src/core/ext/transport/cronet/transport/cronet_transport.h"
#include "src/core/lib/surface/channel.h"
#include "src/core/lib/transport/transport_impl.h"

// Cronet transport object
typedef struct cronet_transport {
  grpc_transport base;  // must be first element in this structure
  void* engine;
  char* host;
} cronet_transport;

extern grpc_transport_vtable grpc_cronet_vtable;

GRPCAPI grpc_channel* grpc_cronet_secure_channel_create(
    void* engine, const char* target, const grpc_channel_args* args,
    void* reserved) {
  gpr_log(GPR_DEBUG,
          "grpc_create_cronet_transport: stream_engine = %p, target=%s", engine,
          target);

  // Disable client authority filter when using Cronet
  grpc_arg disable_client_authority_filter_arg;
  disable_client_authority_filter_arg.key =
      const_cast<char*>(GRPC_ARG_DISABLE_CLIENT_AUTHORITY_FILTER);
  disable_client_authority_filter_arg.type = GRPC_ARG_INTEGER;
  disable_client_authority_filter_arg.value.integer = 1;
  grpc_channel_args* new_args = grpc_channel_args_copy_and_add(
      args, &disable_client_authority_filter_arg, 1);

  grpc_transport* ct =
      grpc_create_cronet_transport(engine, target, new_args, reserved);

  grpc_core::ExecCtx exec_ctx;
  grpc_channel* channel = grpc_channel_create(
      target, new_args, GRPC_CLIENT_DIRECT_CHANNEL, ct, nullptr, 0, nullptr);
  grpc_channel_args_destroy(new_args);
  return channel;
}

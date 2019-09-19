// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#ifndef WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_SNAPSHOT_I_H
#define WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_SNAPSHOT_I_H
#include <improbable/collections.h>
#include <improbable/detail/client_handle.i.h>
#include <improbable/detail/extract.i.h>
#include <improbable/detail/result.i.h>
#include <improbable/worker.h>
#include <cstdint>
#include <string>

namespace worker {
namespace detail {

inline internal::Worker_SnapshotInputStream*
MakeSnapshotInputStream(const detail::ComponentInfo& component_info, const std::string& path) {
  internal::Worker_SnapshotParameters parameters = {};
  parameters.component_vtable_count = static_cast<std::uint32_t>(component_info.Vtables.size());
  parameters.component_vtables =
      reinterpret_cast<const internal::Worker_ComponentVtable*>(component_info.Vtables.data());
  internal::Worker_SnapshotInputStream* input_stream =
      internal::Worker_SnapshotInputStream_Create(path.c_str(), &parameters);
  return input_stream;
}

inline internal::Worker_SnapshotOutputStream*
MakeSnapshotOutputStream(const detail::ComponentInfo& component_info, const std::string& path) {
  internal::Worker_SnapshotParameters parameters = {};
  parameters.component_vtable_count = static_cast<std::uint32_t>(component_info.Vtables.size());
  parameters.component_vtables =
      reinterpret_cast<const internal::Worker_ComponentVtable*>(component_info.Vtables.data());
  internal::Worker_SnapshotOutputStream* output_stream =
      internal::Worker_SnapshotOutputStream_Create(path.c_str(), &parameters);
  return output_stream;
}

inline Result<StreamErrorCode, None> ConvertStreamState(std::uint8_t state_code) {
  assert(state_code != internal::WORKER_STREAM_STATE_GOOD);
  switch (state_code) {
  case internal::WORKER_STREAM_STATE_EOF:
    return Result<StreamErrorCode, None>::Success(StreamErrorCode::kEof);
  case internal::WORKER_STREAM_STATE_INVALID_DATA:
    return Result<StreamErrorCode, None>::Success(StreamErrorCode::kInvalidData);
  case internal::WORKER_STREAM_STATE_BAD:
    return Result<StreamErrorCode, None>::Success(StreamErrorCode::kBadState);
  }
  // Should be unreachable.
  return Result<StreamErrorCode, None>::Failure(
      "Illegal stream state code " + std::to_string(state_code) + ": please report this bug.");
}

}  // namespace detail

inline Result<SnapshotInputStream, StreamErrorCode>
SnapshotInputStream::Create(const ComponentRegistry& registry, const std::string& path) {
  auto worker_protocol_input_stream =
      detail::MakeSnapshotInputStream(registry.GetInternalComponentInfo(), path);
  auto snapshot_state =
      detail::internal::Worker_SnapshotInputStream_GetState(worker_protocol_input_stream);
  if (snapshot_state.stream_state != detail::internal::WORKER_STREAM_STATE_GOOD) {
    auto failure_state = detail::ConvertStreamState(snapshot_state.stream_state);
    if (!failure_state) {
      return Result<SnapshotInputStream, StreamErrorCode>::Failure(failure_state.GetErrorMessage(),
                                                                   StreamErrorCode::kBadState);
    }
    return Result<SnapshotInputStream, StreamErrorCode>::Failure(snapshot_state.error_message,
                                                                 *failure_state);
  }
  return Result<SnapshotInputStream, StreamErrorCode>::Success(
      SnapshotInputStream(&registry.GetInternalComponentInfo(), worker_protocol_input_stream));
}

inline SnapshotInputStream::SnapshotInputStream(
    const detail::ComponentInfo* component_info,
    detail::internal::Worker_SnapshotInputStream* worker_protocol_input_stream)
: component_info{component_info}
, input_stream{worker_protocol_input_stream,
               &detail::internal::Worker_SnapshotInputStream_Destroy} {}

inline bool SnapshotInputStream::HasNext() {
  return 0 != detail::internal::Worker_SnapshotInputStream_HasNext(input_stream.get());
}

inline Result<std::pair<EntityId, Entity>, StreamErrorCode> SnapshotInputStream::ReadEntity() {
  // Worker_SnapshotInputStream_ReadEntity manages the memory for worker_protocol_entity
  // internally.
  const auto* worker_protocol_entity =
      detail::internal::Worker_SnapshotInputStream_ReadEntity(input_stream.get());
  auto snapshot_state = detail::internal::Worker_SnapshotInputStream_GetState(input_stream.get());
  if (snapshot_state.stream_state != detail::internal::WORKER_STREAM_STATE_GOOD) {
    auto failure_state = detail::ConvertStreamState(snapshot_state.stream_state);
    if (!failure_state) {
      return Result<std::pair<EntityId, Entity>, StreamErrorCode>::Failure(
          failure_state.GetErrorMessage(), StreamErrorCode::kBadState);
    }
    return Result<std::pair<EntityId, Entity>, StreamErrorCode>::Failure(
        snapshot_state.error_message, *failure_state);
  }
  Entity entity;
  for (std::uint32_t i = 0; i < worker_protocol_entity->component_count; ++i) {
    const auto& component = worker_protocol_entity->components[i];
    auto it = component_info->MoveSnapshotIntoEntity.find(component.component_id);
    if (it != component_info->MoveSnapshotIntoEntity.end()) {
      // Moves the snapshot data out of the C-owned object. Fine to steal the data here.
      it->second(const_cast<detail::ClientHandleBase*>(
                     static_cast<const detail::ClientHandleBase*>(component.user_handle)),
                 entity);
    }
  }
  return Result<std::pair<EntityId, Entity>, StreamErrorCode>::Success(
      std::make_pair(worker_protocol_entity->entity_id, entity));
}

inline Result<SnapshotOutputStream, StreamErrorCode>
SnapshotOutputStream::Create(const ComponentRegistry& registry, const std::string& path) {
  detail::internal::Worker_SnapshotOutputStream* worker_protocol_output_stream =
      detail::MakeSnapshotOutputStream(registry.GetInternalComponentInfo(), path);
  auto snapshot_state =
      detail::internal::Worker_SnapshotOutputStream_GetState(worker_protocol_output_stream);
  if (snapshot_state.stream_state != detail::internal::WORKER_STREAM_STATE_GOOD) {
    auto failure_state = detail::ConvertStreamState(snapshot_state.stream_state);
    if (!failure_state) {
      return Result<SnapshotOutputStream, StreamErrorCode>::Failure(failure_state.GetErrorMessage(),
                                                                    StreamErrorCode::kBadState);
    }
    return Result<SnapshotOutputStream, StreamErrorCode>::Failure(snapshot_state.error_message,
                                                                  *failure_state);
  }
  return Result<SnapshotOutputStream, StreamErrorCode>::Success(
      SnapshotOutputStream{&registry.GetInternalComponentInfo(), worker_protocol_output_stream});
}

inline SnapshotOutputStream::SnapshotOutputStream(
    const detail::ComponentInfo* component_info,
    detail::internal::Worker_SnapshotOutputStream* worker_protocol_output_stream)
: component_info{component_info}
, output_stream{worker_protocol_output_stream,
                &detail::internal::Worker_SnapshotOutputStream_Destroy} {}

inline Result<None, StreamErrorCode> SnapshotOutputStream::WriteEntity(EntityId entity_id,
                                                                       const Entity& entity) {
  detail::internal::Worker_Entity snapshot_entity;
  snapshot_entity.entity_id = entity_id;
  std::vector<std::unique_ptr<detail::ClientHandleBase>> handle_storage;
  std::unique_ptr<detail::internal::Worker_ComponentData[]> component_snapshots;
  ExtractEntityComponents(*component_info, entity, snapshot_entity.component_count, handle_storage,
                          component_snapshots);
  snapshot_entity.components = component_snapshots.get();
  detail::internal::Worker_SnapshotOutputStream_WriteEntity(output_stream.get(), &snapshot_entity);
  auto snapshot_state = detail::internal::Worker_SnapshotOutputStream_GetState(output_stream.get());
  if (snapshot_state.stream_state != detail::internal::WORKER_STREAM_STATE_GOOD) {
    auto failure_state = detail::ConvertStreamState(snapshot_state.stream_state);
    if (!failure_state) {
      return Result<None, StreamErrorCode>::Failure(failure_state.GetErrorMessage(),
                                                    StreamErrorCode::kBadState);
    }
    return Result<None, StreamErrorCode>::Failure(snapshot_state.error_message, *failure_state);
  }
  return Result<None, StreamErrorCode>::Success({});
}

}  // namespace worker

#endif  // WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_SNAPSHOT_I_H

// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#ifndef WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_EXTRACT_I_H
#define WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_EXTRACT_I_H
#include <improbable/detail/client_handle.i.h>
#include <improbable/detail/components.i.h>
#include <cstdint>
#include <memory>
#include <vector>

namespace worker {
namespace detail {

/** Extracts the components of an Entity into the C equivalent structures. */
inline void ExtractEntityComponents(
    const detail::ComponentInfo& component_info, const Entity& entity,
    std::uint32_t& component_count,
    std::vector<std::unique_ptr<detail::ClientHandleBase>>& handle_storage,
    std::unique_ptr<internal::Worker_ComponentData[]>& component_snapshot_storage) {
  // Create C-compatible continuous storage for the updates.
  const auto& component_ids = entity.GetComponentIds();
  component_count = static_cast<std::uint32_t>(component_ids.size());
  component_snapshot_storage.reset(new internal::Worker_ComponentData[component_count]);
  internal::Worker_ComponentData* entity_components_array = component_snapshot_storage.get();

  // Extract each component into a Worker_ComponentUpdate.
  std::size_t component_index = 0;
  for (const auto& component_id : component_ids) {
    auto it = component_info.ExtractSnapshot.find(component_id);
    if (it != component_info.ExtractSnapshot.end()) {
      handle_storage.emplace_back(it->second(entity));
      entity_components_array[component_index].component_id = component_id;
      entity_components_array[component_index].schema_type = nullptr;
      entity_components_array[component_index].user_handle = handle_storage.back().get();
    } else {
      // This causes the C API to report an error for the unknown component.
      entity_components_array[component_index].component_id = component_id;
      entity_components_array[component_index].schema_type = nullptr;
      entity_components_array[component_index].user_handle = nullptr;
    }
    ++component_index;
  }
}

}  // namespace detail
}  // namespace worker

#endif  // WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_EXTRACT_I_H

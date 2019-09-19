// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#ifndef WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_ENTITY_I_H
#define WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_ENTITY_I_H
#include <improbable/collections.h>
#include <improbable/detail/components.i.h>
#include <improbable/worker.h>

namespace worker {

inline Entity::Entity(const Entity& entity) {
  for (const auto& pair : entity.components) {
    components.emplace(pair.first, pair.second->Copy());
  }
}

inline Entity& Entity::operator=(const Entity& entity) {
  if (&entity != this) {
    components.clear();
    for (const auto& pair : entity.components) {
      components.emplace(pair.first, pair.second->Copy());
    }
  }
  return *this;
}

template <typename T>
Option<typename T::Data&> Entity::Get() {
  auto it = components.find(T::ComponentId);
  if (it == components.end()) {
    return {};
  }
  return static_cast<detail::ComponentStorage<T>&>(*it->second).Get();
}

template <typename T>
Option<const typename T::Data&> Entity::Get() const {
  auto it = components.find(T::ComponentId);
  if (it == components.end()) {
    return {};
  }
  return static_cast<const detail::ComponentStorage<T>&>(*it->second).Get();
}

template <typename T>
void Entity::Add(const typename T::Data& data) {
  auto it = components.find(T::ComponentId);
  if (it == components.end()) {
    components.emplace(
        T::ComponentId,
        std::unique_ptr<detail::ComponentStorageBase>{new detail::ComponentStorage<T>{data}});
  }
}

template <typename T>
void Entity::Add(typename T::Data&& data) {
  auto it = components.find(T::ComponentId);
  if (it == components.end()) {
    components.emplace(T::ComponentId,
                       std::unique_ptr<detail::ComponentStorageBase>{
                           new detail::ComponentStorage<T>{std::move(data)}});
  }
}

template <typename T>
void Entity::Update(const typename T::Update& update) {
  auto it = components.find(T::ComponentId);
  if (it != components.end()) {
    update.ApplyTo(static_cast<detail::ComponentStorage<T>&>(*it->second).Get());
  }
}

template <typename T>
void Entity::Remove() {
  components.erase(T::ComponentId);
}

inline List<ComponentId> Entity::GetComponentIds() const {
  List<ComponentId> ids;
  for (const auto& kv : components) {
    ids.emplace_back(kv.first);
  }
  return ids;
}

inline bool Entity::operator==(const Entity& value) const {
  if (components.size() != value.components.size()) {
    return false;
  }
  for (const auto& pair : value.components) {
    auto it = components.find(pair.first);
    if (it == components.end() || !it->second->Equals(*pair.second)) {
      return false;
    }
  }
  return true;
}

inline bool Entity::operator!=(const Entity& value) const {
  return !(*this == value);
}

namespace detail {

inline void Serialize(const worker::Entity& entity, internal::Schema_Object* object) {
  for (const auto& item : entity.components) {
    auto component_object = detail::internal::Schema_AddObject(object, item.first);
    item.second->Serialize(component_object);
  }
}

inline worker::Entity Deserialize(const detail::ComponentInfo& component_info,
                                  detail::internal::Schema_Object* object) {
  Entity entity;
  std::vector<ComponentId> field_ids(Schema_GetUniqueFieldIdCount(object));
  Schema_GetUniqueFieldIds(object, field_ids.data());
  for (auto field_id : field_ids) {
    component_info.Deserialize.find(field_id)->second(
        detail::internal::Schema_GetObject(object, field_id), entity);
  }
  return entity;
}

}  // namespace detail

}  // namespace worker

namespace std {

template <>
struct hash<::worker::Entity> {
  std::size_t operator()(const ::worker::Entity& value) const {
    std::size_t result = 0;
    // The hash, to match the equality operator, does not encode order.
    for (const auto& pair : value.components) {
      result += 1327 * (std::hash<worker::ComponentId>{}(pair.first) + 977 * pair.second->Hash());
    }
    return result;
  }
};

}  // namespace std

#endif  // WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_ENTITY_I_H

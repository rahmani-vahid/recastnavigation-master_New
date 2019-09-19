// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#ifndef WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_COMPONENTS_I_H
#define WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_COMPONENTS_I_H
#include <improbable/detail/client_handle.i.h>
#include <improbable/detail/result.i.h>
#include <improbable/worker.h>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace worker {
namespace detail {

struct ComponentInfo {
  ComponentInfo() = default;
  ComponentInfo(const ComponentInfo&) = delete;
  ComponentInfo(ComponentInfo&&) = delete;
  ComponentInfo& operator=(const ComponentInfo&) = delete;
  ComponentInfo& operator=(ComponentInfo&&) = delete;

  std::vector<internal::Worker_ComponentVtable> Vtables;
  std::unordered_map<ComponentId, std::function<std::unique_ptr<ClientHandleBase>(const Entity&)>>
      ExtractSnapshot;
  std::unordered_map<ComponentId, std::function<void(ClientHandleBase*, Entity&)>>
      MoveSnapshotIntoEntity;
  std::unordered_map<ComponentId, std::function<void(internal::Schema_Object*, Entity&)>>
      Deserialize;
};

struct CreateComponentInfoHandler {
  ComponentInfo& component_info;
  template <typename T>
  void Accept() const {
    using DataHandle = ClientHandle<typename T::Data>;
    auto vtable = T::Vtable();
    // We store in the vtables of this registry a backpointer to the ComponentInfo. This
    // is required to deserialize entities that are in data fields.
    vtable.user_data = &component_info;
    component_info.Vtables.emplace_back(vtable);
    component_info.ExtractSnapshot[T::ComponentId] = [](const Entity& entity) {
      return std::unique_ptr<ClientHandleBase>{
          new DataHandle{DataHandle::copyable(entity.Get<T>().data())}};
    };
    component_info.MoveSnapshotIntoEntity[T::ComponentId] = [](ClientHandleBase* snapshot,
                                                               Entity& entity) {
      entity.Add<T>(std::move(DataHandle::get_mutable(snapshot)));
    };
    component_info.Deserialize[T::ComponentId] = [this](internal::Schema_Object* object,
                                                        Entity& entity) {
      entity.Add<T>(T::DeserializeData(this->component_info, object));
    };
  }
};

// Storage for arbitrary component data.
class ComponentStorageBase {
public:
  virtual ~ComponentStorageBase(){};
  virtual std::unique_ptr<ComponentStorageBase> Copy() const = 0;

  virtual std::size_t Hash() const = 0;
  virtual bool Equals(const ComponentStorageBase& other) const = 0;
  virtual void Serialize(detail::internal::Schema_Object* object) const = 0;
};

template <typename T>
class ComponentStorage : public ComponentStorageBase {
public:
  explicit ComponentStorage(const typename T::Data& data) : data{data} {}
  explicit ComponentStorage(typename T::Data&& data) : data{std::move(data)} {}
  ~ComponentStorage() override {}

  const typename T::Data& Get() const {
    return data;
  }

  typename T::Data& Get() {
    return data;
  }

  std::unique_ptr<ComponentStorageBase> Copy() const override {
    return std::unique_ptr<ComponentStorageBase>{new ComponentStorage{data}};
  }

  std::size_t Hash() const override {
    return std::hash<typename T::Data>{}(data);
  }

  // This is only used to compare ComponentStorage elements, but is required here
  // to implement Entity::Equals, as Entity keeps a map of ComponentStorageBase.
  bool Equals(const ComponentStorageBase& other) const override {
    auto otherStorage = static_cast<const ComponentStorage&>(other);
    return data == otherStorage.data;
  }

  virtual void Serialize(detail::internal::Schema_Object* object) const override {
    T::SerializeData(data, object);
  }

private:
  typename T::Data data;
};

inline Result<None> CheckComponentRegistered(const ComponentInfo& component_info,
                                             const ComponentId& component_id) {
  if (component_info.ExtractSnapshot.find(component_id) == component_info.ExtractSnapshot.end()) {
    return Result<None>::Failure("Component with ID " + std::to_string(component_id) +
                                 " was not provided to the worker::Components<> template class.");
  }
  return Result<None>::Success({});
}

}  // namespace detail

template <typename... Args>
const detail::ComponentInfo& Components<Args...>::GetInternalComponentInfo() const {
  static auto component_info = [this] {
    auto result = std::unique_ptr<detail::ComponentInfo>(new detail::ComponentInfo);
    ForEachComponent(*this, detail::CreateComponentInfoHandler{*result});
    return result;
  }();
  return *component_info;
}

}  // namespace worker

#endif  // WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_COMPONENTS_I_H

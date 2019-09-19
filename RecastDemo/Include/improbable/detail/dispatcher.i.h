// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#ifndef WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_DISPATCHER_I_H
#define WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_DISPATCHER_I_H
#include <improbable/detail/client_handle.i.h>
#include <improbable/detail/result.i.h>
#include <improbable/worker.h>

namespace worker {
namespace detail {
inline void HandleDisconnect(const detail::DispatcherImpl& impl,
                             const internal::Worker_DisconnectOp& op) {
  DisconnectOp wrapper{static_cast<ConnectionStatusCode>(op.connection_status_code), op.reason};
  impl.disconnect_callbacks.InvokeAll(wrapper);
}

inline void HandleFlagUpdate(const detail::DispatcherImpl& impl,
                             const internal::Worker_FlagUpdateOp& op) {
  FlagUpdateOp wrapper{op.name, {}};
  if (op.value) {
    wrapper.Value.emplace(op.value);
    impl.flag_update_callbacks.InvokeAll(wrapper);
  } else {
    impl.flag_update_callbacks.ReverseInvokeAll(wrapper);
  }
}

inline void HandleLogMessage(const detail::DispatcherImpl& impl,
                             const internal::Worker_LogMessageOp& op) {
  LogMessageOp wrapper{static_cast<LogLevel>(op.level), op.message};
  impl.log_message_callbacks.InvokeAll(wrapper);
}

inline void HandleMetrics(const detail::DispatcherImpl& impl,
                          const internal::Worker_MetricsOp& op) {
  MetricsOp wrapper;
  for (std::size_t i = 0; i < op.metrics.gauge_metric_count; ++i) {
    wrapper.Metrics.GaugeMetrics[op.metrics.gauge_metrics[i].key] =
        op.metrics.gauge_metrics[i].value;
  }
  for (std::size_t i = 0; i < op.metrics.histogram_metric_count; ++i) {
    wrapper.Metrics.HistogramMetrics[op.metrics.histogram_metrics[i].key];
    const auto* wp_buckets = op.metrics.histogram_metrics[i].buckets;
    List<worker::HistogramMetric::Bucket> buckets;
    for (std::size_t j = 0; j < op.metrics.histogram_metrics[i].bucket_count; ++j) {
      buckets.emplace_back(
          HistogramMetric::Bucket{wp_buckets[j].upper_bound, wp_buckets[j].samples});
    }
    wrapper.Metrics.HistogramMetrics[op.metrics.histogram_metrics[i].key] =
        HistogramMetric{std::move(buckets), op.metrics.histogram_metrics[i].sum};
  }
  impl.metrics_callbacks.InvokeAll(wrapper);
}

inline void HandleCriticalSection(const detail::DispatcherImpl& impl,
                                  const internal::Worker_CriticalSectionOp& op) {
  CriticalSectionOp wrapper{op.in_critical_section != 0};
  if (op.in_critical_section) {
    impl.critical_section_callbacks.InvokeAll(wrapper);
  } else {
    impl.critical_section_callbacks.ReverseInvokeAll(wrapper);
  }
}

inline void HandleAddEntity(const detail::DispatcherImpl& impl,
                            const internal::Worker_AddEntityOp& op) {
  AddEntityOp wrapper{op.entity_id};
  impl.add_entity_callbacks.InvokeAll(wrapper);
}

inline void HandleRemoveEntity(const detail::DispatcherImpl& impl,
                               const internal::Worker_RemoveEntityOp& op) {
  RemoveEntityOp wrapper{op.entity_id};
  impl.remove_entity_callbacks.ReverseInvokeAll(wrapper);
}

inline void HandleReserveEntityIdsResponse(const detail::DispatcherImpl& impl,
                                           const internal::Worker_ReserveEntityIdsResponseOp& op) {
  ReserveEntityIdsResponseOp wrapper{RequestId<ReserveEntityIdsRequest>{op.request_id},
                                     static_cast<worker::StatusCode>(op.status_code), op.message,
                                     op.status_code == internal::WORKER_STATUS_CODE_SUCCESS
                                         ? op.first_entity_id
                                         : Option<EntityId>{},
                                     op.number_of_entity_ids};
  impl.reserve_entity_ids_response_callbacks.InvokeAll(wrapper);
}

inline void HandleCreateEntityResponse(const detail::DispatcherImpl& impl,
                                       const internal::Worker_CreateEntityResponseOp& op) {
  CreateEntityResponseOp wrapper{
      RequestId<CreateEntityRequest>{op.request_id},
      static_cast<worker::StatusCode>(op.status_code), op.message,
      op.status_code == internal::WORKER_STATUS_CODE_SUCCESS ? op.entity_id : Option<EntityId>{}};
  impl.create_entity_response_callbacks.InvokeAll(wrapper);
}

inline void HandleDeleteEntityResponse(const detail::DispatcherImpl& impl,
                                       const internal::Worker_DeleteEntityResponseOp& op) {
  DeleteEntityResponseOp wrapper{RequestId<DeleteEntityRequest>{op.request_id}, op.entity_id,
                                 static_cast<worker::StatusCode>(op.status_code), op.message};
  impl.delete_entity_response_callbacks.InvokeAll(wrapper);
}

inline void HandleEntityQueryResponse(const detail::DispatcherImpl& impl,
                                      const internal::Worker_EntityQueryResponseOp& op) {
  EntityQueryResponseOp wrapper{RequestId<EntityQueryRequest>{op.request_id},
                                static_cast<worker::StatusCode>(op.status_code),
                                op.message,
                                op.result_count,
                                {}};
  for (std::uint32_t i = 0; op.results && i < op.result_count; ++i) {
    auto& entity = wrapper.Result[op.results[i].entity_id];
    for (std::uint32_t j = 0; j < op.results[i].component_count; ++j) {
      const auto& component = op.results[i].components[j];
      auto it = impl.component_info.MoveSnapshotIntoEntity.find(component.component_id);
      if (it != impl.component_info.MoveSnapshotIntoEntity.end()) {
        // Moves the snapshot data out of the C-owned object. It's OK to steal the data here since
        // we know this is the only possible callback for the C dispatcher.
        it->second(const_cast<detail::ClientHandleBase*>(
                       static_cast<const detail::ClientHandleBase*>(component.user_handle)),
                   entity);
      }
    }
  }
  impl.entity_query_response_callbacks.InvokeAll(wrapper);
}

inline void HandleAddComponent(const detail::DispatcherImpl& impl,
                               const internal::Worker_AddComponentOp& op) {
  detail::DispatcherImpl::ComponentWrapperOp wrapper{
      op.entity_id, static_cast<const detail::ClientHandleBase*>(op.data.user_handle)};
  impl.add_component_callbacks.InvokeAll(op.data.component_id, wrapper);
}

inline void HandleRemoveComponent(const detail::DispatcherImpl& impl,
                                  const internal::Worker_RemoveComponentOp& op) {
  RemoveComponentOp wrapper{op.entity_id};
  impl.remove_component_callbacks.ReverseInvokeAll(op.component_id, wrapper);
}

inline void HandleAuthorityChange(const detail::DispatcherImpl& impl,
                                  const internal::Worker_AuthorityChangeOp& op) {
  AuthorityChangeOp wrapper{op.entity_id, static_cast<worker::Authority>(op.authority)};
  if (wrapper.Authority == Authority::kAuthoritative) {
    impl.authority_change_callbacks.InvokeAll(op.component_id, wrapper);
  } else {
    impl.authority_change_callbacks.ReverseInvokeAll(op.component_id, wrapper);
  }
}

inline void HandleComponentUpdate(const detail::DispatcherImpl& impl,
                                  const internal::Worker_ComponentUpdateOp& op) {
  detail::DispatcherImpl::ComponentWrapperOp wrapper{
      op.entity_id, static_cast<const detail::ClientHandleBase*>(op.update.user_handle)};
  impl.component_update_callbacks.InvokeAll(op.update.component_id, wrapper);
}

inline void HandleCommandRequest(const detail::DispatcherImpl& impl,
                                 const internal::Worker_CommandRequestOp& op) {
  detail::DispatcherImpl::CommandRequestWrapperOp wrapper{
      op.request_id,
      op.entity_id,
      op.timeout_millis,
      op.caller_worker_id,
      op.caller_attribute_set.attribute_count,
      op.caller_attribute_set.attributes,
      static_cast<const detail::ClientHandleBase*>(op.request.user_handle)};
  impl.command_request_callbacks.InvokeAll(op.request.component_id, wrapper);
}

inline void HandleCommandResponse(const detail::DispatcherImpl& impl,
                                  const internal::Worker_CommandResponseOp& op) {
  detail::DispatcherImpl::CommandResponseWrapperOp wrapper{
      op.request_id,
      op.entity_id,
      op.status_code,
      op.message,
      static_cast<const detail::ClientHandleBase*>(op.response.user_handle),
      op.response.command_index};
  impl.command_response_callbacks.InvokeAll(op.response.component_id, wrapper);
}

}  // namespace detail

// Implementation of Dispatcher.
inline Dispatcher::Dispatcher(const ComponentRegistry& registry)
: impl{registry.GetInternalComponentInfo()} {}

inline Dispatcher::CallbackKey Dispatcher::OnDisconnect(const Callback<DisconnectOp>& callback) {
  impl.disconnect_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey Dispatcher::OnFlagUpdate(const Callback<FlagUpdateOp>& callback) {
  impl.flag_update_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey Dispatcher::OnLogMessage(const Callback<LogMessageOp>& callback) {
  impl.log_message_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey Dispatcher::OnMetrics(const Callback<MetricsOp>& callback) {
  impl.metrics_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnCriticalSection(const Callback<CriticalSectionOp>& callback) {
  impl.critical_section_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey Dispatcher::OnAddEntity(const Callback<AddEntityOp>& callback) {
  impl.add_entity_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnRemoveEntity(const Callback<RemoveEntityOp>& callback) {
  impl.remove_entity_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnReserveEntityIdsResponse(const Callback<ReserveEntityIdsResponseOp>& callback) {
  impl.reserve_entity_ids_response_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnCreateEntityResponse(const Callback<CreateEntityResponseOp>& callback) {
  impl.create_entity_response_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnDeleteEntityResponse(const Callback<DeleteEntityResponseOp>& callback) {
  impl.delete_entity_response_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnEntityQueryResponse(const Callback<EntityQueryResponseOp>& callback) {
  impl.entity_query_response_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

template <typename T>
Result<Dispatcher::CallbackKey>
Dispatcher::OnAddComponent(const Callback<AddComponentOp<T>>& callback) {
  auto result = detail::CheckComponentRegistered(impl.component_info, T::ComponentId);
  if (!result) {
    return detail::ForwardFailure<Dispatcher::CallbackKey>(
        "Error registering OnAddComponent callback", result);
  }
  auto wrapper_callback = [callback](const detail::DispatcherImpl::ComponentWrapperOp& op) {
    callback(
        AddComponentOp<T>{op.EntityId, detail::ClientHandle<typename T::Data>::get(op.Handle)});
  };
  impl.add_component_callbacks.Add(T::ComponentId, impl.current_callback_key, wrapper_callback);
  return Result<Dispatcher::CallbackKey>::Success(impl.current_callback_key++);
}

template <typename T>
Result<Dispatcher::CallbackKey>
Dispatcher::OnRemoveComponent(const Callback<RemoveComponentOp>& callback) {
  auto result = detail::CheckComponentRegistered(impl.component_info, T::ComponentId);
  if (!result) {
    return detail::ForwardFailure<Dispatcher::CallbackKey>(
        "Error registering OnRemoveComponent callback", result);
  }
  impl.remove_component_callbacks.Add(T::ComponentId, impl.current_callback_key, callback);
  return Result<Dispatcher::CallbackKey>::Success(impl.current_callback_key++);
}

template <typename T>
Result<Dispatcher::CallbackKey>
Dispatcher::OnAuthorityChange(const Callback<AuthorityChangeOp>& callback) {
  auto result = detail::CheckComponentRegistered(impl.component_info, T::ComponentId);
  if (!result) {
    return detail::ForwardFailure<Dispatcher::CallbackKey>(
        "Error registering OnAuthorityChange callback", result);
  }
  impl.authority_change_callbacks.Add(T::ComponentId, impl.current_callback_key, callback);
  return Result<Dispatcher::CallbackKey>::Success(impl.current_callback_key++);
}

template <typename T>
Result<Dispatcher::CallbackKey>
Dispatcher::OnComponentUpdate(const Callback<ComponentUpdateOp<T>>& callback) {
  auto result = detail::CheckComponentRegistered(impl.component_info, T::ComponentId);
  if (!result) {
    return detail::ForwardFailure<Dispatcher::CallbackKey>(
        "Error registering OnComponentUpdate callback", result);
  }
  auto wrapper_callback = [callback](const detail::DispatcherImpl::ComponentWrapperOp& op) {
    callback(ComponentUpdateOp<T>{op.EntityId,
                                  detail::ClientHandle<typename T::Update>::get(op.Handle)});
  };
  impl.component_update_callbacks.Add(T::ComponentId, impl.current_callback_key, wrapper_callback);
  return Result<Dispatcher::CallbackKey>::Success(impl.current_callback_key++);
}

template <typename T>
Result<Dispatcher::CallbackKey>
Dispatcher::OnCommandRequest(const Callback<CommandRequestOp<T>>& callback) {
  auto result =
      detail::CheckComponentRegistered(impl.component_info, T::ComponentMetaclass::ComponentId);
  if (!result) {
    return detail::ForwardFailure<Dispatcher::CallbackKey>(
        "Error registering OnCommandRequest callback", result);
  }
  auto wrapper_callback = [callback](const detail::DispatcherImpl::CommandRequestWrapperOp& op) {
    const auto& generic_request =
        detail::ClientHandle<typename T::ComponentMetaclass::GenericCommandObject>::get(op.Request);
    if (generic_request.CommandIndex != T::CommandIndex) {
      return;
    }
    CommandRequestOp<T> wrapper{
        RequestId<IncomingCommandRequest<T>>{op.RequestId},
        op.EntityId,
        op.TimeoutMillis,
        op.CallerWorkerId,
        /* CallerAttributeSet */ {},
        *generic_request.CommandObject.template data<typename T::Request>()};
    for (std::uint32_t i = 0; i < op.CallerAttributeCount; ++i) {
      wrapper.CallerAttributeSet.emplace_back(op.CallerAttribute[i]);
    }
    callback(wrapper);
  };
  impl.command_request_callbacks.Add(T::ComponentMetaclass::ComponentId, impl.current_callback_key,
                                     wrapper_callback);
  return Result<Dispatcher::CallbackKey>::Success(impl.current_callback_key++);
}

template <typename T>
Result<Dispatcher::CallbackKey>
Dispatcher::OnCommandResponse(const Callback<CommandResponseOp<T>>& callback) {
  auto result =
      detail::CheckComponentRegistered(impl.component_info, T::ComponentMetaclass::ComponentId);
  if (!result) {
    return detail::ForwardFailure<Dispatcher::CallbackKey>(
        "Error registering OnCommandResponse callback", result);
  }
  auto wrapper_callback = [callback](const detail::DispatcherImpl::CommandResponseWrapperOp& op) {
    if (op.CommandIndex != T::CommandIndex) {
      return;
    }
    CommandResponseOp<T> wrapper{RequestId<OutgoingCommandRequest<T>>{op.RequestId},
                                 op.EntityId,
                                 static_cast<StatusCode>(op.StatusCode),
                                 op.Message,
                                 {}};
    if (op.Response) {
      const auto& generic_response =
          detail::ClientHandle<typename T::ComponentMetaclass::GenericCommandObject>::get(
              op.Response);
      wrapper.Response.emplace(
          *generic_response.CommandObject.template data<typename T::Response>());
    }
    callback(wrapper);
  };
  impl.command_response_callbacks.Add(T::ComponentMetaclass::ComponentId, impl.current_callback_key,
                                      wrapper_callback);
  return Result<Dispatcher::CallbackKey>::Success(impl.current_callback_key++);
}

inline void Dispatcher::Remove(CallbackKey key) {
  if (!impl.disconnect_callbacks.Remove(key) && !impl.flag_update_callbacks.Remove(key) &&
      !impl.log_message_callbacks.Remove(key) && !impl.metrics_callbacks.Remove(key) &&
      !impl.critical_section_callbacks.Remove(key) && !impl.add_entity_callbacks.Remove(key) &&
      !impl.remove_entity_callbacks.Remove(key) &&
      !impl.reserve_entity_ids_response_callbacks.Remove(key) &&
      !impl.create_entity_response_callbacks.Remove(key) &&
      !impl.delete_entity_response_callbacks.Remove(key) &&
      !impl.entity_query_response_callbacks.Remove(key) &&
      !impl.add_component_callbacks.Remove(key) && !impl.remove_component_callbacks.Remove(key) &&
      !impl.authority_change_callbacks.Remove(key) &&
      !impl.component_update_callbacks.Remove(key) && !impl.command_request_callbacks.Remove(key) &&
      !impl.command_response_callbacks.Remove(key)) {
    std::terminate();
  }
}

inline void Dispatcher::Process(const OpList& op_list) const {
  for (std::uint32_t op_index = 0; op_index < op_list.op_list->op_count; ++op_index) {
    auto& op = op_list.op_list->ops[op_index];
    switch (op.op_type) {
    case detail::internal::WORKER_OP_TYPE_DISCONNECT:
      detail::HandleDisconnect(impl, op.op.disconnect);
      break;
    case detail::internal::WORKER_OP_TYPE_FLAG_UPDATE:
      detail::HandleFlagUpdate(impl, op.op.flag_update);
      break;
    case detail::internal::WORKER_OP_TYPE_LOG_MESSAGE:
      detail::HandleLogMessage(impl, op.op.log_message);
      break;
    case detail::internal::WORKER_OP_TYPE_METRICS:
      detail::HandleMetrics(impl, op.op.metrics);
      break;
    case detail::internal::WORKER_OP_TYPE_CRITICAL_SECTION:
      detail::HandleCriticalSection(impl, op.op.critical_section);
      break;
    case detail::internal::WORKER_OP_TYPE_ADD_ENTITY:
      detail::HandleAddEntity(impl, op.op.add_entity);
      break;
    case detail::internal::WORKER_OP_TYPE_REMOVE_ENTITY:
      detail::HandleRemoveEntity(impl, op.op.remove_entity);
      break;
    case detail::internal::WORKER_OP_TYPE_RESERVE_ENTITY_IDS_RESPONSE:
      detail::HandleReserveEntityIdsResponse(impl, op.op.reserve_entity_ids_response);
      break;
    case detail::internal::WORKER_OP_TYPE_CREATE_ENTITY_RESPONSE:
      detail::HandleCreateEntityResponse(impl, op.op.create_entity_response);
      break;
    case detail::internal::WORKER_OP_TYPE_DELETE_ENTITY_RESPONSE:
      detail::HandleDeleteEntityResponse(impl, op.op.delete_entity_response);
      break;
    case detail::internal::WORKER_OP_TYPE_ENTITY_QUERY_RESPONSE:
      detail::HandleEntityQueryResponse(impl, op.op.entity_query_response);
      break;
    case detail::internal::WORKER_OP_TYPE_ADD_COMPONENT:
      detail::HandleAddComponent(impl, op.op.add_component);
      break;
    case detail::internal::WORKER_OP_TYPE_REMOVE_COMPONENT:
      detail::HandleRemoveComponent(impl, op.op.remove_component);
      break;
    case detail::internal::WORKER_OP_TYPE_AUTHORITY_CHANGE:
      detail::HandleAuthorityChange(impl, op.op.authority_change);
      break;
    case detail::internal::WORKER_OP_TYPE_COMPONENT_UPDATE:
      detail::HandleComponentUpdate(impl, op.op.component_update);
      break;
    case detail::internal::WORKER_OP_TYPE_COMMAND_REQUEST:
      detail::HandleCommandRequest(impl, op.op.command_request);
      break;
    case detail::internal::WORKER_OP_TYPE_COMMAND_RESPONSE:
      detail::HandleCommandResponse(impl, op.op.command_response);
      break;
    }
  }
}

}  // namespace worker

#endif  // WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_DISPATCHER_I_H

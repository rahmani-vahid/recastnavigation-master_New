// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#ifndef WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_CONNECTION_I_H
#define WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_CONNECTION_I_H
#include <improbable/detail/client_handle.i.h>
#include <improbable/detail/extract.i.h>
#include <improbable/detail/result.i.h>
#include <improbable/worker.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace worker {
namespace detail {

struct ConnectionParameterStorage {
  internal::Worker_Alpha_KcpParameters DownstreamKcp;
  internal::Worker_Alpha_KcpParameters UpstreamKcp;
  internal::Worker_ErasureCodecParameters DownstreamErasureCodec;
  internal::Worker_ErasureCodecParameters UpstreamErasureCodec;
  internal::Worker_HeartbeatParameters DownstreamHeartbeat;
  internal::Worker_HeartbeatParameters UpstreamHeartbeat;
  internal::Worker_Alpha_FlowControlParameters FlowControl;
};

inline void LocatorDeploymentListThunk(void* user_data,
                                       const internal::Worker_DeploymentList* deployment_list) {
  const auto& callback = *static_cast<std::function<void(const DeploymentList&)>*>(user_data);
  DeploymentList wrapper;
  for (std::uint32_t i = 0; i < deployment_list->deployment_count; ++i) {
    const auto& protocol_deployment = deployment_list->deployments[i];
    wrapper.Deployments.emplace_back();
    auto& deployment = wrapper.Deployments.back();
    deployment.DeploymentName = protocol_deployment.deployment_name;
    deployment.AssemblyName = protocol_deployment.assembly_name;
    deployment.Description = protocol_deployment.description;
    deployment.UsersConnected = protocol_deployment.users_connected;
    deployment.UsersCapacity = protocol_deployment.users_capacity;
  }
  if (deployment_list->error) {
    wrapper.Error.emplace(deployment_list->error);
  }
  callback(wrapper);
}

inline std::uint8_t LocatorQueueStatusThunk(void* user_data,
                                            const internal::Worker_QueueStatus* queue_status) {
  const auto& callback = *static_cast<Locator::QueueStatusCallback*>(user_data);
  QueueStatus wrapper;
  wrapper.PositionInQueue = queue_status->position_in_queue;
  if (queue_status->error) {
    wrapper.Error.emplace(queue_status->error);
  }
  return static_cast<std::uint8_t>(callback(wrapper));
}

inline void ConnectionGetWorkerFlagThunk(void* user_data, const char* value) {
  const auto& callback = *static_cast<std::function<void(const Option<std::string>&)>*>(user_data);
  Option<std::string> cpp_flag_value;
  if (value) {
    cpp_flag_value.emplace(value);
  }
  callback(cpp_flag_value);
}

inline internal::Worker_LocatorParameters
ConvertLocatorParameters(const LocatorParameters& params) {
  internal::Worker_LocatorParameters protocol_params = {};
  protocol_params.project_name = params.ProjectName.c_str();
  if (params.CredentialsType == LocatorCredentialsType::kLoginToken) {
    protocol_params.credentials_type = internal::WORKER_LOCATOR_LOGIN_TOKEN_CREDENTIALS;
    protocol_params.login_token.token = params.LoginToken.Token.c_str();
  }
  if (params.CredentialsType == LocatorCredentialsType::kSteam) {
    protocol_params.credentials_type = internal::WORKER_LOCATOR_STEAM_CREDENTIALS;
    protocol_params.steam.ticket = params.Steam.Ticket.c_str();
    protocol_params.steam.deployment_tag = params.Steam.DeploymentTag.c_str();
  }
  if (params.CredentialsType == LocatorCredentialsType::kPlayerIdentity) {
    protocol_params.credentials_type = internal::WORKER_LOCATOR_PLAYER_IDENTITY_CREDENTIALS;
    protocol_params.player_identity.player_identity_token =
        params.PlayerIdentity.PlayerIdentityToken.c_str();
    protocol_params.player_identity.login_token = params.PlayerIdentity.LoginToken.c_str();
  }
  protocol_params.enable_logging = params.EnableLogging;
  protocol_params.logging.log_prefix = params.Logging.LogPrefix.c_str();
  protocol_params.logging.max_log_files = params.Logging.MaxLogFiles;
  protocol_params.logging.max_log_file_size_bytes = params.Logging.MaxLogFileSizeBytes;
  protocol_params.use_insecure_connection = params.UseInsecureConnection;
  return protocol_params;
}

inline internal::Worker_ConnectionParameters
ConvertConnectionParameters(const ConnectionParameters& params, ConnectionParameterStorage& storage,
                            const detail::ComponentInfo& component_info) {
  internal::Worker_ConnectionParameters protocol_params = {};
  protocol_params.worker_type = params.WorkerType.c_str();

  switch (params.Network.ConnectionType) {
  case NetworkConnectionType::kTcp:
    protocol_params.network.connection_type =
        static_cast<std::uint8_t>(internal::WORKER_NETWORK_CONNECTION_TYPE_TCP);
    break;
  case NetworkConnectionType::kRaknet:
    protocol_params.network.connection_type =
        static_cast<std::uint8_t>(internal::WORKER_NETWORK_CONNECTION_TYPE_RAKNET);
    break;
  case NetworkConnectionType::kKcp:
    protocol_params.network.connection_type =
        static_cast<std::uint8_t>(internal::WORKER_NETWORK_CONNECTION_TYPE_KCP);
    break;
  case NetworkConnectionType::kModularUdp:
    protocol_params.network.connection_type =
        static_cast<std::uint8_t>(internal::WORKER_NETWORK_CONNECTION_TYPE_MODULAR_UDP);
    break;
  }
  protocol_params.network.use_external_ip = params.Network.UseExternalIp;
  protocol_params.network.connection_timeout_millis = params.Network.ConnectionTimeoutMillis;
  protocol_params.network.default_command_timeout_millis =
      params.Network.DefaultCommandTimeoutMillis;
  protocol_params.network.tcp.multiplex_level = params.Network.Tcp.MultiplexLevel;
  protocol_params.network.tcp.no_delay = params.Network.Tcp.NoDelay;
  protocol_params.network.tcp.send_buffer_size = params.Network.Tcp.SendBufferSize;
  protocol_params.network.tcp.receive_buffer_size = params.Network.Tcp.ReceiveBufferSize;
  protocol_params.network.raknet.heartbeat_timeout_millis =
      params.Network.RakNet.HeartbeatTimeoutMillis;
  protocol_params.network.kcp.fast_retransmission = params.Network.Kcp.FastRetransmission;
  protocol_params.network.kcp.early_retransmission = params.Network.Kcp.EarlyRetransmission;
  protocol_params.network.kcp.non_concessional_flow_control =
      params.Network.Kcp.NonConcessionalFlowControl;
  protocol_params.network.kcp.multiplex_level = params.Network.Kcp.MultiplexLevel;
  protocol_params.network.kcp.update_interval_millis = params.Network.Kcp.UpdateIntervalMillis;
  protocol_params.network.kcp.min_rto_millis = params.Network.Kcp.MinRtoMillis;
  protocol_params.network.kcp.send_window_size = params.Network.Kcp.SendWindowSize;
  protocol_params.network.kcp.recv_window_size = params.Network.Kcp.RecvWindowSize;
  protocol_params.network.kcp.enable_erasure_codec = params.Network.Kcp.EnableErasureCodec;
  protocol_params.network.kcp.erasure_codec.original_packet_count =
      params.Network.Kcp.ErasureCodec.OriginalPacketCount;
  protocol_params.network.kcp.erasure_codec.recovery_packet_count =
      params.Network.Kcp.ErasureCodec.RecoveryPacketCount;
  protocol_params.network.kcp.erasure_codec.window_size =
      params.Network.Kcp.ErasureCodec.WindowSize;
  protocol_params.network.kcp.heartbeat.interval_millis =
      params.Network.Kcp.Heartbeat.IntervalMillis;
  protocol_params.network.kcp.heartbeat.timeout_millis = params.Network.Kcp.Heartbeat.TimeoutMillis;

  auto set_kcp_base = [](const Option<alpha::KcpParameters>& cpp,
                         const internal::Worker_Alpha_KcpParameters*& c,
                         internal::Worker_Alpha_KcpParameters& storage) {
    if (cpp) {
      storage.fast_retransmission = cpp->FastRetransmission;
      storage.early_retransmission = cpp->EarlyRetransmission;
      storage.non_concessional_flow_control = cpp->NonConcessionalFlowControl;
      storage.multiplex_level = cpp->MultiplexLevel;
      storage.update_interval_millis = cpp->UpdateIntervalMillis;
      storage.min_rto_millis = cpp->MinRtoMillis;
      c = &storage;
    } else {
      c = nullptr;
    }
  };

  auto set_erasure_codec = [](const Option<ErasureCodecParameters>& cpp,
                              const internal::Worker_ErasureCodecParameters*& c,
                              internal::Worker_ErasureCodecParameters& storage) {
    if (cpp) {
      storage.original_packet_count = cpp->OriginalPacketCount;
      storage.recovery_packet_count = cpp->RecoveryPacketCount;
      storage.window_size = cpp->WindowSize;
      c = &storage;
    } else {
      c = nullptr;
    }
  };

  auto set_heartbeat = [](const Option<HeartbeatParameters>& cpp,
                          const internal::Worker_HeartbeatParameters*& c,
                          internal::Worker_HeartbeatParameters& storage) {
    if (cpp) {
      storage.interval_millis = cpp->IntervalMillis;
      storage.timeout_millis = cpp->TimeoutMillis;
      c = &storage;
    } else {
      c = nullptr;
    }
  };

  const auto& cpp_udp = params.Network.ModularUdp;
  auto& c_udp = protocol_params.network.modular_udp;

  switch (cpp_udp.SecurityType) {
  case NetworkSecurityType::kInsecure:
    c_udp.security_type =
        static_cast<std::uint8_t>(internal::WORKER_NETWORK_SECURITY_TYPE_INSECURE);
    break;
  case NetworkSecurityType::kDtls:
    c_udp.security_type = static_cast<std::uint8_t>(internal::WORKER_NETWORK_SECURITY_TYPE_DTLS);
    break;
  }

  set_kcp_base(cpp_udp.DownstreamKcp, c_udp.downstream_kcp, storage.DownstreamKcp);
  set_kcp_base(cpp_udp.UpstreamKcp, c_udp.upstream_kcp, storage.UpstreamKcp);
  set_erasure_codec(cpp_udp.DownstreamErasureCodec, c_udp.downstream_erasure_codec,
                    storage.DownstreamErasureCodec);
  set_erasure_codec(cpp_udp.UpstreamErasureCodec, c_udp.upstream_erasure_codec,
                    storage.UpstreamErasureCodec);
  set_heartbeat(cpp_udp.DownstreamHeartbeat, c_udp.downstream_heartbeat,
                storage.DownstreamHeartbeat);
  set_heartbeat(cpp_udp.UpstreamHeartbeat, c_udp.upstream_heartbeat, storage.UpstreamHeartbeat);

  if (cpp_udp.FlowControl) {
    storage.FlowControl.downstream_window_size_bytes =
        cpp_udp.FlowControl->DownstreamWindowSizeBytes;
    storage.FlowControl.upstream_window_size_bytes = cpp_udp.FlowControl->UpstreamWindowSizeBytes;
    c_udp.flow_control = &storage.FlowControl;
  } else {
    c_udp.flow_control = nullptr;
  }

  protocol_params.send_queue_capacity = params.SendQueueCapacity;
  protocol_params.receive_queue_capacity = params.ReceiveQueueCapacity;
  protocol_params.log_message_queue_capacity = params.LogMessageQueueCapacity;
  protocol_params.built_in_metrics_report_period_millis = params.BuiltInMetricsReportPeriodMillis;
  protocol_params.enable_protocol_logging_at_startup = params.EnableProtocolLoggingAtStartup;
  protocol_params.protocol_logging.log_prefix = params.ProtocolLogging.LogPrefix.c_str();
  protocol_params.protocol_logging.max_log_file_size_bytes =
      params.ProtocolLogging.MaxLogFileSizeBytes;
  protocol_params.protocol_logging.max_log_files = params.ProtocolLogging.MaxLogFiles;
  protocol_params.thread_affinity.receive_threads_affinity_mask =
      params.ThreadAffinity.ReceiveThreadsAffinityMask;
  protocol_params.thread_affinity.send_threads_affinity_mask =
      params.ThreadAffinity.SendThreadsAffinityMask;
  protocol_params.thread_affinity.temporary_threads_affinity_mask =
      params.ThreadAffinity.TemporaryThreadsAffinityMask;
  protocol_params.enable_dynamic_components = params.EnableDynamicComponents;

  protocol_params.component_vtable_count =
      static_cast<std::uint32_t>(component_info.Vtables.size());
  protocol_params.component_vtables =
      reinterpret_cast<const internal::Worker_ComponentVtable*>(component_info.Vtables.data());
  return protocol_params;
}

inline internal::Worker_Constraint
ConvertConstraint(const query::Constraint& constraint,
                  std::vector<std::unique_ptr<internal::Worker_Constraint[]>>& storage) {
  internal::Worker_Constraint result = {};
  if (const auto* entity_id = constraint.data<query::EntityIdConstraint>()) {
    result.constraint_type = internal::WORKER_CONSTRAINT_TYPE_ENTITY_ID;
    result.constraint.entity_id_constraint.entity_id = entity_id->EntityId;
  } else if (const auto* component = constraint.data<query::ComponentConstraint>()) {
    result.constraint_type = internal::WORKER_CONSTRAINT_TYPE_COMPONENT;
    result.constraint.component_constraint.component_id = component->ComponentId;
  } else if (const auto* sphere = constraint.data<query::SphereConstraint>()) {
    result.constraint_type = internal::WORKER_CONSTRAINT_TYPE_SPHERE;
    result.constraint.sphere_constraint.x = sphere->X;
    result.constraint.sphere_constraint.y = sphere->Y;
    result.constraint.sphere_constraint.z = sphere->Z;
    result.constraint.sphere_constraint.radius = sphere->Radius;
  } else if (const auto* and_constraint = constraint.data<query::AndConstraint>()) {
    auto size = static_cast<std::uint32_t>(and_constraint->size());
    storage.emplace_back(new internal::Worker_Constraint[size]);
    auto ptr = storage.back().get();
    for (std::uint32_t i = 0; i < size; ++i) {
      ptr[i] = ConvertConstraint((*and_constraint)[i], storage);
    }

    result.constraint_type = internal::WORKER_CONSTRAINT_TYPE_AND;
    result.constraint.and_constraint.constraint_count = size;
    result.constraint.and_constraint.constraints = ptr;
  } else if (const auto* or_constraint = constraint.data<query::OrConstraint>()) {
    auto size = static_cast<std::uint32_t>(or_constraint->size());
    storage.emplace_back(new internal::Worker_Constraint[size]);
    auto ptr = storage.back().get();
    for (std::uint32_t i = 0; i < size; ++i) {
      ptr[i] = ConvertConstraint((*or_constraint)[i], storage);
    }

    result.constraint_type = internal::WORKER_CONSTRAINT_TYPE_OR;
    result.constraint.or_constraint.constraint_count = size;
    result.constraint.or_constraint.constraints = ptr;
  } else if (const auto* not_constraint = constraint.data<query::NotConstraint>()) {
    storage.emplace_back(new internal::Worker_Constraint[1]);
    auto ptr = storage.back().get();
    ptr[0] = ConvertConstraint(not_constraint->Constraint, storage);

    result.constraint_type = internal::WORKER_CONSTRAINT_TYPE_NOT;
    result.constraint.not_constraint.constraint = ptr;
  }
  return result;
}

inline void SendClientComponentUpdate(internal::Worker_Connection* connection, EntityId entity_id,
                                      ComponentId component_id, const ClientHandleBase* update,
                                      const UpdateParameters& parameters) {
  internal::Worker_ComponentUpdate component_update{nullptr, component_id, nullptr,
                                                    const_cast<ClientHandleBase*>(update)};
  internal::Worker_UpdateParameters update_parameters{
      static_cast<std::uint8_t>(parameters.Loopback)};
  internal::Worker_Connection_SendComponentUpdate(connection, entity_id, &component_update,
                                                  &update_parameters);
}

inline void SendClientAddComponent(internal::Worker_Connection* connection, EntityId entity_id,
                                   ComponentId component_id, const ClientHandleBase* add_component,
                                   const UpdateParameters& parameters) {
  internal::Worker_ComponentData component_data{nullptr, component_id, nullptr,
                                                const_cast<ClientHandleBase*>(add_component)};
  internal::Worker_UpdateParameters update_parameters{
      static_cast<std::uint8_t>(parameters.Loopback)};
  internal::Worker_Connection_SendAddComponent(connection, entity_id, &component_data,
                                               &update_parameters);
}

inline std::int64_t SendClientCommandRequest(internal::Worker_Connection* connection,
                                             EntityId entity_id, ComponentId component_id,
                                             const ClientHandleBase* request,
                                             std::uint32_t command_id,
                                             const Option<std::uint32_t>& timeout_millis,
                                             const CommandParameters& parameters) {
  internal::Worker_CommandRequest command_request{nullptr, component_id, command_id, nullptr,
                                                  const_cast<ClientHandleBase*>(request)};
  internal::Worker_CommandParameters command_parameters{
      static_cast<std::uint8_t>(parameters.AllowShortCircuit)};
  return internal::Worker_Connection_SendCommandRequest(connection, entity_id, &command_request,
                                                        timeout_millis.data(), &command_parameters);
}

inline void SendClientCommandResponse(internal::Worker_Connection* connection,
                                      std::int64_t request_id, ComponentId component_id,
                                      std::uint32_t command_id, const ClientHandleBase* response) {
  internal::Worker_CommandResponse command_response{nullptr, component_id, command_id, nullptr,
                                                    const_cast<ClientHandleBase*>(response)};
  internal::Worker_Connection_SendCommandResponse(connection, request_id, &command_response);
}

inline void SendClientCommandFailure(internal::Worker_Connection* connection,
                                     std::int64_t request_id, const std::string& message) {
  internal::Worker_Connection_SendCommandFailure(connection, request_id, message.c_str());
}

}  // namespace detail

inline Locator::Locator(const std::string& hostname, std::uint16_t port,
                        const LocatorParameters& params)
: locator{nullptr, &detail::internal::Worker_Locator_Destroy} {
  auto protocol_params = detail::ConvertLocatorParameters(params);
  locator.reset(detail::internal::Worker_Locator_Create(hostname.c_str(), port, &protocol_params));
}

inline Locator::Locator(const std::string& hostname, const LocatorParameters& params)
: Locator::Locator(hostname, 0 /* Use cloud port. */, params) {}

inline Future<DeploymentList> Locator::GetDeploymentListAsync() {
  auto future = Worker_Locator_GetDeploymentListAsync(locator.get());
  return Future<DeploymentList>(
      [future](const Option<std::uint32_t>& timeout_millis) {
        Option<DeploymentList> result;
        std::function<void(const DeploymentList& callback)> callback =
            [&](const DeploymentList& list) { result.emplace(list); };
        detail::internal::Worker_DeploymentListFuture_Get(future, timeout_millis.data(),
                                                          static_cast<void*>(&callback),
                                                          &detail::LocatorDeploymentListThunk);
        return result;
      },
      [future] { detail::internal::Worker_DeploymentListFuture_Destroy(future); });
}

inline Future<Connection> Locator::ConnectAsync(const ComponentRegistry& registry,
                                                const ConnectionParameters& params) {
  const auto& component_info = registry.GetInternalComponentInfo();
  std::unique_ptr<detail::internal::Worker_ComponentVtable[]> component_array;
  detail::ConnectionParameterStorage storage{};
  auto protocol_params = detail::ConvertConnectionParameters(params, storage, component_info);

  auto future = Worker_Locator_ConnectAsync(locator.get(), &protocol_params);
  return Future<Connection>(
      [future, &component_info](const Option<std::uint32_t>& timeout_millis) {
        Option<Connection> result;
        auto connection_ptr =
            detail::internal::Worker_ConnectionFuture_Get(future, timeout_millis.data());
        if (connection_ptr) {
          result.emplace(Connection{&component_info, connection_ptr});
        }
        return result;
      },
      [future] { detail::internal::Worker_ConnectionFuture_Destroy(future); });
}

inline Future<Connection> Locator::ConnectAsync(const ComponentRegistry& registry,
                                                const std::string& deployment_name,
                                                const ConnectionParameters& params,
                                                const QueueStatusCallback& callback) {
  const auto& component_info = registry.GetInternalComponentInfo();
  std::unique_ptr<detail::internal::Worker_ComponentVtable[]> component_array;
  detail::ConnectionParameterStorage storage{};
  auto protocol_params = detail::ConvertConnectionParameters(params, storage, component_info);
  auto shared_callback = std::make_shared<Locator::QueueStatusCallback>(callback);

  auto future = detail::internal::Worker_Locator_ConnectAndQueueAsync(
      locator.get(), deployment_name.c_str(), &protocol_params,
      static_cast<void*>(shared_callback.get()), &detail::LocatorQueueStatusThunk);

  return Future<Connection>(
      [future, shared_callback, &component_info](const Option<std::uint32_t>& timeout_millis) {
        Option<Connection> result;
        auto connection_ptr =
            detail::internal::Worker_ConnectionFuture_Get(future, timeout_millis.data());
        if (connection_ptr) {
          result.emplace(Connection{&component_info, connection_ptr});
        }
        return result;
      },
      [future] { detail::internal::Worker_ConnectionFuture_Destroy(future); });
}

inline Future<Connection> Connection::ConnectAsync(const ComponentRegistry& registry,
                                                   const std::string& receptionist_hostname,
                                                   std::uint16_t receptionist_port,
                                                   const std::string& worker_id,
                                                   const ConnectionParameters& params) {
  const auto& component_info = registry.GetInternalComponentInfo();
  detail::ConnectionParameterStorage storage{};
  auto protocol_params = ConvertConnectionParameters(params, storage, component_info);

  auto future = detail::internal::Worker_ConnectAsync(
      receptionist_hostname.c_str(), receptionist_port, worker_id.c_str(), &protocol_params);

  return Future<Connection>(
      [future, &component_info](const Option<std::uint32_t>& timeout_millis) {
        Option<Connection> result;
        auto connection_ptr =
            detail::internal::Worker_ConnectionFuture_Get(future, timeout_millis.data());
        if (connection_ptr) {
          result.emplace(Connection{&component_info, connection_ptr});
        }
        return result;
      },
      [future] { detail::internal::Worker_ConnectionFuture_Destroy(future); });
}

inline bool Connection::IsConnected() const {
  return detail::internal::Worker_Connection_IsConnected(connection.get()) != 0;
}

inline ConnectionStatusCode Connection::GetConnectionStatusCode() const {
  return static_cast<ConnectionStatusCode>(
      detail::internal::Worker_Connection_GetConnectionStatusCode(connection.get()));
}

inline std::string Connection::GetConnectionStatusDetailString() const {
  return detail::internal::Worker_Connection_GetConnectionStatusDetailString(connection.get());
}

inline std::string Connection::GetWorkerId() const {
  return detail::internal::Worker_Connection_GetWorkerId(connection.get());
}

inline worker::List<std::string> Connection::GetWorkerAttributes() const {
  auto worker_attributes =
      detail::internal::Worker_Connection_GetWorkerAttributes(connection.get());
  worker::List<std::string> result;
  for (std::uint32_t i = 0; i < worker_attributes->attribute_count; ++i) {
    result.emplace_back(worker_attributes->attributes[i]);
  }
  return result;
}

inline Option<std::string> Connection::GetWorkerFlag(const std::string& flag_name) const {
  Option<std::string> flag_value;
  std::function<void(const Option<std::string>&)> callback = [&](const Option<std::string>& val) {
    flag_value = val;
  };
  auto wrapper = static_cast<void*>(&callback);
  detail::internal::Worker_Connection_GetWorkerFlag(connection.get(), flag_name.c_str(), wrapper,
                                                    &detail::ConnectionGetWorkerFlagThunk);
  return flag_value;
}

inline OpList Connection::GetOpList(std::uint32_t timeout_millis) {
  return OpList{detail::internal::Worker_Connection_GetOpList(connection.get(), timeout_millis)};
}

inline void Connection::SendLogMessage(LogLevel level, const std::string& logger_name,
                                       const std::string& message,
                                       const Option<EntityId>& entity_id) {
  detail::internal::Worker_LogMessage log_message;
  log_message.level = static_cast<std::uint8_t>(level);
  log_message.logger_name = logger_name.c_str();
  log_message.message = message.c_str();
  log_message.entity_id = entity_id.data();
  detail::internal::Worker_Connection_SendLogMessage(connection.get(), &log_message);
}

inline void Connection::SendMetrics(Metrics& metrics) {
  detail::internal::Worker_Metrics protocol_metrics = {};
  protocol_metrics.load = metrics.Load.data();

  auto gauge_count = metrics.GaugeMetrics.size();
  auto histogram_count = metrics.HistogramMetrics.size();
  std::size_t total_bucket_count = 0;
  for (const auto& pair : metrics.HistogramMetrics) {
    total_bucket_count += pair.second.Buckets().size();
  }

  std::unique_ptr<detail::internal::Worker_GaugeMetric[]> gauge_data;
  std::unique_ptr<detail::internal::Worker_HistogramMetric[]> histogram_data;
  std::unique_ptr<detail::internal::Worker_HistogramMetricBucket[]> bucket_data;

  if (gauge_count) {
    gauge_data.reset(new detail::internal::Worker_GaugeMetric[gauge_count]);
    std::size_t index = 0;
    for (const auto& pair : metrics.GaugeMetrics) {
      gauge_data[index].key = pair.first.c_str();
      gauge_data[index].value = pair.second;
      ++index;
    }
    protocol_metrics.gauge_metric_count = static_cast<std::uint32_t>(gauge_count);
    protocol_metrics.gauge_metrics = gauge_data.get();
  } else {
    protocol_metrics.gauge_metric_count = 0;
    protocol_metrics.gauge_metrics = nullptr;
  }

  if (histogram_count) {
    histogram_data.reset(new detail::internal::Worker_HistogramMetric[histogram_count]);
    bucket_data.reset(new detail::internal::Worker_HistogramMetricBucket[total_bucket_count]);

    std::size_t index = 0;
    std::size_t bucket_index = 0;
    for (const auto& pair : metrics.HistogramMetrics) {
      histogram_data[index].key = pair.first.c_str();
      histogram_data[index].sum = pair.second.Sum();
      histogram_data[index].bucket_count = static_cast<std::uint32_t>(pair.second.Buckets().size());
      histogram_data[index].buckets = bucket_index + bucket_data.get();

      for (const auto& bucket : pair.second.Buckets()) {
        bucket_data[bucket_index].upper_bound = bucket.UpperBound;
        bucket_data[bucket_index].samples = bucket.Samples;
        ++bucket_index;
      }
      ++index;
    }
    protocol_metrics.histogram_metric_count = static_cast<std::uint32_t>(histogram_count);
    protocol_metrics.histogram_metrics = histogram_data.get();
  } else {
    protocol_metrics.histogram_metric_count = 0;
    protocol_metrics.histogram_metrics = nullptr;
  }

  detail::internal::Worker_Connection_SendMetrics(connection.get(), &protocol_metrics);
  for (auto& pair : metrics.HistogramMetrics) {
    pair.second.ClearObservations();
  }
}

inline RequestId<ReserveEntityIdsRequest>
Connection::SendReserveEntityIdsRequest(std::uint32_t number_of_entity_ids,
                                        const Option<std::uint32_t>& timeout_millis) {
  return RequestId<ReserveEntityIdsRequest>{
      detail::internal::Worker_Connection_SendReserveEntityIdsRequest(
          connection.get(), number_of_entity_ids, timeout_millis.data())};
}

inline Result<RequestId<CreateEntityRequest>>
Connection::SendCreateEntityRequest(const Entity& entity, const Option<EntityId>& entity_id,
                                    const Option<std::uint32_t>& timeout_millis) {
  std::vector<std::unique_ptr<detail::ClientHandleBase>> handle_storage;
  std::unique_ptr<detail::internal::Worker_ComponentData[]> component_snapshot_array;
  std::uint32_t component_count;
  for (const auto& component_id : entity.GetComponentIds()) {
    auto result = detail::CheckComponentRegistered(*component_info, component_id);
    if (!result) {
      return detail::ForwardFailure<RequestId<CreateEntityRequest>>(
          "Error sending create entity request" +
              (entity_id.empty() ? "" : " for reserved entity ID " + std::to_string(*entity_id)),
          result);
    }
  }
  ExtractEntityComponents(*component_info, entity, component_count, handle_storage,
                          component_snapshot_array);
  return Result<RequestId<CreateEntityRequest>>::Success(
      RequestId<CreateEntityRequest>{detail::internal::Worker_Connection_SendCreateEntityRequest(
          connection.get(), static_cast<std::uint32_t>(component_count),
          component_snapshot_array.get(), entity_id.data(), timeout_millis.data())});
}

inline RequestId<DeleteEntityRequest>
Connection::SendDeleteEntityRequest(EntityId entity_id,
                                    const Option<std::uint32_t>& timeout_millis) {
  return RequestId<DeleteEntityRequest>{detail::internal::Worker_Connection_SendDeleteEntityRequest(
      connection.get(), entity_id, timeout_millis.data())};
}

inline RequestId<EntityQueryRequest>
Connection::SendEntityQueryRequest(const query::EntityQuery& entity_query,
                                   const Option<std::uint32_t>& timeout_millis) {
  detail::internal::Worker_EntityQuery protocol_query;
  protocol_query.snapshot_result_type_component_id_count = 0;
  protocol_query.snapshot_result_type_component_ids = nullptr;
  if (entity_query.ResultType.data<query::CountResultType>()) {
    protocol_query.result_type = detail::internal::WORKER_RESULT_TYPE_COUNT;
  }
  std::unique_ptr<detail::internal::Worker_ComponentId[]> snapshot_result_type_storage;
  const auto* snapshot_result_type = entity_query.ResultType.data<query::SnapshotResultType>();
  if (snapshot_result_type) {
    protocol_query.result_type = detail::internal::WORKER_RESULT_TYPE_SNAPSHOT;
    if (!snapshot_result_type->ComponentIds.empty()) {
      auto size = static_cast<std::uint32_t>(snapshot_result_type->ComponentIds->size());
      snapshot_result_type_storage.reset(new detail::internal::Worker_ComponentId[size]);
      for (std::uint32_t i = 0; i < size; ++i) {
        snapshot_result_type_storage[i] = (*snapshot_result_type->ComponentIds)[i];
      }
      protocol_query.snapshot_result_type_component_id_count = size;
      protocol_query.snapshot_result_type_component_ids = snapshot_result_type_storage.get();
    }
  }
  std::vector<std::unique_ptr<detail::internal::Worker_Constraint[]>> storage;
  protocol_query.constraint = detail::ConvertConstraint(entity_query.Constraint, storage);
  return RequestId<EntityQueryRequest>{detail::internal::Worker_Connection_SendEntityQueryRequest(
      connection.get(), &protocol_query, timeout_millis.data())};
}

inline void
Connection::SendComponentInterest(EntityId entity_id,
                                  const Map<ComponentId, InterestOverride>& interest_overrides) {
  std::vector<detail::internal::Worker_InterestOverride> protocol_overrides;
  for (const auto& pair : interest_overrides) {
    protocol_overrides.push_back({pair.first, pair.second.IsInterested});
  }
  detail::internal::Worker_Connection_SendComponentInterest(
      connection.get(), entity_id, protocol_overrides.data(),
      static_cast<std::uint32_t>(protocol_overrides.size()));
}

template <typename T>
void Connection::SendAuthorityLossImminentAcknowledgement(EntityId entity_id) {
  detail::internal::Worker_Connection_SendAuthorityLossImminentAcknowledgement(
      connection.get(), entity_id, T::ComponentId);
}

template <typename T>
Result<None> Connection::SendAddComponent(EntityId entity_id, const typename T::Data& data,
                                          const worker::UpdateParameters& parameters) {
  auto result = detail::CheckComponentRegistered(*component_info, T::ComponentId);
  if (!result) {
    return detail::ForwardFailure<None>(
        "Error adding component to entity " + std::to_string(entity_id), result);
  }
  auto handle = detail::ClientHandle<typename T::Data>::copyable(&data);
  detail::SendClientAddComponent(connection.get(), entity_id, T::ComponentId, &handle, parameters);
  return Result<None>::Success({});
}

template <typename T>
Result<None> Connection::SendAddComponent(EntityId entity_id, typename T::Data&& data,
                                          const worker::UpdateParameters& parameters) {
  auto result = detail::CheckComponentRegistered(*component_info, T::ComponentId);
  if (!result) {
    return detail::ForwardFailure<None>(
        "Error adding component to entity " + std::to_string(entity_id), result);
  }
  auto handle = detail::ClientHandle<typename T::Data>::moveable(&data);
  detail::SendClientAddComponent(connection.get(), entity_id, T::ComponentId, &handle, parameters);
  return Result<None>::Success({});
}

template <typename T>
Result<None> Connection::SendRemoveComponent(std::int64_t entity_id,
                                             const worker::UpdateParameters& parameters) {
  auto result = detail::CheckComponentRegistered(*component_info, T::ComponentId);
  if (!result) {
    return detail::ForwardFailure<None>(
        "Error adding component to entity " + std::to_string(entity_id), result);
  }
  detail::internal::Worker_UpdateParameters update_parameters{
      static_cast<std::uint8_t>(parameters.Loopback)};
  detail::internal::Worker_Connection_SendRemoveComponent(connection.get(), entity_id,
                                                          T::ComponentId, &update_parameters);
  return Result<None>::Success({});
}

template <typename T>
Result<None> Connection::SendComponentUpdate(EntityId entity_id, const typename T::Update& update,
                                             const UpdateParameters& parameters) {
  auto result = detail::CheckComponentRegistered(*component_info, T::ComponentId);
  if (!result) {
    return detail::ForwardFailure<None>(
        "Error sending component update for entity " + std::to_string(entity_id), result);
  }
  auto handle = detail::ClientHandle<typename T::Update>::copyable(&update);
  detail::SendClientComponentUpdate(connection.get(), entity_id, T::ComponentId, &handle,
                                    parameters);
  return Result<None>::Success({});
}

template <typename T>
Result<None> Connection::SendComponentUpdate(EntityId entity_id, typename T::Update&& update,
                                             const UpdateParameters& parameters) {
  auto result = detail::CheckComponentRegistered(*component_info, T::ComponentId);
  if (!result) {
    return detail::ForwardFailure<None>(
        "Error sending component update for entity " + std::to_string(entity_id), result);
  }
  auto handle = detail::ClientHandle<typename T::Update>::movable(&update);
  detail::SendClientComponentUpdate(connection.get(), entity_id, T::ComponentId, &handle,
                                    parameters);
  return Result<None>::Success({});
}

template <typename T>
Result<RequestId<OutgoingCommandRequest<T>>>
Connection::SendCommandRequest(EntityId entity_id, const typename T::Request& request,
                               const Option<std::uint32_t>& timeout_millis,
                               const CommandParameters& parameters) {
  auto result =
      detail::CheckComponentRegistered(*component_info, T::ComponentMetaclass::ComponentId);
  if (!result) {
    return detail::ForwardFailure<RequestId<OutgoingCommandRequest<T>>>(
        "Error sending command request", result);
  }
  using GenericRequest = typename T::ComponentMetaclass::GenericCommandObject;
  GenericRequest generic_request{T::CommandIndex, request};
  auto handle = detail::ClientHandle<GenericRequest>::movable(&generic_request);

  return Result<RequestId<OutgoingCommandRequest<T>>>::Success(RequestId<OutgoingCommandRequest<T>>{
      detail::SendClientCommandRequest(connection.get(), entity_id,
                                       T::ComponentMetaclass::ComponentId, &handle, T::CommandIndex,
                                       timeout_millis, parameters)});
}

template <typename T>
Result<RequestId<OutgoingCommandRequest<T>>>
Connection::SendCommandRequest(EntityId entity_id, typename T::Request&& request,
                               const Option<std::uint32_t>& timeout_millis,
                               const CommandParameters& parameters) {
  auto result =
      detail::CheckComponentRegistered(*component_info, T::ComponentMetaclass::ComponentId);
  if (!result) {
    return detail::ForwardFailure<RequestId<OutgoingCommandRequest<T>>>(
        "Error sending command request", result);
  }
  using GenericRequest = typename T::ComponentMetaclass::GenericCommandObject;
  GenericRequest generic_request{T::CommandIndex, std::move(request)};
  auto handle = detail::ClientHandle<GenericRequest>::movable(&generic_request);

  return Result<RequestId<OutgoingCommandRequest<T>>>::Success(RequestId<OutgoingCommandRequest<T>>{
      detail::SendClientCommandRequest(connection.get(), entity_id,
                                       T::ComponentMetaclass::ComponentId, &handle, T::CommandIndex,
                                       timeout_millis, parameters)});
}

template <typename T>
Result<None> Connection::SendCommandResponse(RequestId<IncomingCommandRequest<T>> request_id,
                                             const typename T::Response& response) {
  auto result =
      detail::CheckComponentRegistered(*component_info, T::ComponentMetaclass::ComponentId);
  if (!result) {
    return detail::ForwardFailure<None>("Error sending command response", result);
  }
  using GenericResponse = typename T::ComponentMetaclass::GenericCommandObject;
  GenericResponse generic_response{T::CommandIndex, response};
  auto handle = detail::ClientHandle<GenericResponse>::movable(&generic_response);

  detail::SendClientCommandResponse(connection.get(), request_id.Id,
                                    T::ComponentMetaclass::ComponentId, T::CommandIndex, &handle);
  return Result<None>::Success({});
}

template <typename T>
Result<None> Connection::SendCommandResponse(RequestId<IncomingCommandRequest<T>> request_id,
                                             typename T::Response&& response) {
  auto result =
      detail::CheckComponentRegistered(*component_info, T::ComponentMetaclass::ComponentId);
  if (!result) {
    return detail::ForwardFailure<None>("Error sending command response", result);
  }
  using GenericResponse = typename T::ComponentMetaclass::GenericCommandObject;
  GenericResponse generic_response{T::CommandIndex, std::move(response)};
  auto handle = detail::ClientHandle<GenericResponse>::movable(&generic_response);

  detail::SendClientCommandResponse(connection.get(), request_id.Id,
                                    T::ComponentMetaclass::ComponentId, T::CommandIndex, &handle);
  return Result<None>::Success({});
}

template <typename T>
void Connection::SendCommandFailure(RequestId<IncomingCommandRequest<T>> request_id,
                                    const std::string& message) {
  detail::SendClientCommandFailure(connection.get(), request_id.Id, message);
}

inline void Connection::SetProtocolLoggingEnabled(bool enabled) {
  detail::internal::Worker_Connection_SetProtocolLoggingEnabled(connection.get(), enabled);
}

inline Connection::Connection(const detail::ComponentInfo* component_info,
                              detail::internal::Worker_Connection* connection)
: component_info{component_info}
, connection{connection, &detail::internal::Worker_Connection_Destroy} {}

inline OpList::OpList(detail::internal::Worker_OpList* op_list)
: op_list{op_list, &detail::internal::Worker_OpList_Destroy} {}

}  // namespace worker

#endif  // WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_CONNECTION_I_H

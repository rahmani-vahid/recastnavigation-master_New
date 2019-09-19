// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#ifndef WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_DEVELOPMENT_AUTH_I_H
#define WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_DEVELOPMENT_AUTH_I_H
#include <improbable/collections.h>
#include <improbable/worker.h>

namespace worker {
namespace detail {
inline void DevelopmentPlayerIdentityTokenThunk(
    void* user_data,
    const internal::Worker_Alpha_PlayerIdentityTokenResponse* player_identity_token) {
  auto& response = *static_cast<alpha::PlayerIdentityTokenResponse*>(user_data);
  auto status = static_cast<worker::ConnectionStatusCode>(player_identity_token->status.code);

  response.Status = {status, std::string{player_identity_token->status.detail}};
  if (status == worker::ConnectionStatusCode::kSuccess) {
    response.PlayerIdentityToken = std::string{player_identity_token->player_identity_token};
  }
}

inline void
DevelopmentLoginTokenListThunk(void* user_data,
                               const internal::Worker_Alpha_LoginTokensResponse* login_tokens) {
  auto& response = *static_cast<alpha::LoginTokensResponse*>(user_data);
  auto status = static_cast<worker::ConnectionStatusCode>(login_tokens->status.code);
  response.Status = {status, std::string{login_tokens->status.detail}};
  if (status != worker::ConnectionStatusCode::kSuccess) {
    return;
  }
  auto& login_tokens_list = response.LoginTokens;
  for (uint32_t i = 0; i < login_tokens->login_token_count; i++) {
    auto& protocol_login_token = login_tokens->login_tokens[i];
    List<std::string> tags_list;
    for (uint32_t j = 0; j < protocol_login_token.tag_count; j++) {
      tags_list.emplace_back(protocol_login_token.tags[j]);
    }
    alpha::LoginTokenDetails login_token_details{protocol_login_token.deployment_id,
                                                 protocol_login_token.deployment_name, tags_list,
                                                 protocol_login_token.login_token};
    login_tokens_list.emplace_back(login_token_details);
  }
}
}  // namespace detail

namespace alpha {
inline Future<PlayerIdentityTokenResponse>
CreateDevelopmentPlayerIdentityTokenAsync(const std::string& hostname, std::uint16_t port,
                                          const alpha::PlayerIdentityTokenRequest& params) {
  detail::internal::Worker_Alpha_PlayerIdentityTokenRequest protocol_params = {};
  protocol_params.player_id = params.PlayerId.c_str();
  protocol_params.development_authentication_token = params.DevelopmentAuthenticationToken.c_str();
  protocol_params.display_name = params.DisplayName.c_str();
  protocol_params.duration_seconds = params.DurationSeconds.data();
  protocol_params.use_insecure_connection = params.UseInsecureConnection;
  protocol_params.metadata = params.Metadata.c_str();

  auto* future = detail::internal::Worker_Alpha_CreateDevelopmentPlayerIdentityTokenAsync(
      hostname.c_str(), port, &protocol_params);

  return Future<PlayerIdentityTokenResponse>(
      [future](const Option<std::uint32_t>& timeout_millis) {
        PlayerIdentityTokenResponse response;
        detail::internal::Worker_Alpha_PlayerIdentityTokenResponseFuture_Get(
            future, timeout_millis.data(), &response, &detail::DevelopmentPlayerIdentityTokenThunk);
        return response;
      },
      [future] {
        detail::internal::Worker_Alpha_PlayerIdentityTokenResponseFuture_Destroy(future);
      });
}

inline Future<LoginTokensResponse>
CreateDevelopmentLoginTokensAsync(const std::string& hostname, std::uint16_t port,
                                  const LoginTokensRequest& params) {
  detail::internal::Worker_Alpha_LoginTokensRequest protocol_params = {};
  protocol_params.player_identity_token = params.PlayerIdentityToken.c_str();
  protocol_params.use_insecure_connection = params.UseInsecureConnection;
  protocol_params.duration_seconds = params.DurationSeconds.data();
  protocol_params.worker_type = params.WorkerType.c_str();

  auto* future = detail::internal::Worker_Alpha_CreateDevelopmentLoginTokensAsync(
      hostname.c_str(), port, &protocol_params);

  return Future<LoginTokensResponse>(
      [future](const Option<std::uint32_t>& timeout_millis) {
        LoginTokensResponse response;
        detail::internal::Worker_Alpha_LoginTokensResponseFuture_Get(
            future, timeout_millis.data(), &response, &detail::DevelopmentLoginTokenListThunk);
        return response;
      },
      [future] { detail::internal::Worker_Alpha_LoginTokensResponseFuture_Destroy(future); });
}

}  // namespace alpha
}  // namespace worker
#endif  // WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_DEVELOPMENT_AUTH_I_H

// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#ifndef WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_RESULT_I_H
#define WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_RESULT_I_H
#include <improbable/worker.h>
#include <cassert>

namespace worker {
namespace detail {

template <typename ToT, typename E, typename FromT>
Result<ToT, E> ForwardFailure(const std::string& prefix, const Result<FromT, E>& error) {
  return Result<ToT, E>::Failure(prefix + ": " + error.GetErrorMessage(), error.GetErrorCode());
}

template <typename E>
struct Error {
  Error(const std::string& message, const E& code) : message{message}, code{code} {}
  Error(const std::string& message, E&& code) : message{message}, code{std::move(code)} {}
  Error(std::string&& message, const E& code) : message{std::move(message)}, code{code} {}
  Error(std::string&& message, E&& code) : message{std::move(message)}, code{std::move(code)} {}

  std::string message;
  E code;
};
}  // namespace detail

template <typename T, typename E>
Result<T, E> Result<T, E>::Success(const T& value) {
  Result<T, E> result;
  result.value = value;
  return result;
}

template <typename T, typename E>
Result<T, E> Result<T, E>::Success(T&& value) {
  Result<T, E> result;
  result.value = std::move(value);
  return result;
}

template <typename T, typename E>
Result<T, E> Result<T, E>::Failure(const std::string& error, const E& code) {
  Result<T, E> result;
  result.error = detail::Error<E>(error, code);
  return result;
}

template <typename T, typename E>
Result<T, E> Result<T, E>::Failure(std::string&& error, const E& code) {
  Result<T, E> result;
  result.error = detail::Error<E>(std::move(error), code);
  return result;
}

template <typename T, typename E>
Result<T, E> Result<T, E>::Failure(const std::string& error, E&& code) {
  Result<T, E> result;
  result.error = detail::Error<E>(error, std::move(code));
  return result;
}

template <typename T, typename E>
Result<T, E> Result<T, E>::Failure(std::string&& error, E&& code) {
  Result<T, E> result;
  result.error = detail::Error<E>(std::move(error), std::move(code));
  return result;
}

template <typename T, typename E>
bool Result<T, E>::HasError() const {
  return static_cast<bool>(error);
}

template <typename T, typename E>
bool Result<T, E>::HasValue() const {
  return static_cast<bool>(value);
}

template <typename T, typename E>
const T& Result<T, E>::GetValue() const {
  if (HasError()) {
    assert(!"GetValue called when an error is present.");
    std::terminate();
  }
  return *value;
}

template <typename T, typename E>
T& Result<T, E>::GetValue() {
  if (HasError()) {
    assert(!"GetValue called when an error is present.");
    std::terminate();
  }
  return *value;
}

template <typename T, typename E>
const std::string& Result<T, E>::GetErrorMessage() const {
  if (!HasError()) {
    assert(!"GetError called when no error is present.");
    std::terminate();
  }
  return error->message;
}

template <typename T, typename E>
const E& Result<T, E>::GetErrorCode() const {
  if (!HasError()) {
    assert(!"GetError called when no error is present.");
    std::terminate();
  }
  return error->code;
}

template <typename T, typename E>
Result<T, E>::operator bool() const {
  return HasValue();
}

template <typename T, typename E>
const T& Result<T, E>::operator*() const {
  return GetValue();
}

template <typename T, typename E>
T& Result<T, E>::operator*() {
  return GetValue();
}

template <typename T, typename E>
const T* Result<T, E>::operator->() const {
  return &GetValue();
}

template <typename T, typename E>
T* Result<T, E>::operator->() {
  return &GetValue();
}

template <typename T, typename E>
Result<T, E>::Result() {}

}  // namespace worker

#endif  // WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_RESULT_I_H
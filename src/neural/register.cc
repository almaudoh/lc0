/*
  This file is part of Leela Chess Zero.
  Copyright (C) 2024 The LCZero Authors

  Leela Chess is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Leela Chess is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Leela Chess.  If not, see <http://www.gnu.org/licenses/>.

  Additional permission under GNU GPL version 3 section 7

  If you modify this Program, or any covered work, by linking or
  combining it with NVIDIA Corporation's libraries from the NVIDIA CUDA
  Toolkit and the NVIDIA CUDA Deep Neural Network library (or a
  modified version of those libraries), containing parts covered by the
  terms of the respective license agreement, the licensors of this
  Program grant you additional permission to convey the resulting work.
*/

#include "neural/register.h"

#include <algorithm>

#include "default_backend.h"
#include "neural/shared_params.h"

namespace lczero {

BackendManager* BackendManager::Get() {
  static BackendManager instance;
  return &instance;
}

std::vector<std::string> BackendManager::GetBackendNames() const {
  std::vector<std::pair<int, std::string>> priority_and_names;
  std::transform(algorithms_.begin(), algorithms_.end(),
                 std::back_inserter(priority_and_names),
                 [](const std::unique_ptr<BackendFactory>& factory) {
                   return std::make_pair(factory->GetPriority(),
                                         std::string(factory->GetName()));
                 });
  std::sort(priority_and_names.begin(), priority_and_names.end(),
            std::greater<>());
  std::vector<std::string> result;
  std::transform(priority_and_names.begin(), priority_and_names.end(),
                 std::back_inserter(result),
                 [](const std::pair<int, std::string>& p) { return p.second; });
#ifdef DEFAULT_BACKEND
  std::string name = DEFAULT_BACKEND;
  auto pos = std::find(result.begin(), result.end(), name);
  if (pos == result.end()) throw Exception("Unknown backend: " + name);
  std::rotate(result.begin(), pos, pos + 1);
#endif
  return result;
}

BackendFactory* BackendManager::GetFactoryByName(std::string_view name) const {
  auto iter =
      std::find_if(algorithms_.begin(), algorithms_.end(),
                   [name](const std::unique_ptr<BackendFactory>& factory) {
                     return factory->GetName() == name;
                   });
  return iter == algorithms_.end() ? nullptr : iter->get();
}

std::unique_ptr<Backend> BackendManager::CreateFromParams(
    const OptionsDict& options) const {
  const std::string backend =
      options.Get<std::string>(SharedBackendParams::kBackendId);
  return CreateFromName(backend, options);
}

std::unique_ptr<Backend> BackendManager::CreateFromName(
    std::string_view name, const OptionsDict& options) const {
  BackendFactory* factory = GetFactoryByName(name);
  if (!factory) throw Exception("Unknown backend: " + std::string(name));
  return factory->Create(options);
}

void BackendManager::RemoveBackend(const BackendFactory* factory) {
  auto iter = std::find_if(algorithms_.begin(), algorithms_.end(),
                           [factory](const std::unique_ptr<BackendFactory>& f) {
                             return f.get() == factory;
                           });
  if (iter == algorithms_.end()) {
    throw Exception("Attempt to remove unregistered backend");
  }
  algorithms_.erase(iter);
}

}  // namespace lczero
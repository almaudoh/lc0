/*
 This file is part of Leela Chess Zero.
 Copyright (C) 2018-2022 The LCZero Authors

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
 */

#pragma once

#include <cstddef>
#include <vector>

#include "proto/net.pb.h"

namespace lczero {
// The following list matches the one in net.proto. Ideally this would be done
// by including proto/net.pb.h, but this is incompatible with nvcc.
enum ActivationFunction {
  DEFAULT = 0,
  MISH = 1,
  RELU = 2,
  NONE = 3,
  TANH = 4,
  SIGMOID = 5,
  SELU = 6,
  SWISH = 7,
  RELU_2 = 8,
  SOFTMAX = 9,
};

// Softmax activation
void SoftmaxActivation(const size_t size, const float* input, float* output);

void BiasResidual(const size_t batch_size, const size_t channels, float* data,
                  const float* biases, const float* eltwise,
                  const ActivationFunction activation = RELU);

void BiasActivate(const size_t batch_size, const size_t channels, float* data,
                  const float* biases,
                  const ActivationFunction activation = RELU);

float Activate(const float val, const ActivationFunction activation);

void Activate(const size_t len, const float* data, const float* bias,
              float* output, const ActivationFunction activation);

void Activate(const size_t len, float gamma, const float* data,
              const float* bias, float beta, float* out,
              const ActivationFunction activation);

}  // namespace lczero

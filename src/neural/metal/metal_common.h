/*
  This file is part of Leela Chess Zero.
  Copyright (C) 2021 The LCZero Authors

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
#pragma once

namespace lczero {
namespace metal_backend {

static int kNumOutputPolicy = 1858;
static int kInputPlanes = 112;

struct InputsOutputs {
  InputsOutputs(int maxBatchSize, bool wdl, bool moves_left) {
    input_masks_mem_ = (uint64_t*)malloc(maxBatchSize * kInputPlanes * sizeof(uint64_t));

    input_val_mem_ = (float*)malloc(maxBatchSize * kInputPlanes * sizeof(float));

    op_policy_mem_ = (float*)malloc(maxBatchSize * kNumOutputPolicy * sizeof(float));

    op_value_mem_ = (float*)malloc(maxBatchSize * (wdl ? 3 : 1) * sizeof(float));

    if (moves_left) {
      op_moves_left_mem_ = (float*)malloc(maxBatchSize * sizeof(float));
    } else
      op_moves_left_mem_ = nullptr;
  }
  ~InputsOutputs() {
    free(input_masks_mem_);
    free(input_val_mem_);
    free(op_policy_mem_);
    free(op_value_mem_);
    if (op_moves_left_mem_) {
      free(op_moves_left_mem_);
    }
  }
  uint64_t* input_masks_mem_;
  float* input_val_mem_;
  float* op_policy_mem_;
  float* op_value_mem_;
  float* op_moves_left_mem_;
};

}  // namespace metal_backend
}  // namespace lczero
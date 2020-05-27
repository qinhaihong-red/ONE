/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ResizeBilinear.h"
#include "Convert.h"

#include <cassert>
#include <vector>

flatbuffers::Offset<void> ResizeBilinearChef::value(flatbuffers::FlatBufferBuilder &fbb) const
{
  auto &operation = (*_operation);

  assert(operation.has_resize_bilinear_options());

  auto options = operation.resize_bilinear_options();

  tflite::ResizeBilinearOptionsBuilder options_builder{fbb};

  options_builder.add_align_corners(options.align_corners());
  options_builder.add_half_pixel_centers(options.half_pixel_centers());

  return options_builder.Finish().Union();
}

std::unique_ptr<OpChef> ResizeBilinearChefFactory::create(const tflchef::Operation *operation) const
{
  return std::unique_ptr<OpChef>{new ResizeBilinearChef{operation}};
}

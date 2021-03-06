/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <luci/IR/Nodes/CircleConst.h>

#include <loco.h>
#include <loco/IR/Graph.h>

#include <oops/UserExn.h>

#include <cassert>

namespace
{

template <loco::DataType T>
void copy_values(const luci::CircleConst *node, luci::CircleConst *cloned)
{
  assert(T == node->dtype());
  assert(T == cloned->dtype());

  const auto size = node->size<T>();
  cloned->size<T>(size);
  for (uint32_t i = 0; i < size; i++)
    cloned->at<T>(i) = node->at<T>(i);
}

} // namespace

namespace luci
{

luci::CircleConst *clone(luci::CircleConst *node)
{
  auto cloned = node->graph()->nodes()->create<luci::CircleConst>();

  // TODO: We don't have any naming policy for newly created nodes yet.
  //       Fix this when we have one.
  cloned->name(node->name());

  // dtype/shape
  cloned->dtype(node->dtype());
  cloned->rank(node->rank());
  for (uint32_t i = 0; i < node->rank(); i++)
    cloned->dim(i).set(node->dim(i).value());
  cloned->shape_status(luci::ShapeStatus::VALID);

  // values
  switch (node->dtype())
  {
    case loco::DataType::FLOAT32:
      copy_values<loco::DataType::FLOAT32>(node, cloned);
      break;

    case loco::DataType::U8:
      copy_values<loco::DataType::U8>(node, cloned);
      break;

    case loco::DataType::S8:
      copy_values<loco::DataType::S8>(node, cloned);
      break;

    case loco::DataType::S16:
      copy_values<loco::DataType::S16>(node, cloned);
      break;

    case loco::DataType::S32:
      copy_values<loco::DataType::S32>(node, cloned);
      break;

    case loco::DataType::S64:
      copy_values<loco::DataType::S64>(node, cloned);
      break;

    case loco::DataType::BOOL:
      copy_values<loco::DataType::BOOL>(node, cloned);
      break;

    default:
      throw oops::UserExn("Unsupported tensor dtype");
  }

  // quantparam
  const auto *quantparam = node->quantparam();
  if (quantparam != nullptr)
  {
    auto qparam = std::make_unique<luci::CircleQuantParam>();
    qparam->scale = quantparam->scale;
    qparam->zerop = quantparam->zerop;
    qparam->min = quantparam->min;
    qparam->max = quantparam->max;
    qparam->quantized_dimension = quantparam->quantized_dimension;

    cloned->quantparam(std::move(qparam));
  }

  // sparsity
  const auto *sparsity = node->sparsityparam();
  if (sparsity != nullptr)
  {
    auto sparam = std::make_unique<luci::SparsityParam>();
    sparam->traversal_order = sparsity->traversal_order;
    sparam->block_map = sparsity->block_map;
    sparam->dim_metadata = sparsity->dim_metadata;

    cloned->sparsityparam(std::move(sparam));
  }

  // op version
  cloned->op_version(node->op_version());

  return cloned;
}

} // namespace luci

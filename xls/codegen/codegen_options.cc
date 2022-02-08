// Copyright 2021 The XLS Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "xls/codegen/codegen_options.h"

#include "xls/common/proto_adaptor_utils.h"

namespace xls::verilog {

CodegenOptions& CodegenOptions::entry(absl::string_view name) {
  entry_ = name;
  return *this;
}

CodegenOptions& CodegenOptions::module_name(absl::string_view name) {
  module_name_ = name;
  return *this;
}

CodegenOptions& CodegenOptions::reset(absl::string_view name, bool asynchronous,
                                      bool active_low, bool reset_data_path) {
  reset_proto_ = ResetProto();
  reset_proto_->set_name(ToProtoString(name));
  reset_proto_->set_asynchronous(asynchronous);
  reset_proto_->set_active_low(active_low);
  reset_proto_->set_reset_data_path(reset_data_path);
  return *this;
}

CodegenOptions& CodegenOptions::manual_control(absl::string_view input_name) {
  if (!pipeline_control_.has_value()) {
    pipeline_control_ = PipelineControl();
  }
  pipeline_control_->mutable_manual()->set_input_name(
      ToProtoString(input_name));
  return *this;
}

absl::optional<ManualPipelineControl> CodegenOptions::manual_control() const {
  if (!pipeline_control_.has_value() || !pipeline_control_->has_manual()) {
    return absl::nullopt;
  }
  return pipeline_control_->manual();
}

CodegenOptions& CodegenOptions::valid_control(
    absl::string_view input_name,
    absl::optional<absl::string_view> output_name) {
  if (!pipeline_control_.has_value()) {
    pipeline_control_ = PipelineControl();
  }
  ValidProto* valid = pipeline_control_->mutable_valid();
  valid->set_input_name(ToProtoString(input_name));
  if (output_name.has_value()) {
    valid->set_output_name(ToProtoString(*output_name));
  }
  return *this;
}

absl::optional<ValidProto> CodegenOptions::valid_control() const {
  if (!pipeline_control_.has_value() || !pipeline_control_->has_valid()) {
    return absl::nullopt;
  }
  return pipeline_control_->valid();
}

CodegenOptions& CodegenOptions::clock_name(absl::string_view clock_name) {
  clock_name_ = std::move(clock_name);
  return *this;
}

CodegenOptions& CodegenOptions::use_system_verilog(bool value) {
  use_system_verilog_ = value;
  return *this;
}

CodegenOptions& CodegenOptions::flop_inputs(bool value) {
  flop_inputs_ = value;
  return *this;
}

CodegenOptions& CodegenOptions::flop_outputs(bool value) {
  flop_outputs_ = value;
  return *this;
}

CodegenOptions& CodegenOptions::flop_outputs_kind(IOKind value) {
  flop_outputs_kind_ = value;
  return *this;
}

CodegenOptions& CodegenOptions::split_outputs(bool value) {
  split_outputs_ = value;
  return *this;
}

CodegenOptions& CodegenOptions::add_idle_output(bool value) {
  add_idle_output_ = value;
  return *this;
}

CodegenOptions& CodegenOptions::assert_format(absl::string_view value) {
  assert_format_ = std::string{value};
  return *this;
}

CodegenOptions& CodegenOptions::gate_format(absl::string_view value) {
  gate_format_ = std::string{value};
  return *this;
}

CodegenOptions& CodegenOptions::emit_as_pipeline(bool value) {
  emit_as_pipeline_ = value;
  return *this;
}

CodegenOptions& CodegenOptions::streaming_channel_data_suffix(
    absl::string_view value) {
  streaming_channel_data_suffix_ = value;
  return *this;
}

CodegenOptions& CodegenOptions::streaming_channel_valid_suffix(
    absl::string_view value) {
  streaming_channel_valid_suffix_ = value;
  return *this;
}

CodegenOptions& CodegenOptions::streaming_channel_ready_suffix(
    absl::string_view value) {
  streaming_channel_ready_suffix_ = value;
  return *this;
}

}  // namespace xls::verilog

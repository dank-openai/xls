// Copyright 2020 The XLS Authors
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

#include "xls/visualization/ir_viz/ir_to_json.h"

#include <memory>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "pybind11/pybind11.h"
#include "xls/common/python/absl_casters.h"
#include "xls/common/status/statusor_pybind_caster.h"
#include "xls/delay_model/delay_estimator.h"
#include "xls/delay_model/delay_estimators.h"
#include "xls/ir/function.h"
#include "xls/ir/ir_parser.h"

namespace py = pybind11;

namespace xls {

namespace {

// Returns the particular function/proc to view in the visualizer.
absl::StatusOr<FunctionBase*> GetFunctionBaseToView(Package* package) {
  if (package->procs().size() == 1) {
    return package->procs().front().get();
  }
  // TODO(meheff): Support more than one proc.
  if (package->procs().size() > 1) {
    return absl::UnimplementedError(
        "Visualizing more than one proc not supported.");
  }
  return package->EntryFunction();
}

}  // namespace

// IR to JSON conversion function which takes strings rather than objects.
absl::StatusOr<std::string> IrToJsonWrapper(
    absl::string_view ir_text, absl::string_view delay_model_name,
    absl::optional<int64_t> pipeline_stages,
    absl::optional<absl::string_view> entry_name) {
  XLS_ASSIGN_OR_RETURN(std::unique_ptr<Package> package,
                       Parser::ParsePackage(ir_text));
  FunctionBase* func_base;
  if (entry_name.has_value()) {
    XLS_ASSIGN_OR_RETURN(func_base, package->GetFunction(entry_name.value()));
  } else {
    XLS_ASSIGN_OR_RETURN(func_base, GetFunctionBaseToView(package.get()));
  }
  XLS_ASSIGN_OR_RETURN(DelayEstimator * delay_estimator,
                       GetDelayEstimator(delay_model_name));
  if (pipeline_stages.has_value()) {
    // TODO(meheff): Support scheduled procs.
    XLS_RET_CHECK(func_base->IsFunction());
    XLS_ASSIGN_OR_RETURN(
        PipelineSchedule schedule,
        PipelineSchedule::Run(
            func_base->AsFunctionOrDie(), *delay_estimator,
            SchedulingOptions().pipeline_stages(pipeline_stages.value())));
    return IrToJson(package.get(), *delay_estimator, &schedule,
                    func_base->name());
  } else {
    return IrToJson(package.get(), *delay_estimator, /*schedule=*/nullptr,
                    func_base->name());
  }
}

PYBIND11_MODULE(ir_to_json, m) {
  ImportStatusModule();

  m.def("ir_to_json", &IrToJsonWrapper, py::arg("ir_text"),
        py::arg("delay_model_name"), py::arg("pipeline_stages") = absl::nullopt,
        py::arg("entry") = absl::nullopt);
}

}  // namespace xls

/*
 * Copyright 2019 The TensorFlow Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "main_functions.hpp"

#include "accelerometer_handler.hpp"
#include "constants.hpp"
#include "magic_wand_model_data.hpp"
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <zephyr/kernel.h>

int timerCycleCount = 0;
namespace {
	tflite::ErrorReporter *error_reporter = nullptr;
	const tflite::Model *model = nullptr;
	tflite::MicroInterpreter *interpreter = nullptr;
	TfLiteTensor *model_input = nullptr;
	int input_length;
	constexpr int kTensorArenaSize = 60 * 1024;
	uint8_t tensor_arena[kTensorArenaSize];
} /* namespace */

void worker(struct k_work *work)
{
	ReadAccelerometer(error_reporter, model_input->data.f, input_length);
}

K_WORK_DEFINE(my_work, worker);

void my_timer_handler(struct k_timer *dummy)
{
  timerCycleCount++;
  k_work_submit(&my_work);
}

K_TIMER_DEFINE(my_timer, my_timer_handler, NULL);

void setup(void)
{
	static tflite::MicroErrorReporter micro_error_reporter; /* NOLINT */

	error_reporter = &micro_error_reporter;

	model = tflite::GetModel(train_model_tflite);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		TF_LITE_REPORT_ERROR(error_reporter,
				     "Model provided is schema version %d not equal "
				     "to supported version %d.",
				     model->version(), TFLITE_SCHEMA_VERSION);
		return;
	}

	static tflite::MicroMutableOpResolver < 6 > micro_op_resolver; /* NOLINT */
	micro_op_resolver.AddConv2D();
	micro_op_resolver.AddDepthwiseConv2D();
	micro_op_resolver.AddFullyConnected();
	micro_op_resolver.AddMaxPool2D();
	micro_op_resolver.AddSoftmax();
  micro_op_resolver.AddReshape();

	/* Build an interpreter to run the model with. */
	static tflite::MicroInterpreter static_interpreter(
		model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
	interpreter = &static_interpreter;

	/* Allocate memory from the tensor_arena for the model's tensors. */
	interpreter->AllocateTensors();

	/* Obtain pointer to the model's input tensor. */
	model_input = interpreter->input(0);
	if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
	    (model_input->dims->data[1] != 30) ||
	    (model_input->dims->data[2] != kChannelNumber) ||
	    (model_input->type != kTfLiteFloat32)) {
    while(1)
		TF_LITE_REPORT_ERROR(error_reporter,
				     "Bad input tensor parameters in model");

		return;
	}

	input_length = model_input->bytes / sizeof(float);

	TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
	if (setup_status != kTfLiteOk) {
		TF_LITE_REPORT_ERROR(error_reporter, "Set up failed\n");
	}
  k_timer_start(&my_timer, K_MSEC(40), K_MSEC(40));
}


void loop(void)
{
	/* Attempt to read new data from the accelerometer. */
	/* Run inference, and report any error */
  if(timerCycleCount%10 == 0){
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed on index: %d\n",
               begin_index);
      return;
    }
    int gesture_index=3;
    float max_pred = -1;
    for (int i=0; i<2; i++) {
      if (interpreter->output(0)->data.f[i] > max_pred) {
        max_pred = interpreter->output(0)->data.f[i];
        gesture_index = i;
      }
    }

    printf("gesture_index: %d @ %f\n", gesture_index, max_pred);
  }
  return;
}

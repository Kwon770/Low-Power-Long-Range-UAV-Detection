// This header files are used to TensorFlow model and drone detection.
#include <TensorFlowLite.h>
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "drone_detect_model_data.h"

// This header file is used to caputre image.
#include "esp32_image.h"


namespace {

// It is used to predict drone detection.
// Interpreter calculate all operation for drone detection.
tflite::MicroInterpreter* interpreter = nullptr;

// It has our tensorflow lite mode data
const tflite::Model* model = nullptr;

// It is used to handle error of tensorflow model
tflite::ErrorReporter* error_reporter = nullptr;

// It points input data.
TfLiteTensor* input = nullptr;

// An area of memory to use for input, output, and intermediate arrays. 
// (it is used by interpeter obejct)
// tensor_arena is used by interpreter.
constexpr int kTensorArenaSize = 60 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

// This is stroed reszied image data.
static int output_image[784] = {0};

// frame_buffer is stored image data buffer by operation of take_photo() function.
camera_fb_t *frame_buffer = nullptr;

}

void init_camera() {
  // Init
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.pixel_format = PIXFORMAT_GRAYSCALE; // PIXFORMAT_ + RGB565 | YUV422 | GRAYSCALE | JPEG

  // check psram is available.
  //  If PSRAM is available, use more high quality image.
  if (psramFound()) {
    config.frame_size = FRAMESIZE_96X96; // FRAMESIZE_ + QVGA(320X240)|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10; //10-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_96X96;
    config.jpeg_quality = 12;
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // Init camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
  }
}

// Take picture with camera & save camera frame buffer(fb= image) to PSRAM(fb)
// Taking picutre function is library of "esp_camera.h"
camera_fb_t *take_photo() {


  // fb->width
  // fb->height
  // fb->format
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return (NULL);
  }

  // print current image shape
  Serial.printf("\n\nWidth = %u, Height=%u, Len=%u\n", fb->width, fb->height, fb->len);

  return (fb);
}

// downsample_image is resize input image to specific image size
void downsample_image(uint8_t *input_image, int input_width,
                      int input_height, int output_width, int output_height,
                      int *output_image) {
  // calculate the ratio of the input and output sizes
  double width_ratio = (float)input_width / output_width;
  double height_ratio = (float)input_height / output_height;
  Serial.println("DOWN SAMPLING");

  // iterate over the output image pixels
  for (int y = 0; y < output_height; y++) {
    for (int x = 0; x < output_width; x++) {
      // calculate the corresponding pixel position in the input image
      int input_x = (int)(x * width_ratio);
      int input_y = (int)(y * height_ratio);

      // calculate the average pixel value in the input image region
      int sum = 0;
      for (int j = 0; j < height_ratio; j++) {
        for (int i = 0; i < width_ratio; i++) {
          int index = (input_y + j) * input_width + (input_x + i);
          sum += input_image[index];
        }
      }
      int avg = sum / (width_ratio * height_ratio);

      // store the averaged pixel value in the output image
      int output_index = y * output_width + x;
      
      // calculate the average pixel value in the input image region
      output_image[output_index] = (int)avg;
    }
  }
}

void setup() {
  // Set upload speed to ESP32-WROVER environment
  Serial.begin(115200);

  Serial.println("START OBJECT DETECTION");
  
  // micro_error_reporter is used to repoprt Tensorflow error.
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  Serial.println("Map the model into a usable data structure");
  
  // Initialize C style tensorflow lite model to GetModel object.
  model = tflite::GetModel(custom_model_48x48_gray_tflite);

  Serial.println("AllOpsResolver: pulls in all the operation implementations we need.");

  // AllOpsResolver computes operations of model.
  static tflite::ops::micro::AllOpsResolver resolver;

  Serial.println("Build an interpreter to run the model with.");
  
  // Build an interpreter to run the model with.
  // We can predict drone dtection by using interpreter.
  static tflite::MicroInterpreter static_interpreter(
    model, resolver, tensor_arena, kTensorArenaSize, error_reporter);

  Serial.println("ASSIGN STATIC_INTERPRETER to interpreter");
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    Serial.println("AllocateTensors() failed");
    return;
  }

  // Get information about the memory area to use for the model's input.
  input = interpreter->input(0);
  init_camera();
  Serial.println("setup ok.");
}

void loop() {
  // Set upload speed to ESP32-WROVER environment
  Serial.begin(115200);

  frame_buffer = take_photo();
  Serial.println("END TAKE PHOTO");

  memset(output_image, 0.0f, 784 * sizeof(float));
  Serial.println("END RESET PHOTO");


  // ESP32-WROVER supports limited image resolution.
  // So to downsize image sizes, we have to resize image by self.
  // Downsize image to specific size. (96x96 to 48x48
  downsample_image(frame_buffer->buf, 96, 96, 48, 48, output_image);

  esp_camera_fb_return(frame_buffer);

  Serial.println("SET image array");

  // Set resized data to input of model
  for (int i = 0; i < 48 * 48; i++) {
    input->data.f[i] = (float)output_image[i];
  }

  Serial.println("\nINVOKE START");
  
  // Run the model using input data.
  interpreter->Invoke();


  Serial.println("INVOKE END");

  // We can access predicted value by using output pointer.
  TfLiteTensor* output = interpreter->output(0);


  // Inference result is coordinate of bounding box.
  int8_t width = output->data.f[2] - output->data.f[0];
  int8_t height = output->data.f[3] - output->data.f[1];

  // If model can not detect any dorne.
  // height or width is negative value
  if (width < 0 || height < 0 || !width || !height || (height == 0 && width ==0))
    Serial.print("inference result = [NO]");
  else
    Serial.print("inference result = [YES]");
  delay(5000);
}

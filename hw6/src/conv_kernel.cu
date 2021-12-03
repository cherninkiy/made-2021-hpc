#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

const int NUM_THREADS = 16;


__global__ void conv2d(int rows, int cols, int channels, const float *image,
    int kernel_size, const float* kernel, float norm, float *buffer)
{
    int col = threadIdx.x + blockIdx.x * blockDim.x;
    int row = threadIdx.y + blockIdx.y * blockDim.y;
	int c = threadIdx.z;
    int b = kernel_size / 2;

    double pix = 0;
    if (row > b && row < (rows - b) && col > b && col < (cols - b))
    {
        for (int k = -b; k <= b; k++)
        {
            for (int l = -b; l <= b; l++)
            {
                pix += 1/norm * kernel[((k + 1) * kernel_size + l) % kernel_size] 
                        * image[(row + l) * cols * channels + (col + k) * channels + c];
            }
        }
    }
    else
    {
        pix = image[row * cols * channels + col * channels + c];
    }
    pix = fminf(fmaxf(pix, 0.0f), 1.0f);
    buffer[row * cols * channels + col * channels + c] = pix;
}

cudaError_t applyConv2d(int rows, int cols, int channels, const uchar* image,
    int kernel_size, const float* kernel, float norm, float* buffer)
{
    int num_pixels = rows * cols * channels;
    int num_bytes = num_pixels * sizeof(float);
    int kernel_bytes = kernel_size * sizeof(float);

    cudaError_t error;

    float* img_array;
    error = cudaMalloc(&img_array, num_bytes);
    if (error != cudaError::cudaSuccess)
    {
        return error;
    }

    float* buf_array;
    error = cudaMalloc(&buf_array, num_bytes);
    if (error != cudaError::cudaSuccess)
    {
        cudaFree(img_array);
        return error;
    }

    float* kernel_array;
    error = cudaMalloc(&kernel_array, kernel_bytes);
    if (error != cudaError::cudaSuccess)
    {
        cudaFree(img_array);
        cudaFree(buf_array);
        return error;
    }

    error = cudaMemcpy(img_array, image, num_bytes, cudaMemcpyHostToDevice);
    if (error != cudaError::cudaSuccess)
    {
        cudaFree(img_array);
        cudaFree(buf_array);
        cudaFree(kernel_array);
        return error;
    }

    error = cudaMemcpy(kernel_array, kernel, kernel_bytes, cudaMemcpyHostToDevice);
    if (error != cudaError::cudaSuccess)
    {
        cudaFree(img_array);
        cudaFree(buf_array);
        cudaFree(kernel_array);
        return error;
    }

    int grid_width = (cols + NUM_THREADS - 1) / NUM_THREADS;
    int grid_height = (rows + NUM_THREADS - 1) / NUM_THREADS;
    const dim3 grid_size(grid_width, grid_height);
    const dim3 block_size(NUM_THREADS, NUM_THREADS, channels);

    conv2d<<<grid_size, block_size>>>(rows, cols, channels, img_array,
            kernel_size, kernel_array, norm, buf_array);

    error = cudaPeekAtLastError();
    if (error != cudaError::cudaSuccess)
    {
        cudaFree(img_array);
        cudaFree(buf_array);
        cudaFree(kernel_array);
        return error;
    }

    error = cudaMemcpy(buffer, buf_array, num_bytes, cudaMemcpyDeviceToHost);

    cudaFree(img_array);
    cudaFree(buf_array);
    cudaFree(kernel_array);
    return error;
}

int main(int argc, char* argv[])
{
    Mat image = imread("images/lena.jpg");
    image.convertTo(image, CV_32F, 1.0 / 255);

    // Blur 5x5
    {
        vector<float> kernel = {
                1.0, 1.0, 1.0, 1.0, 1.0,
                1.0, 1.0, 1.0, 1.0, 1.0,
                1.0, 1.0, 1.0, 1.0, 1.0,
                1.0, 1.0, 1.0, 1.0, 1.0,
                1.0, 1.0, 1.0, 1.0, 1.0
            };

        float norm = kernel.size() * kernel.size();

        vector<float> buffer(image.rows * image.cols * image.channels(), 0.0f);

        auto error = applyConv2d(image.rows, image.cols, image.channels(), &image.data[0],
                            kernel.size(), kernel.data(), norm, buffer.data());
        if (error != cudaError::cudaSuccess)
        {
            cout << cudaGetErrorString(error) << endl;
            return 1;
        }

        Mat result(image.rows, image.cols, CV_32FC3, buffer.data());
        result.convertTo(result, CV_8U, 255);

        if (!imwrite("images/lena_blur.jpg", result))
        {
            cout << "Cannot save image" << endl;
        }

        cout << "Output: images/lena_blur.jpg" << endl;
    }

    // Conv 3x3
    {
        vector<float> kernel = {
                0.5f, 0.5f, 0.5f,
                1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, 0.5f
            };

        float norm = 0.0;
        for (int i = 0; i < kernel.size(); ++i) {
            norm += kernel[i];
        }

        vector<float> buffer(image.rows * image.cols * image.channels(), 0.0f);

        auto error = applyConv2d(image.rows, image.cols, image.channels(), &image.data[0],
                            kernel.size(), kernel.data(), norm, buffer.data());
        if (error != cudaError::cudaSuccess)
        {
            cout << cudaGetErrorString(error) << endl;
            return 1;
        }

        Mat result(image.rows, image.cols, CV_32FC3, buffer.data());
        result.convertTo(result, CV_8U, 255);

        if (!imwrite("images/lena_conv.jpg", result))
        {
            cout << "Cannot save image" << endl;
        }

        cout << "Output: images/lena_conv.jpg" << endl;
    }

    return 0;
}

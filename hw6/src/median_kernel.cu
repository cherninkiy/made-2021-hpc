#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

const int NUM_THREADS = 16;
const int KERNEL_SIZE = 5;

__global__ void median2d(int rows, int cols, int channels, const float *image, float *buffer) {

    int col = threadIdx.x + blockIdx.x * blockDim.x;
    int row = threadIdx.y + blockIdx.y * blockDim.y;
	int c = threadIdx.z;
    int b = KERNEL_SIZE / 2;
    const int n = KERNEL_SIZE * KERNEL_SIZE;

    double pix, tmp;
    int counter = 0;
    double arr[n] = {0};
    if (row > b && row < (rows - b) && col > b && col < (cols - b))
    {
        for (int k = -b; k <= b; k++)
        {
            for (int l = -b; l <= b; l++)
            {
                arr[counter] = image[(row + l) * cols * channels + (col + k) * channels + c];
                counter++;
            }
        }
        // sort
        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                if (arr[i] > arr[j])
                {
                    tmp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = tmp;
                }
            }
        }
        if (n % 2)
            pix = arr[b];
        else
            pix = (arr[b] + arr[b+1])/2;
    }
    else
    {
        pix = image[row * cols * channels + col * channels + c];
    }
    buffer[row * cols * channels + col * channels + c] = pix;
}

cudaError_t applyMedian2d(int rows, int cols, int channels, const uchar* image, float* buffer)
{
    int num_pixels = rows * cols * channels;
    int num_bytes = num_pixels * sizeof(float);

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

    error = cudaMemcpy(img_array, image, num_bytes, cudaMemcpyHostToDevice);
    if (error != cudaError::cudaSuccess)
    {
        cudaFree(img_array);
        cudaFree(buf_array);
        return error;
    }

    int grid_width = (cols + NUM_THREADS - 1) / NUM_THREADS;
    int grid_height = (rows + NUM_THREADS - 1) / NUM_THREADS;
    const dim3 grid_size(grid_width, grid_height);
    const dim3 block_size(NUM_THREADS, NUM_THREADS, channels);

    median2d<<<grid_size, block_size>>>(rows, cols, channels, img_array, buf_array);

    error = cudaPeekAtLastError();
    if (error != cudaError::cudaSuccess)
    {
        cudaFree(img_array);
        cudaFree(buf_array);
        return error;
    }

    error = cudaMemcpy(buffer, buf_array, num_bytes, cudaMemcpyDeviceToHost);

    cudaFree(img_array);
    cudaFree(buf_array);
    return error;
}

int main(int argc, char* argv[])
{
    Mat image = imread("images/lena.jpg");
    image.convertTo(image, CV_32F, 1.0 / 255);

    vector<float> buffer(image.rows * image.cols * image.channels(), 0.0f);

    auto error = applyMedian2d(image.rows, image.cols, image.channels(), &image.data[0], buffer.data());
    if (error != cudaError::cudaSuccess)
    {
        cout << cudaGetErrorString(error) << endl;
        return 1;
    }

    Mat result(image.rows, image.cols, CV_32FC3, buffer.data());
    result.convertTo(result, CV_8U, 255);

    if (!imwrite("images/lena_median.jpg", result))
    {
        cout << "Cannot save image" << endl;
    }

    cout << "Output: images/lena_median.jpg" << endl;

    return 0;
}

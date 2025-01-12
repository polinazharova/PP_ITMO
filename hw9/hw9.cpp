#include <iostream>
#include <cuda_runtime.h>


__global__ void countThreads(bool* conditionArray, int* res, int* size) {
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < size && conditionArray[idx]) {
		atomicAdd(res, 1);
	}
}

__global__ void dot(int* a, int* b, int* c) {
	__shared__ int temp[N];
	temp[threadIdx.x] = a[threadIdx.x] * b[threadIdx.x];
	__syncthreads();

	if (threadIdx.x == 0) {
		int sum = 0;
		for (int i = 0; i < N; i++)
			sum += temp[i];
		*c = sum;
	}
}

#define N 512

int main(void) {
	int* a, * b, * c;
	int* dev_a, * dev_b, * dev_c;
	int size = N * sizeof(int);

	cudaMalloc((void**)&dev_a, size);
	cudaMalloc((void**)&dev_b, size);
	cudaMalloc((void**)&dev_c, sizeof(int));

	a = new int(size);
	b = new int(size);
	c = new int;

	random_ints(a, N);
	random_ints(b, N);

	cudaMemcpy(dev_a, a, size, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_b, b, size, cudaMemcpyHostToDevice);


	dot << < 1, N >> > (dev_a, dev_b, dev_c);

	cudaMemcpy(c, dev_c, sizeof(int), cudaMemcpyDeviceToHost);

	free(a); free(b); free(c);
	cudaFree(dev_a);
	cudaFree(dev_b);
	cudaFree(dev_c);

	return 0;

}


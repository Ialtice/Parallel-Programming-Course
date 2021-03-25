#include <stdio.h>
#include <stdlib.h>
// __global__ keyword specifies a device kernel function
__global__ void cuda_hello() {
    printf("Hello World from GPU!\n");
    printf("hello form GPU B.x=%d, Thread.x=%d\n", blockIdx.x, threadIdx.x);
}

int main() {
    printf("Hello World from CPU!\n");

    // Call a device function from the host: a kernel launch Which will print from the device
    cuda_hello<<<6,1>>>(); 
	//cuda_hello<<<1,6>>>();  comment and uncomment lines 13-14 for lab step 7 and 8
    // This call waits for all of the submitted GPU work to complete
    cudaDeviceSynchronize();
    return 0; 
}


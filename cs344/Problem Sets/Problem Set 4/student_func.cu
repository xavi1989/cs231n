//Udacity HW 4
//Radix Sorting

#include "utils.h"
#include <thrust/host_vector.h>

/* Red Eye Removal
   ===============
   
   For this assignment we are implementing red eye removal.  This is
   accomplished by first creating a score for every pixel that tells us how
   likely it is to be a red eye pixel.  We have already done this for you - you
   are receiving the scores and need to sort them in ascending order so that we
   know which pixels to alter to remove the red eye.

   Note: ascending order == smallest to largest

   Each score is associated with a position, when you sort the scores, you must
   also move the positions accordingly.

   Implementing Parallel Radix Sort with CUDA
   ==========================================

   The basic idea is to construct a histogram on each pass of how many of each
   "digit" there are.   Then we scan this histogram so that we know where to put
   the output of each digit.  For example, the first 1 must come after all the
   0s so we have to know how many 0s there are to be able to start moving 1s
   into the correct position.

   1) Histogram of the number of occurrences of each digit
   2) Exclusive Prefix Sum of Histogram
   3) Determine relative offset of each digit
        For example [0 0 1 1 0 0 1]
                ->  [0 1 0 1 2 3 2]
   4) Combine the results of steps 2 & 3 to determine the final
      output location for each element and move it there

   LSB Radix sort is an out-of-place sort and you will need to ping-pong values
   between the input and output buffers we have provided.  Make sure the final
   sorted results end up in the output buffer!  Hint: You may need to do a copy
   at the end.

 */

__global__ 
void scan_kernel(unsigned *d_in, unsigned int *d_out, const size_t size) {
    int mid = threadIdx.x + blockDim.x * blockIdx.x;
    if(mid >= size)
        return;
    d_out[mid] = d_in[mid];
    __syncthreads();

    for(int s = 1; s <= size; s *= 2) {
          int spot = mid - s; 
         
          unsigned int val = 0;
          if(spot >= 0)
              val = d_out[spot];
          __syncthreads();
          if(spot >= 0)
              d_out[mid] += val;
          __syncthreads();

    }

    d_out[mid] = d_out[mid] - d_in[mid];
}

__global__
void exclusive_scan_kernel(unsigned *d_in, unsigned int *d_out, const size_t size) {
  // 1d of blockDim
  int index = threadIdx.x + blockDim.x * blockIdx.x;
  if(index >= size)
    return;

  d_out[index] = 1; //d_in[index];
  __syncthreads();

  size_t s = 0;
  for(s=1; s<size; s<<=1) {
    if((index + 1)%(2*s) == 0) {
      if(index - s >= 0) {
        d_out[index] = d_out[index] + d_out[index - s];
      }
    }

    __syncthreads();
  }

  // downsweep
  if(index == size-1) {
    d_out[index] = 0;
  }

  __syncthreads();

  for(s = s/2; s>0; s>>=1) {
    if((index + 1) % (2*s) == 0) {
      if(index - s >=0) {
        int tmp = d_out[index - s];
        d_out[index - s] = d_out[index];
        d_out[index] = d_out[index] + tmp;
      }
    }

    __syncthreads();
  }
}

__global__
void predicate_kernel(unsigned int *d_in,
                      unsigned int *d_out,
                      const size_t size, unsigned int mask, int *count) {
    int id = threadIdx.x + blockDim.x * blockIdx.x;

    if(id >= size)
        return;

    unsigned int bit = d_in[id] & mask;

    if(count) { // get 0
        d_out[id] = bit == 0 ? 1 : 0;
        if(bit == 0)
          atomicAdd(count, 1);
    } else {
        d_out[id] = bit == 1 ? 1 : 0;
    }
}

__global__
void scatter_kernel(unsigned int* const d_inputVals,
               unsigned int* const d_inputPos,
               unsigned int* const d_outputVals,
               unsigned int* const d_outputPos,
               const size_t size,
               unsigned int *scan_in,
               unsigned int *offset_in, int* offset) {
    int id = threadIdx.x + blockDim.x * blockIdx.x;

    if(id >= size)
        return;

    if(scan_in[id] == 1) {
        int addr = offset_in[id] + offset == NULL?0:(*offset);
        d_outputVals[addr] = d_inputVals[id];
        d_outputPos[addr] = d_inputPos[id];
    }
}

#include <stdio.h>

// gives you a good max size for n/d
int get_max_size(int n, int d) {
    return (int)ceil( (float)n/(float)d ) + 1;
}

void your_sort(unsigned int* const d_inputVals,
               unsigned int* const d_inputPos,
               unsigned int* const d_outputVals,
               unsigned int* const d_outputPos,
               const size_t numElems)
{ 
  //TODO
  //PUT YOUR SORT HERE
  unsigned int *in = (unsigned int *)calloc(1, numElems * sizeof(unsigned int));
  unsigned int *offset = (unsigned int *)calloc(1, numElems * sizeof(unsigned int));

  checkCudaErrors(cudaMemcpy(in, d_inputVals,  sizeof(unsigned int) * numElems, cudaMemcpyDeviceToHost));

  printf("numElems %d \n", (int)numElems);
  for(int i=0; i<50; i++) {
    //printf("%d \n", in[i]);
  }

  dim3 blockdim(1024);
  dim3 griddim(numElems / 1024 + 1);

  unsigned int *d_in, *d_inPos, *d_out, *d_outPos, *d_scan, *d_offset;
  checkCudaErrors(cudaMalloc(&d_in,     sizeof(int) * numElems));
  checkCudaErrors(cudaMalloc(&d_inPos,  sizeof(int) * numElems));

  checkCudaErrors(cudaMalloc(&d_out,    sizeof(int) * numElems));
  checkCudaErrors(cudaMemset(d_out, 0,  sizeof(int) * numElems));

  checkCudaErrors(cudaMalloc(&d_outPos,   sizeof(int) * numElems));
  checkCudaErrors(cudaMemset(d_outPos, 0, sizeof(int) * numElems));

  checkCudaErrors(cudaMalloc(&d_scan,   sizeof(int) * numElems));
  checkCudaErrors(cudaMemset(d_scan, 0, sizeof(int) * numElems));

  checkCudaErrors(cudaMalloc(&d_offset, sizeof(int) * numElems));
  checkCudaErrors(cudaMemset(d_offset, 0, sizeof(int) * numElems));

  checkCudaErrors(cudaMemcpy(d_in,    d_inputVals, sizeof(int) * numElems, cudaMemcpyDeviceToDevice));
  checkCudaErrors(cudaMemcpy(d_inPos, d_inputPos,  sizeof(int) * numElems, cudaMemcpyDeviceToDevice));

  int *d_count;
  int count;
  checkCudaErrors(cudaMalloc(&d_count, sizeof(int)));

  unsigned int mask = 0x01;
  unsigned int *tmp;

  for(int j=0; j<32; j++) {
    // predicative kernel
    printf("j is %d 111111111111\n", j);
    mask <<= j;
    // bit 0
    count = 0;
    checkCudaErrors(cudaMemcpy(d_count, &count,    sizeof(int), cudaMemcpyHostToDevice));
    checkCudaErrors(cudaGetLastError());
  checkCudaErrors(cudaMemcpy(in, d_in,  sizeof(int) * numElems, cudaMemcpyDeviceToHost));

  printf("numElems %d \n", (int)numElems);
  for(int i=0; i<10; i++) {
    //printf("%d \n", in[i]);
  }

    predicate_kernel<<<griddim, blockdim>>>(d_in, d_scan, numElems, mask , d_count);
    cudaDeviceSynchronize();

  checkCudaErrors(cudaMemcpy(offset, d_scan,  sizeof(unsigned int) * numElems, cudaMemcpyDeviceToHost));

  printf("0 1 array \n");
  for(int i=0; i<10; i++) {
    printf("%d \n", offset[i]);
  }
    checkCudaErrors(cudaGetLastError());


    for(int i = 0; i < get_max_size(numElems, blockdim.x); i++) {
        exclusive_scan_kernel<<<dim3(1), blockdim>>>(d_scan, d_offset, numElems);
        cudaDeviceSynchronize(); checkCudaErrors(cudaGetLastError());
    
    }



    cudaDeviceSynchronize();
    checkCudaErrors(cudaGetLastError());

  checkCudaErrors(cudaMemcpy(offset, d_offset,  sizeof(unsigned int) * numElems, cudaMemcpyDeviceToHost));

    printf("offset \n");
  for(int i=0; i<10; i++) {
    printf("%d \n", offset[i]);
  }
    checkCudaErrors(cudaMemcpy(&count, d_count,    sizeof(int), cudaMemcpyDeviceToHost));
    printf("count is %d \n", count);

    scatter_kernel<<<griddim, blockdim>>>(d_in, d_inPos,
                                          d_out, d_outPos,
                                          numElems,
                                          d_scan,
                                          d_offset, NULL);

    cudaDeviceSynchronize();
    checkCudaErrors(cudaGetLastError());
    // bit 1
    predicate_kernel<<<griddim, blockdim>>>(d_in, d_scan, numElems, mask , NULL);
    cudaDeviceSynchronize();
    checkCudaErrors(cudaGetLastError());

    for(int i = 0; i < get_max_size(numElems, blockdim.x); i++) {
        exclusive_scan_kernel<<<dim3(1), blockdim>>>(d_scan, d_offset, numElems);
        cudaDeviceSynchronize(); checkCudaErrors(cudaGetLastError());
    
    }

    cudaDeviceSynchronize();
    checkCudaErrors(cudaGetLastError());

    scatter_kernel<<<griddim, blockdim>>>(d_in, d_inPos,
                                          d_out, d_outPos,
                                          numElems,
                                          d_scan,
                                          d_offset, d_count);
    cudaDeviceSynchronize();
    checkCudaErrors(cudaGetLastError());

    tmp = d_in;
    d_in = d_out;
    d_out = tmp;

    tmp = d_inPos;
    d_inPos = d_outPos;
    d_outPos = tmp;
  }


  cudaMemcpy(d_outputVals, d_out,    sizeof(unsigned int)*numElems, cudaMemcpyDeviceToDevice);
  cudaMemcpy(d_outputPos,  d_outPos, sizeof(unsigned int)*numElems, cudaMemcpyDeviceToDevice);

  if(d_in) {
    checkCudaErrors(cudaFree(d_in));
  }

  if(d_out) {
    checkCudaErrors(cudaFree(d_out));
  }

  if(d_inPos) {
    checkCudaErrors(cudaFree(d_inPos));
  }

  if(d_outPos) {
    checkCudaErrors(cudaFree(d_outPos));
  }

  if(d_scan) {
    checkCudaErrors(cudaFree(d_scan));
  }

  if(d_offset) {
    checkCudaErrors(cudaFree(d_offset));
  }

  if(d_count) {
    checkCudaErrors(cudaFree(d_count));
  }

}

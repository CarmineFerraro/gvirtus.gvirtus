#include <cuda_runtime_api.h>
#include "CudaUtil.h"
#include "CudaRtHandler.h"

CUDA_ROUTINE_HANDLER(EventCreate) {
    cudaEvent_t *event = input_buffer->Assign<cudaEvent_t>();

    cudaError_t exit_code = cudaEventCreate(event);

    Buffer *out = new Buffer();
    out->Add(event);

    return new Result(exit_code, out);
}

#if 0 
// FIXME: this should be conditioned on cuda version
CUDA_ROUTINE_HANDLER(EventCreateWithFlags) {
    cudaEvent_t *event = input_buffer->Assign<cudaEvent_t>();
    int flags = input_buffer->Get<int>();

    cudaError_t exit_code = cudaEventCreateWithFlags(event, flags);

    Buffer *out = new Buffer();
    out->Add(event);

    return new Result(exit_code, out);
}
#endif

CUDA_ROUTINE_HANDLER(EventDestroy) {
    cudaEvent_t event = input_buffer->Get<cudaEvent_t>();

    return new Result(cudaEventDestroy(event));
}

CUDA_ROUTINE_HANDLER(EventElapsedTime) {
    float *ms = input_buffer->Assign<float>();
    cudaEvent_t start = input_buffer->Get<cudaEvent_t>();
    cudaEvent_t end = input_buffer->Get<cudaEvent_t>();

    cudaError_t exit_code = cudaEventElapsedTime(ms, start, end);

    Buffer *out = new Buffer();
    out->Add(ms);

    return new Result(exit_code, out);
}

CUDA_ROUTINE_HANDLER(EventQuery) {
    cudaEvent_t event = input_buffer->Get<cudaEvent_t>();

    return new Result(cudaEventQuery(event));
}

CUDA_ROUTINE_HANDLER(EventRecord) {
    cudaEvent_t event = input_buffer->Get<cudaEvent_t>();
    cudaStream_t stream = input_buffer->Get<cudaStream_t>();

    return new Result(cudaEventRecord(event, stream));
}

CUDA_ROUTINE_HANDLER(EventSynchronize) {
    cudaEvent_t event = input_buffer->Get<cudaEvent_t>();

    return new Result(cudaEventSynchronize(event));
}

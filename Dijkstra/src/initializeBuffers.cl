__kernel void initializeBuffers(__global int *maskArray, __global float *costArray,__global float* updatingCostArray,int sourceVertex,int vertexCount) {
	int tid=get_global_id(0);
	if(tid<vertexCount){
	if(tid==sourceVertex) {
		maskArray[tid]=1;
		costArray[tid]=0.0;
		updatingCostArray[tid]=0.0;
	} else {
		maskArray[tid]=0;
		costArray[tid]=FLT_MAX;
		updatingCostArray[tid]=FLT_MAX;
	}
	}
}
__kernel void DijkstraKernel2(__global int *maskArray,__global float *costArray,__global float *updatingCostArray,int vertexCount) {
	int tid=get_global_id(0);
	if(tid<vertexCount){
	if(updatingCostArray[tid]<costArray[tid]) {
		costArray[tid]=updatingCostArray[tid];
		maskArray[tid]=1;
	} else if(updatingCostArray[tid]>costArray[tid]){
		updatingCostArray[tid]=costArray[tid];
	}
	}
}

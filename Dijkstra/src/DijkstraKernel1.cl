__kernel void DijkstraKernel1(__global int *vertexArray,__global int *edgeArray,__global int *maskArray,__global float *weightArray,__global float *costArray,__global float *updatingCostArray,int vertexCount,int edgeCount) {
	int tid=get_global_id(0);
	if(tid<vertexCount){
	if(maskArray[tid]!=0) {
		maskArray[tid]=0;
		int edgeStart=vertexArray[tid];
		int edgeEnd;
		if(tid+1<vertexCount) {
			edgeEnd=vertexArray[tid+1];
		} else {
			edgeEnd=vertexCount;
		}
		for(int edge=edgeStart;edge<edgeEnd;edge++) {
			int nid=edgeArray[edge];
			if(updatingCostArray[nid]>(costArray[tid]+weightArray[edge])){
				updatingCostArray[nid]=(costArray[tid]+weightArray[edge]);
			}
		}
	}
	}
}

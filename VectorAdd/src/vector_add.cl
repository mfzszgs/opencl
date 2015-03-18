__kernel void vector_add(const int size, __global float *A, __global float *B, __global float*C)
{
	int gid0 = get_global_id(0);
	int gid1 = get_global_id(1);
	int gid2 = get_global_id(2);
	int gsize0=get_global_size(0);
	int gsize1=get_global_size(1);
	int gsize2=get_global_size(2);
	int tmp=gid2*gsize1*gsize0+gid1*gsize0+gid0;
	if(tmp<size) C[tmp] = A[tmp]+B[tmp];
}
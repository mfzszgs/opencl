__kernel void matrix_mul(const int Mdim, const int Ndim, const int Pdim, __global float *A, __global float *B, __global float*C) 
{
	int k;
	int i = get_global_id(0);
	int j = get_global_id(1);
	float tmp;
	if ((i<Ndim) && (j<Mdim))
	{
		tmp = 0.0;
		for (k = 0; k<Pdim; k++)
			tmp += A[i*Pdim + k] * B[k*Mdim + j];
		C[i*Mdim + j] = tmp;
	}
}

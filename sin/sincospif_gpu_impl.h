#ifndef SINCOSPIF_GPU_IMPL_H
#define SINCOSPIF_GPU_IMPL_H

void sincospif_gpu_calculate_taylor(float *angles, float *sin_results, float *cos_results, int num_angles);
extern double sin_taylor_series(double x);
extern double cos_taylor_series(double x);
double normalize_angle(double angle);

#endif // 

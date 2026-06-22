#ifndef __COLORS_H__
#define __COLORS_H__

#ifdef __cplusplus
extern "C" {
#endif

void RGBtoHSV(float r, float g, float b, float *h, float *s, float *v);
void HSVtoRGB(float *r, float *g, float *b, float h, float s, float v);

#ifdef __cplusplus
}
#endif

#endif

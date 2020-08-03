#version 460

layout(local_size_x = 16, local_size_y = 16) in;
layout(binding = 0, rgba8)uniform mediump image2D inputImage;
layout(binding = 1, rgba8)uniform mediump image2D outImage;

void main()
{
	float u = float(gl_GlobalInvocationID.x); //因为是并行计算，这样可以取出对应点的颜色
	float v = float(gl_GlobalInvocationID.y);
	vec4 inputColor = imageLoad(inputImage, ivec2(u, v));
	imageStore(outImage, ivec2(u, v), vec4(1.0, 1.0, 1.0, 1.0) - inputColor);
}
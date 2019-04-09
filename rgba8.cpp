#include <string.h>
#include "rgba8.h"

void Convert_RGBA8(uint32_t* buf, A3D_ImageFormat f, int w, int h, const void* data, int palsize, const void* palbuf)
{
	int bits = 0;
	int bytes = 0;

	switch (f)
	{
		case A3D_RGB8:
		{
			const uint8_t* src = (const uint8_t*)data;
			int n = w * h;
			for (int i = 0; i < n; i++)
				buf[i] = src[3 * i + 0] | (src[3 * i + 1] << 8) | (src[3 * i + 2] << 16) | 0xFF000000;
			break;
		}
		case A3D_RGB16:
		{
			const uint16_t* src = (const uint16_t*)data;
			int n = w * h;
			for (int i = 0; i < n; i++)
				buf[i] = (src[3 * i + 0] >> 8) | ((src[3 * i + 1] >> 8) << 8) | ((src[3 * i + 2] >> 8) << 16) | 0xFF000000;
			break;
		}
		case A3D_RGBA8:
			memcpy(buf, data, w*h * sizeof(uint32_t));
			break;
		case A3D_RGBA16:
		{
			const uint16_t* src = (const uint16_t*)data;
			int n = w * h;
			for (int i = 0; i < n; i++)
				buf[i] = (src[4 * i + 0] >> 8) | ((src[4 * i + 1] >> 8) << 8) | ((src[4 * i + 2] >> 8) << 16) | ((src[4 * i + 3] >> 8) << 24);
			break;
		}
		case A3D_LUMINANCE1:
			break;
		case A3D_LUMINANCE2:
			break;
		case A3D_LUMINANCE4:
			break;
		case A3D_LUMINANCE8:
		{
			const uint8_t* src = (const uint8_t*)data;
			int n = w * h;
			for (int i = 0; i < n; i++)
				buf[i] = src[i] | (src[i] << 8) | (src[i] << 16) | 0xFF000000;
			break;
		}
		case A3D_LUMINANCE_ALPHA1:
			break;
		case A3D_LUMINANCE_ALPHA2:
			break;
		case A3D_LUMINANCE_ALPHA4:
			break;
		case A3D_LUMINANCE_ALPHA8:
		{
			const uint8_t* src = (const uint8_t*)data;
			int n = w * h;
			for (int i = 0; i < n; i++)
				buf[i] = src[2 * i + 0] | (src[2 * i + 0] << 8) | (src[2 * i + 0] << 16) | (src[2 * i + 1] << 24);
			break;
		}

		case A3D_INDEX1_RGB:
		case A3D_INDEX1_RGBA:
			break;

		case A3D_INDEX2_RGB:
		case A3D_INDEX2_RGBA:
			break;

		case A3D_INDEX4_RGB:
		case A3D_INDEX4_RGBA:
			break;

		case A3D_INDEX8_RGB:
		case A3D_INDEX8_RGBA:
		{
			const uint8_t* src = (const uint8_t*)data;
			int n = w * h;
			for (int i = 0; i < n; i++)
			{
				if (src[i] >= palsize)
					buf[i] = 0;
				else
					buf[i] = ((uint32_t*)palbuf)[src[i]];
			}
			break;
		}
	}

#if 0
	if (bits)
	{
		int out_row = sizeof(uint32_t) * w;
		int in_row = (bits * w + 7) >> 3;
		const uint8_t* in_line = (const uint8_t*)data;
		uint8_t* out_line = (uint8_t*)buf;

		int d = 1;
		int q = 8 / bits - 1;
		int m = (1 << bits) - 1;
		int r = 0;
		int mul = 1;
		switch (bits)
		{
			case 1: r = 3; mul = 255; break;
			case 2: r = 2; mul = 85;  break;
			case 4: r = 1; mul = 17;  break;
		}

		if (bytes < 3)
		{
			// unpack, normalize
			for (int y = 0; y < h; y++)
			{
				for (int b = 0; b < out_row; b++)
				{
					int val = (in_line[b >> r] >> ((b&q)*bits)) & m;
					out_line[b] = val * mul;
				}

				in_line += in_row;
				out_line += out_row;
			}
		}
		else
		{
			if (bytes == 4) // depal rgba
				for (int y = 0; y < h; y++)
				{
					for (int x = 0; x < w; x++)
					{
						int idx = (in_line[x >> r] >> ((x&q)*bits)) & m;
						if (idx >= palsize)
							((uint32_t*)out_line)[x] = 0x00000000; // black-transp if outside pal
						else
							((uint32_t*)out_line)[x] = ((const uint32_t*)palbuf)[idx];
					}

					in_line += in_row;
					out_line += out_row;
				}
			else // depal rgb
				for (int y = 0; y < h; y++)
				{
					for (int x = 0; x < w; x++)
					{
						int idx = (in_line[x >> r] >> ((x&q)*bits)) & m;

						if (idx >= palsize)
						{
							out_line[3 * x + 0] = 0;
							out_line[3 * x + 1] = 0;
							out_line[3 * x + 2] = 0;
						}
						else
						{
							out_line[3 * x + 0] = ((const uint8_t*)palbuf)[4 * idx + 0];
							out_line[3 * x + 1] = ((const uint8_t*)palbuf)[4 * idx + 1];
							out_line[3 * x + 2] = ((const uint8_t*)palbuf)[4 * idx + 2];
						}
					}

					in_line += in_row;
					out_line += out_row;
				}
		}
	}
#endif
}

void ConvertLuminanceToAlpha_RGBA8(uint32_t* buf, const uint8_t rgb[3], A3D_ImageFormat f, int w, int h, const void* data, int palsize, const void* palbuf)
{
	uint32_t const_rgb = 0;

	if (rgb)
		const_rgb = rgb[0] | (rgb[1] << 8) | (rgb[2] << 16);

	switch (f)
	{
		case A3D_RGB8:
		{
			const uint8_t* src = (const uint8_t*)data;
			for (int i = 0; i < w*h; i++)
				buf[i] = (((src[3 * i + 0] * 2 + src[3 * i + 1] * 3 + src[3 * i + 2] + 3) / 6) << 24) | const_rgb;
			break;
		}

		case A3D_RGB16:
		{
			const uint16_t* src = (const uint16_t*)data;
			for (int i = 0; i < w*h; i++)
				buf[i] = (((src[3 * i + 0] * 2 + src[3 * i + 1] * 3 + src[3 * i + 2] + 3 * 257) / (6 * 257)) << 24) | const_rgb;
			break;
		}

		case A3D_RGBA8:
		{
			const uint8_t* src = (const uint8_t*)data;
			for (int i = 0; i < w*h; i++)
				buf[i] = (((src[4 * i + 0] * 2 + src[4 * i + 1] * 3 + src[4 * i + 2] + 3) / 6) << 24) | const_rgb;
			break;
		}

		case A3D_RGBA16:
		{
			const uint16_t* src = (const uint16_t*)data;
			for (int i = 0; i < w*h; i++)
				buf[i] = (((src[4 * i + 0] * 2 + src[4 * i + 1] * 3 + src[4 * i + 2] + 3 * 257) / (6 * 257)) << 24) | const_rgb;
			break;
		}

		case A3D_LUMINANCE1:
			break;
		case A3D_LUMINANCE2:
			break;
		case A3D_LUMINANCE4:
			break;
		case A3D_LUMINANCE8:
		{
			const uint8_t* src = (const uint8_t*)data;
			for (int i = 0; i < w*h; i++)
				buf[i] = (src[i] << 24) | const_rgb;
			break;
		}
		case A3D_LUMINANCE_ALPHA1:
			break;
		case A3D_LUMINANCE_ALPHA2:
			break;
		case A3D_LUMINANCE_ALPHA4:
			break;
		case A3D_LUMINANCE_ALPHA8:
		{
			const uint8_t* src = (const uint8_t*)data;
			for (int i = 0; i < w*h; i++)
				buf[i] = (src[2 * i + 0] << 24) | const_rgb;
			break;
		}

		case A3D_INDEX1_RGB:
			break;
		case A3D_INDEX2_RGB:
			break;
		case A3D_INDEX4_RGB:
			break;
		case A3D_INDEX8_RGB:
		{
			const uint8_t* src = (const uint8_t*)data;
			for (int i = 0; i < w*h; i++)
			{
				int q = src[i];
				if (q >= palsize)
					buf[i] = 0xFFFFFF;
				else
				{
					const uint8_t* p = (const uint8_t*)palbuf + 4 * q;
					buf[i] = (((p[0] * 2 + p[1] * 3 + p[2] + 3) / 6) << 24) | const_rgb;
				}
			}
			break;
		}

		case A3D_INDEX1_RGBA:
			break;
		case A3D_INDEX2_RGBA:
			break;
		case A3D_INDEX4_RGBA:
			break;
		case A3D_INDEX8_RGBA:
			const uint8_t* src = (const uint8_t*)data;
			for (int i = 0; i < w*h; i++)
			{
				int q = src[i];
				if (q >= palsize)
					buf[i] = 0xFFFFFF;
				else
				{
					const uint8_t* p = (const uint8_t*)palbuf + 4 * q;
					buf[i] = (((p[0] * 2 + p[1] * 3 + p[2] + 3) / 6) << 24) | const_rgb;
				}
			}
			break;
	}
}

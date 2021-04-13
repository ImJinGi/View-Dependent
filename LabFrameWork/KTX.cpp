#pragma once

#include "KTX.h"

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif /* _MSC_VER */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "GL/glew.h"


namespace ktx
{

	namespace file
	{

		static const unsigned char identifier[] =
		{
			0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
			//0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
		};

		static const unsigned int swap32(const unsigned int u32)
		{
			union
			{
				unsigned int u32;
				unsigned char u8[4];
			} a, b;

			a.u32 = u32;
			b.u8[0] = a.u8[3];
			b.u8[1] = a.u8[2];
			b.u8[2] = a.u8[1];
			b.u8[3] = a.u8[0];

			return b.u32;
		}

		static const unsigned short swap16(const unsigned short u16)
		{
			union
			{
				unsigned short u16;
				unsigned char u8[2];
			} a, b;

			a.u16 = u16;
			b.u8[0] = a.u8[1];
			b.u8[1] = a.u8[0];

			return b.u16;
		}

		static unsigned int calculate_stride(const header& h, unsigned int width, unsigned int pad = 4)
		{
			unsigned int channels = 0;
			switch (h.glbaseinternalformat)
			{
			case GL_RED:    channels = 1;
				break;
			case GL_RG:     channels = 2;
				break;
			case GL_BGR:
				break;
			case GL_RGB:    channels = 3;
				break;
			case GL_BGRA:
				break;
			case GL_RGBA:   channels = 4;
				break;
			}

			unsigned int stride = h.gltypesize * channels * width;

			stride = (stride + (pad - 1)) & ~(pad - 1);

			return stride;
		}

		static unsigned int calculate_face_size(const header& h)
		{
			unsigned int stride = calculate_stride(h, h.pixelwidth);

			return stride * h.pixelheight;
		}

		extern
			unsigned int load(const char * filename, unsigned int tex)
		{
			FILE * fp;
			GLuint temp = 0;
			GLuint retval = 0;
			header h;
			size_t data_start, data_end;
			unsigned char * data;
			GLenum target = GL_NONE;

			fp = fopen(filename, "rb");

			if (!fp)
			{
				//std::cout << filename << " file does not exist" << std::endl;
				return 0;
			}
			else
			{
				//std::cout << filename << " load successful" << std::endl;
			}

			if (fread(&h, sizeof(h), 1, fp) != 1)
				goto fail_read;

			if (memcmp(h.identifier, identifier, sizeof(identifier)) != 0)
				goto fail_header;

			if (h.endianness == 0x04030201)
			{
				// No swap needed
			}

			else if (h.endianness == 0x01020304)
			{
				// Swap needed
				h.endianness = swap32(h.endianness);
				h.gltype = swap32(h.gltype);
				h.gltypesize = swap32(h.gltypesize);
				h.glformat = swap32(h.glformat);
				h.glinternalformat = swap32(h.glinternalformat);
				h.glbaseinternalformat = swap32(h.glbaseinternalformat);
				h.pixelwidth = swap32(h.pixelwidth);
				h.pixelheight = swap32(h.pixelheight);
				h.pixeldepth = swap32(h.pixeldepth);
				h.arrayelements = swap32(h.arrayelements);
				h.faces = swap32(h.faces);
				h.miplevels = swap32(h.miplevels);
				h.keypairbytes = swap32(h.keypairbytes);

			}
			else
			{
				goto fail_header;
			}

			// Guess target (texture type)
			if (h.pixelheight == 0)
			{
				//std::cout << "h.pixelheight == 0" << std::endl;
				if (h.arrayelements == 0)
				{
					//std::cout << "h.arrayelements == 0" << std::endl;
					//std::cout << "GL_TEXTURE_1D" << std::endl;
					target = GL_TEXTURE_1D;
				}
				else
				{
					//std::cout << "else" << std::endl;
					//std::cout << "GL_TEXTURE_1D_ARRAY" << std::endl;
					target = GL_TEXTURE_1D_ARRAY;
				}
			}
			else if (h.pixeldepth == 0)
			{
				//std::cout << "h.pixeldepth == 0" << std::endl;
				if (h.arrayelements == 0)
				{
					//std::cout << "h.arrayelements == 0" << std::endl;
					if (h.faces == 0)
					{
						//std::cout << "h.faces == 0" << std::endl;
						//std::cout << "GL_TEXTURE_2D" << std::endl;
						target = GL_TEXTURE_2D;
					}
					else
					{
						//std::cout << "else" << std::endl;
						//std::cout << "GL_TEXTURE_CUBE_MAP" << std::endl;
						target = GL_TEXTURE_CUBE_MAP;
					}
				}
				else
				{
					//std::cout << "else" << std::endl;
					if (h.faces == 0)
					{
						//std::cout << "h.faces == 0" << std::endl;
						//std::cout << "GL_TEXTURE_2D_ARRAY" << std::endl;
						target = GL_TEXTURE_2D_ARRAY;
					}
					else
					{
						//std::cout << "else" << std::endl;
						//std::cout << "GL_TEXTURE_CUBE_MAP_ARRAY" << std::endl;
						target = GL_TEXTURE_2D_ARRAY;
					}
				}
			}
			else
			{
				//std::cout << "else" << std::endl;
				//std::cout << "GL_TEXTURE_3D" << std::endl;
				target = GL_TEXTURE_3D;
			}

			// Check for insanity...
			if (target == GL_NONE ||                                    // Couldn't figure out target
				(h.pixelwidth == 0) ||                                  // Texture has no width???
				(h.pixelheight == 0 && h.pixeldepth != 0))              // Texture has depth but no height???
			{
				goto fail_header;
			}

			temp = tex;
			if (tex == 0)
			{
				glGenTextures(1, &tex);
			}

			glBindTexture(target, tex);

			data_start = ftell(fp) + h.keypairbytes;
			fseek(fp, 0, SEEK_END);
			data_end = ftell(fp);
			fseek(fp, data_start, SEEK_SET);

			data = new unsigned char[data_end - data_start];
			memset(data, 0, data_end - data_start);

			fread(data, 1, data_end - data_start, fp);

			if (h.miplevels == 0)
			{
				h.miplevels = 1;
			}
			unsigned int channels = 0;
			switch (h.glbaseinternalformat)
			{
			case GL_RED:    channels = 1;
				break;
			case GL_RG:     channels = 2;
				break;
			case GL_BGR:
				break;
			case GL_RGB:    channels = 3;
				break;
			case GL_BGRA:
				break;
			case GL_RGBA:   channels = 4;
				break;
			}

			switch (target)
			{
			case GL_TEXTURE_1D:
				glTexStorage1D(GL_TEXTURE_1D, h.miplevels, h.glinternalformat, h.pixelwidth);
				glTexSubImage1D(GL_TEXTURE_1D, 0, 0, h.pixelwidth, h.glformat, h.glinternalformat, data);
				//std::cout << "GL_TEXTURE_1D" << std::endl;
				break;
			case GL_TEXTURE_2D:
				// glTexImage2D(GL_TEXTURE_2D, 0, h.glinternalformat, h.pixelwidth, h.pixelheight, 0, h.glformat, h.gltype, data);
				if (h.gltype == GL_NONE)
				{
					//std::cout << "GL_TEXTURE_2D GL_NONE" << std::endl;
					glCompressedTexImage2D(GL_TEXTURE_2D, 0, h.glinternalformat, h.pixelwidth, h.pixelheight, 0, 420 * 380 / 2, data);
				}
				else
				{
					//std::cout << "GL_TEXTURE_2D" << std::endl;
					glTexStorage2D(GL_TEXTURE_2D, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight);
					{
						unsigned char * ptr = data;
						unsigned int height = h.pixelheight;
						unsigned int width = h.pixelwidth;
						glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
						for (unsigned int i = 0; i < h.miplevels; i++)
						{
							glTexSubImage2D(GL_TEXTURE_2D, i, 0, 0, width, height, h.glformat, h.gltype, ptr);
							ptr += height * calculate_stride(h, width, 1);
							height >>= 1;
							width >>= 1;
							if (!height)
								height = 1;
							if (!width)
								width = 1;
						}
					}
				}
				break;
			case GL_TEXTURE_3D:
				//std::cout << "GL_TEXTURE_3D" << std::endl;
				glTexStorage3D(GL_TEXTURE_3D, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight, h.pixeldepth);
				glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.pixeldepth, h.glformat, h.gltype, data);
				break;
			case GL_TEXTURE_1D_ARRAY:
				//std::cout << "GL_TEXTURE_1D_ARRAY" << std::endl;
				glTexStorage2D(GL_TEXTURE_1D_ARRAY, h.miplevels, h.glinternalformat, h.pixelwidth, h.arrayelements);
				glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0, 0, 0, h.pixelwidth, h.arrayelements, h.glformat, h.gltype, data);
				break;
			case GL_TEXTURE_2D_ARRAY:
				//std::cout << "GL_TEXTURE_2D_ARRAY" << std::endl;
				glTexStorage3D(GL_TEXTURE_2D_ARRAY, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight, h.arrayelements);
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.arrayelements, h.glformat, h.gltype, data);
				break;
			case GL_TEXTURE_CUBE_MAP:
				//std::cout << "GL_TEXTURE_CUBE_MAP" << std::endl;
				glTexStorage2D(GL_TEXTURE_CUBE_MAP, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight);
				// glTexSubImage3D(GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.faces, h.glformat, h.gltype, data);
				{
					unsigned int face_size = calculate_face_size(h);
					for (unsigned int i = 0; i < h.faces; i++)
					{
						glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, h.pixelwidth, h.pixelheight, h.glformat, h.gltype, data + face_size * i);
					}
				}
				break;
			case GL_TEXTURE_CUBE_MAP_ARRAY:
				glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight, h.arrayelements);
				glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.faces * h.arrayelements, h.glformat, h.gltype, data);
				break;
			default:                                               // Should never happen
				goto fail_target;
			}

			if (h.miplevels == 1)
			{
				glGenerateMipmap(target);
			}

			retval = tex;
			/*std::cout << std::endl;
			std::cout << "h.endianness: " << h.endianness << std::endl;
			std::cout << "h.gltype: " << h.gltype << std::endl;
			std::cout << "h.gltypesize: " << h.gltypesize << std::endl;
			std::cout << "h.glformat: " << h.glformat << std::endl;
			std::cout << "h.glinternalformat: " << h.glinternalformat << std::endl;
			std::cout << "h.glbaseinternalformat: " << h.glbaseinternalformat << std::endl;
			std::cout << "h.pixelwidth: " << h.pixelwidth << std::endl;
			std::cout << "h.pixelheight: " << h.pixelheight << std::endl;
			std::cout << "h.pixeldepth: " << h.pixeldepth << std::endl;
			std::cout << "h.arrayelements: " << h.arrayelements << std::endl;
			std::cout << "h.faces: " << h.faces << std::endl;
			std::cout << "h.miplevels:  " << h.miplevels << std::endl;
			std::cout << "h.keypairbytes: " << h.keypairbytes << std::endl;
			std::cout << std::endl;*/
		fail_target:
			//std::cout << "fail_target" << std::endl;
			delete[] data;

		fail_header:
			//std::cout << "fail_header" << std::endl;
		fail_read:
			//std::cout << "fail_read" << std::endl;
			fclose(fp);

			return retval;
		}

		bool save(const char * filename, unsigned int target, unsigned int tex)
		{
			header h;

			memset(&h, 0, sizeof(h));
			memcpy(h.identifier, identifier, sizeof(identifier));
			h.endianness = 0x04030201;

			glBindTexture(target, tex);

			glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, (GLint *)&h.pixelwidth);
			glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, (GLint *)&h.pixelheight);
			glGetTexLevelParameteriv(target, 0, GL_TEXTURE_DEPTH, (GLint *)&h.pixeldepth);
			//std::cout << "save successfully " << std::endl;
			return true;
		}

	}

}


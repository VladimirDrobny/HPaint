#pragma once

#include <fstream>
#include <cstring>

namespace files
{
	inline unsigned char* int_to_bytes_little(int n, int size)
	{
		if (size < 1)
		{
			return nullptr;
		}
		unsigned char* ret = (unsigned char*)malloc(size);
		memset(ret, 0, size);
		int index = 0;
		while (index < size)
		{
			ret[index] = n & 0xFF;
			n >>= 8;
			if (n == 0)
			{
				break;
			}
			index++;
		}

		return ret;
	}

	inline int bytes_to_int_little(unsigned char* bytes, int size)
	{
		int ret = 0;
		int scale = 1;
		int index = 0;
		while(index < size)
		{
			ret += scale * (int)bytes[index];
			scale <<= 8;
			index++;
		}
		return ret;
	}

	struct Himage_file {
		int file_size;
		unsigned char* file_data;
	};

	inline void save_himage(Himage_file* saving_file, std::string path)
	{
		std::fstream file (path, std::ios :: out | std::ios :: binary);
		file.write(reinterpret_cast<const char*>(saving_file->file_data), saving_file->file_size);
		file.close();
        free(saving_file->file_data);
        delete saving_file;
	}
}

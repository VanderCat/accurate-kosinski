#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kosinski_compress.h"
#include "kosinski_decompress.h"

bool LoadFileToBuffer(char *file_name, unsigned char **file_buffer, long int *file_size)
{
	bool success = false;

	FILE *file = fopen(file_name, "rb");

	if (file)
	{
		fseek(file, 0, SEEK_END);
		*file_size = ftell(file);
		rewind(file);

		*file_buffer = malloc(*file_size);

		if (*file_buffer)
		{
			fread(*file_buffer, *file_size, 1, file);

			success = true;
		}

		fclose(file);
	}

	return success;
}

int main(int argc, char *argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		FILE *in_file = fopen(argv[i], "rb");
		FILE *out_file = fopen("out.unc", "wb");

		KosinskiDecompress(in_file, out_file);

		fclose(in_file);
		fclose(out_file);

		unsigned char *file_buffer;
		long int file_size;

		if (!LoadFileToBuffer("out.unc", &file_buffer, &file_size))
			break;
		printf("File '%s' with size %lX loaded\n", argv[i], file_size);
		FILE *dst_file = fopen("out.kos", "wb");
		KosinskiCompress(file_buffer, file_size, dst_file);
		fclose(dst_file);

		unsigned char *file_buffer1, *file_buffer2;
		long int file_size1, file_size2;

		if (!LoadFileToBuffer(argv[i], &file_buffer1, &file_size1))
			break;
		if (!LoadFileToBuffer("out.kos", &file_buffer2, &file_size2))
			break;

		if (file_size1 != file_size2)
			printf("File sizes don't match!\n");

		if (memcmp(file_buffer1, file_buffer2, (file_size1 > file_size2) ? file_size2 : file_size1))
			printf("The files don't match!\n\n");
		else
			printf("Yay the files match.\n\n");
	}

	getchar();

	remove("out.kos");
	remove("out.unc");;
}

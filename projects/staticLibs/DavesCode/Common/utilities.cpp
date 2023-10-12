#include "utilities.h"

namespace DC
{
	bool convertFileToHeader(const String& filename, const String& arrayName, unsigned int numberOfElementsPerRow)
	{
		FILE* fs = NULL;  // Source file
		FILE* fd = NULL;  // Destination file

		String strFilenameInput = filename;
		String strFilenameOutput = filename;
		strFilenameOutput.append(L".h");

		// Open source
		fopen_s(&fs, filename.c_strn(), "rb");
		if (fs == NULL)
			return false;

		// Open destination
		fopen_s(&fd, strFilenameOutput.c_strn(), "wb");
		if (fd == NULL)
		{
			fclose(fs);
			return false;
		}

		// Write comment giving name of binary file that this data came from
		fprintf(fd, "// Following data taken from file \"%s\"\n", filename.c_strn());

		// Get length of data
		fseek(fs, 0, SEEK_END);
		int length = ftell(fs);
		fseek(fs, 0, SEEK_SET);

		// Write #define for length of array
		String strArrayNameUppercase = arrayName;
		strArrayNameUppercase.uppercase();
		strArrayNameUppercase.append(L"_SIZE");
		fprintf(fd, "#define %s %d\n", strArrayNameUppercase.c_strn(), length);

		// Write start of array
		fprintf(fd, "unsigned char %s[] =\n", arrayName.c_strn());
		fprintf(fd, "{\n ");

		// Write out data from source binary file
		while (feof(fs) == false)
		{
			unsigned char tmp;
			for (int e = 0; e < (int)numberOfElementsPerRow; e++)
			{
				fread(&tmp, 1, sizeof(unsigned char), fs);
				if (feof(fs) == false)
					fprintf(fd, "0x%02x,", tmp);
			}

			if (feof(fs)) // Reached end of file
			{
				fseek(fd, -1, SEEK_CUR);  // Remove last comma
				fprintf(fd, "};");
			}
			fprintf(fd, "\n ");
		}
		fprintf(fd, "\n");

		fclose(fs);
		fclose(fd);
		return true;
	}
}
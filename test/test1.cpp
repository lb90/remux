#include <ebml/StdIOCallback.h>
#include <matroska/FileKax.h>

class FileContext {
public:
	FileContext(const char *path)
	 : iocb(path, MODE_READ),
	   filek(iocb)
	 { }
private:
	libebml::StdIOCallback     iocb;
	libmatroska::FileMatroska  filek;
	
	void printinfo();
};

void FileContext::printinfo() {
	printf("number of tracks: %d\n", filek.GetTrackNumber());
}

void read_file(char *filename) {
	FileContext filectx(filename);
}

int main(int argc, char **argv) {
	int retcode = 0;

	if (argc != 2) {
		printf("usage: %s filename.mkv\n", argv[0]);
		retcode = 1;
	}
	else {
		char *filename = argv[1];
		read_file(filename);
	}

	return retcode;
}


#include <cstdlib>
#include <climits>
#include <ebml/StdIOCallback.h>
#include <matroska/FileKax.h>
#include <matroska/KaxTracks.h>

class FileContext {
public:
	FileContext(const char *path)
	 : ebml_iocb(path, MODE_READ),
	 { }
private:
	libebml::StdIOCallback     ebml_iocb;
	libebml::EbmlStream        ebml_stream;
	
	void printinfo();
};

int FileContext::printinfo() {
	libebml::EbmlStream ebml_stream(ebml_iocb);
	
	libebml::EbmlElement *toplevel
	
	toplevel = ebml_stream.FindNextID(EbmlHead::ClassInfos, (uint64_t)-1);
	if (toplevel == NULL)
		return 1;
	
	
	
	printf("number of tracks: %d\n", number_of_tracks);
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


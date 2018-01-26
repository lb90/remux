#ifndef REMUX_MEDIACONVERT_H
#define REMUX_MEDIACONVERT_H

#include <thread>
#include <functional>



class mediaconvert {
private:
	mediaconvert();

public:
	static mediaconvert_t& inst() const;
	
	void convert();
	void convert(size_t i);

private:
	std::thread thr;
	std::function<void(int, int)> progresscallback;

	static mediaconvert_t *inst;
};

#endif


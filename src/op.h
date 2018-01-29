#ifndef REMUX_OP_H
#define REMUX_OP_H

#include <cstdlib>
#include <vector>

class op {
public:
	static void media_scan(int n);
	static void remove(std::vector<size_t>& indexv);
	static void enqueue(int n);
};

#endif

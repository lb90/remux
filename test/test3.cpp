#include <cstdlib>
#include <boost/property_tree/ptree.hpp>

int main() {
	boost::property_tre::ptree pt;
	
	pt.add("EBML-head", "a");
	pt.add("EBML-head.Document-type", "b");
	
	

	return 0;
}

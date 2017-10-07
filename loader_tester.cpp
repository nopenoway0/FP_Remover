#include "WrestlerLoader.h"
#include "Wrestler.h"
#include <vector>
#include <cstring>

#define NUM_CHUNKS 17

using fp_remover::loaders::WrestlerLoader;
using fp_remover::classes::Wrestler;
using namespace std;

// Removes section resulting in reallocation
// returns new size of buffer
unsigned long delete_section(char** p, unsigned long start, unsigned long length, unsigned long size);

int main(int argc, char* args[])
{
	string filename;
	if(argc > 1) filename = args[1];
	else filename = "savedata";


	cout << "loading from file " << filename << endl;

	vector<Wrestler> p;
	int wrestler_count_loc, size, wrestler_size;
	unsigned long skip_byte_loc;
	const long int *chunk_locations;
	const char* fp;
	int *ip; // used to modify integers in memory

	{
		WrestlerLoader wl(filename);
		wl.load();

		wrestler_count_loc = wl.get_record_loc();
		skip_byte_loc = wl.get_byte_skip_loc();
		p = wl.get_wrestlers();
		fp = wl.get_file_p();
		size = wl.get_filesize();
		chunk_locations = wl.get_chunk_locations();

		for(auto& w : p) cout << w.get_name() << endl;

		char* fp_m = new char[size];

		fp_m = (char*) memcpy(fp_m, fp, size);

		// remove wrestler at 0 index
		wrestler_size = p.at(0).get_fp_end() - p.at(0).get_fp_start();		

		ip = (int*) &fp_m[wrestler_count_loc];
		cout << "number of wrestlers " << *ip << endl;
		*ip -= 1; // substract 1 from wrestler count

		for( int x = 5; x < NUM_CHUNKS; x++)
		{
			ip = (int*) &fp_m[chunk_locations[x]];
			*ip -= wrestler_size;		
		}

		for(int x = 11; x < NUM_CHUNKS; x++)
		{
			ip = (int*) &fp_m[chunk_locations[x]];
			*ip -= 4;
		}
		if(skip_byte_loc < p.at(0).get_fp_start()) exit(-1);
		else if(skip_byte_loc >= size) exit(-1);

		size = delete_section(&fp_m, skip_byte_loc, 4, size);
		size = delete_section(&fp_m, p.at(0).get_fp_start(), wrestler_size, size);



		ofstream output("output", ios_base::binary);

		output.write(fp_m, size);

		output.close();

		fp = nullptr;
		ip = nullptr;
		delete [] fp_m;
	} // destroy uneeded loader
	return 0;
}

unsigned long delete_section(char** p, unsigned long start, unsigned long length, unsigned long size)
{	
	unsigned long new_size = size - length;
	if(new_size >= size) exit(-1);// throw error
	
	for(unsigned long x = 0; x < size - start + length; x++) (*p)[start + x] = (*p)[start + length + x];

	*p = (char*) realloc(*p, new_size);
	return new_size;
}
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
unsigned long delete_section(char*& p, unsigned long start, unsigned long length, unsigned long size);

unsigned long remove_wrestler(char*& fp_m, vector<Wrestler>& w, int index, int count, unsigned long skip_byte_loc, unsigned long size, const long int* chunk_locations);

int main(int argc, char* args[])
{
	string filename;
	string output_name;
	if(argc > 1) filename = args[1];
	else filename = "savedata";
	if(argc > 2) output_name = args[2];
	else output_name = "output";

	cout << "loading from file " << filename << endl;

	vector<Wrestler> p;
	int wrestler_count_loc, size;
	unsigned long skip_byte_loc;
	const long int *chunk_locations;
	const char* fp;

	{
		WrestlerLoader wl(filename);
		char* fp_m;
		ofstream ofile;
		for(int in = 1; in < 10; in++)
		{

			wl.load();
			cout << "load succesful" << endl; // testing
			wrestler_count_loc = wl.get_record_loc();
			skip_byte_loc = wl.get_byte_skip_loc();
			p = wl.get_wrestlers();
			fp = wl.get_file_p();
			size = wl.get_filesize();
			chunk_locations = wl.get_chunk_locations();

			//cout << "removing " << p.at(in).get_name() << endl; 

			fp_m = new char[size];

			fp_m = (char*) memcpy(fp_m, fp, size);

			cout << in << " - " << "CHUNK DATA:" << endl;
			for( int x = 0; x < NUM_CHUNKS; x++)
			{
				cout << *(int*) &fp_m[chunk_locations[x]] << endl;
			}
			
			//for(auto& x : p) cout << x.get_name() << endl;
			for( int x = 0; x < 5; x++) cout << p.at(x).get_name() << endl;
			// remove wrestler at in input from the user
			size = remove_wrestler(fp_m, p, 0, wrestler_count_loc, skip_byte_loc, size, chunk_locations);

			wl.write(fp_m, size);

			//ofile.open(output_name, ios_base::binary | ios_base::trunc);
			//ofile.write(fp_m, size);
			//ofile.close();
			fp = nullptr;
			delete [] fp_m;
			fp_m = nullptr;
			chunk_locations = nullptr;
			//p.clear();
			cout << endl;
		}

	} // destroy uneeded loader
	return 0;
}

unsigned long delete_section(char*& p, unsigned long start, unsigned long length, unsigned long size)
{	
	unsigned long new_size = size - length;
	if(new_size >= size || size <= length || (size - (start + length)) < 0) throw runtime_error("invalid delete_section call");

	for(unsigned long x = 0; x < size - (start + length); x++) p[start + x] = p[start + length + x];

	p = (char*) realloc(p, new_size);

	return new_size;
}

unsigned long remove_wrestler(char*& fp_m, vector<Wrestler>& w, int index, int count, unsigned long skip_byte_loc, unsigned long size, const long int* chunk_locations)
{
	unsigned long wrestler_size = w.at(index).get_fp_end() - w.at(index).get_fp_start();
	int *ip = nullptr;
	ip = (int*) &fp_m[count];
	*ip -= 1; // substract 1 from wrestler count

	cout << "number of wrestlers " << *ip << endl;
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

	if((unsigned int) skip_byte_loc < w.at(index).get_fp_start()) exit(-1);
	else if(skip_byte_loc >= size) exit(-1);

	cout << "removing wrestler " << w.at(index).get_name() << " with id " << *(int*) &fp_m[skip_byte_loc] << " at " << w.at(index).get_fp_start() << endl;


	size = delete_section(fp_m, skip_byte_loc, 4, size);

	size = delete_section(fp_m, w.at(index).get_fp_start(), wrestler_size, size);

	cout << "removed wrestler from memory" << endl;

	return size;
}
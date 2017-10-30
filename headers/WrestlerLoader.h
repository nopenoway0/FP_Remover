#ifndef _WRESTLER_LOADER_H
#define _WRESTLER_LOADER_H

#include "SaveLoader.h"
#include "Wrestler.h"

namespace fp_remover{ namespace loaders{

using frontend::loaders::SaveLoader;
using fp_remover::classes::Wrestler;
using namespace std;

class WrestlerLoader : public SaveLoader<Wrestler>
{
public:
	WrestlerLoader();
	WrestlerLoader(string name);
	~WrestlerLoader();
	void load() override;
	Wrestler parse() override;
	vector<Wrestler> get_wrestlers();
	int get_record_loc();
	unsigned long get_byte_skip_loc();
	const char* get_file_p();
	unsigned long get_filesize();
	const long int* get_chunk_locations();
	bool write(char* b_in, int size);
private:
	// inherit:
	// vector<T> contents
	// ifstream* f
	int getLength();
	string getString();
	char* file_memory;
	long int* chunk_location; // points to all chunks
	void clear_memory();
	int wrestler_record_loc; // location in buffer where total number of wrestlers is located
	unsigned long byte_skip_loc; // skip copying this byte, after 1 delete
	unsigned long filesize;
};

}}
#endif
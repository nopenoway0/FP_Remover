#include <fstream>
#include <iostream>
#include <stdint.h>
#include <cwchar>
#include <cstring>
#include <vector>
#include "Wrestler.h"
#include "Character.h"

#define FILE_NAME "savedata"

using namespace std;

typedef fp_remover::classes::FIELD_FLAGS wrestler_flags;

typedef fp_remover::classes::Wrestler Wrestler;


void printChar(char* buffer, int size);
int getLength(ifstream&);
string getString(ifstream&);

enum{
	VER_NUM,
	NUM_CHUNKS
};

enum
{
	F_NAME,
	L_NAME,
	NICKNAME,
};

typedef struct fp_group
{
	string first_name;
	string last_name;
	int id;
	int alignment;
} fp_group;

typedef struct organization
{
	string sname;
	string lname;
	int id;
} org;

struct saveDataChunk{
	int32_t chunkKind;
	int32_t version;
	int64_t top;
};

typedef struct optionSettings settings;
typedef struct saveDataChunk dataChunk;


int main(int argc, char* args[]){
	string filename;
	int number_wrestlers;
	if(argc  == 2)
	{
		filename = args[1];
	}
	else filename = FILE_NAME;
	ifstream file(filename, ios::in | ios::binary);
	file.seekg(0, ios_base::end);
	long filesize = file.tellg();
	long* chunk_location;
	file.seekg(ios_base::beg);

	char* file_memory = new char[filesize];

	file.read(file_memory, filesize);

	string read_string;
	char* buffer = new char[16];
	int num_chunks = -1;
	int wrestler_num_location;
	file.seekg(0L);
	vector<Wrestler> w_list;

	// read 2 ints, second is number of chunks
	file.read(buffer, 8);
	num_chunks = *(int32_t*) &buffer[4];

	saveDataChunk* list_chunks = new saveDataChunk[num_chunks];

	file.seekg(16L); // move to location to begin loading chunks
	
	chunk_location = new long[num_chunks]; // store chunk top location for changing after wrestler is removed
	for(int x = 0; x < num_chunks; x++)
	{

		file.read(buffer, 16);
		list_chunks[x].chunkKind = *(int32_t*) &buffer[0];
		list_chunks[x].version = *(int32_t*) &buffer[4];
		list_chunks[x].top = *(int64_t*) &buffer[8];

		chunk_location[x] = (long) file.tellg() - 8;
	}

	for(int x = 0; x < num_chunks; x++)
	{
		cout << x << ": " << list_chunks[x].chunkKind << " " << list_chunks[x].version << " " << list_chunks[x].top << endl;
	}

	// set to wreslter load start location
{ // start wrestling load
	file.seekg(list_chunks[4].top);

	wrestler_num_location = file.tellg();
	file.read(buffer, 4);
	number_wrestlers = *(int*) buffer;
	cout << "Number Wrestlers: " << number_wrestlers << endl;

	// does x amount of wreslers
	int counter = 0; // for printing
	Wrestler* wr;
	int* modifier = nullptr;
	for(int x = 0; x < number_wrestlers; x++)
	{
		wr = new Wrestler();
		wr->set_fp_start(file.tellg());
		counter = 0;
		for(int y = 0; y < 3; y++)
		{
			file.read(buffer, 4);
			counter++;
		}

		file.read(buffer, 1);
		counter++;

		for(int y = 0; y < 4; y++)
		{
			read_string = getString(file);
			switch(y)
			{
				case F_NAME:	wr->set_field(read_string, wrestler_flags::F_NAME);
								break;
				case L_NAME: 	wr->set_field(read_string, wrestler_flags::L_NAME);
								break;
				case NICKNAME:	wr->set_field(read_string, wrestler_flags::N_NAME);
			}
			counter++;

		}
		for(int y = 0; y < 27; y++)
		{
			file.read(buffer, 4);
			counter++;
			if(counter == 12) break; // break at birth year starts at this point
		}

		file.seekg(2 * 4, file.cur);

		file.read(buffer, 4);
		wr->set_field(*(int*) buffer, wrestler_flags::G_ID);

		file.seekg((324 * 4) + 1 + (91 * 4 * 3), file.cur); // skip unecessary values
		
		read_string = getString(file);
		file.seekg(17, file.cur); // skip 17 bytes
		read_string = getString(file);

		// costume data loading
		file.seekg(12, file.cur);

		for(int y = 0; y < 4; y++)
		{
			file.seekg(1, file.cur); // skip bool
			for(int z = 0; z < 144; z++) read_string = getString(file);
			file.seekg((144 * 4 * 4) + (149 * 4), file.cur); // skip 144 * 4 float then 149 float
		}

		wr->set_fp_end(file.tellg());
		w_list.push_back(*wr);
		delete wr;
	}

	ofstream o_file("output", ios_base::binary);

	// create writer class to write out everything except memory chunks
	cout << "file size: " << filesize << endl;

	// write to file, without first wrestler

	// set new number to wrestlers
	modifier = (int*) &file_memory[wrestler_num_location];
	*modifier = (number_wrestlers - 1);

	// change chunk tops
	for( int x = 5; x < 17; x++)
	{
		modifier = (int*) &file_memory[chunk_location[x]];
		*modifier -= (w_list[0].get_fp_end() - w_list[0].get_fp_start());		
	}

	for(int x = 11; x < 17; x++)
	{
		modifier = (int*) &file_memory[chunk_location[x]];
		*modifier -= 4;
	}

	file.seekg(list_chunks[10].top + 4);

	file.read(buffer, 4);
	int num_elements = *(int*) buffer;

	file.seekg((num_elements - 1) * 4, file.cur);

	int byte_test_loc = file.tellg();

	// generate class to handle this
	for(long x = 0; x < filesize; x++)
	{
		if(byte_test_loc == x) x += 3;
		else if((x <= w_list[0].get_fp_start()) || (x > w_list[0].get_fp_end())) o_file.write(&file_memory[x], 1);
	}

	// end what class should do

	o_file.close();
	modifier = nullptr;
	wr = nullptr;
} // end wrestler load

{ // start disp_order load
	for(auto& w : w_list) cout << w.get_name() << endl;
}
	file.close();
	delete [] chunk_location;
	delete [] file_memory;
	delete [] list_chunks;
	delete [] buffer;
	return 0;
}

string getString(ifstream& in)
{
	int length = getLength(in);
	char* buffer = new char[length + 1]();
	in.read(buffer, length);
	buffer[length] = '\0';
	string result(buffer);
	delete [] buffer;
	return result;
}

int getLength(ifstream& in)
{
    int count = 0;
    int shift = 0;
    char b;
    do {
        // Check for a corrupted stream.  Read a max of 5 bytes.
        // In a future version, add a DataFormatException.
        if (shift == 5 * 7)
        {
        	//cerr << "error reading string" << endl;
        	//exit(-1);
        }

        // ReadByte handles end of stream cases for us.
        in.read(&b, 1);
        count |= (b & 0x7F) << shift;
        shift += 7;
    } while ((b & 0x80) != 0);
    return count;
}
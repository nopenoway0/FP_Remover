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

struct saveDataChunk{
	int32_t chunkKind;
	int32_t version;
	int64_t top;
};

typedef struct optionSettings settings;
typedef struct saveDataChunk dataChunk;


int main(int argc, char* args[]){
	string filename;
	if(argc  == 2)
	{
		filename = args[1];
	}
	else filename = FILE_NAME;
	ifstream file(filename, ios::in | ios::binary);
	file.seekg(0, ios_base::end);
	long filesize = file.tellg();
	file.seekg(ios_base::beg);

	char* file_memory = new char[filesize];

	file.read(file_memory, filesize);

	string read_string;
	char* buffer = new char[8];
	int num_chunks = -1;
	int wrestler_num_location;
	file.seekg(0L);
	vector<Wrestler> w_list;
	// print ver and cunkdatanum
	for(int x = 0; x < 2; x++)
	{
		file.read(buffer, 4);
		if(x == NUM_CHUNKS) num_chunks = *(int32_t*) buffer;
	}

	saveDataChunk* list_chunks = new saveDataChunk[num_chunks];

	file.seekg(16L); // move to location to begin loading chunks
	// load first chunk data total chunks given by second integer read
	for(int x = 0; x < num_chunks; x++)
	{

		file.read(buffer, 4);
		list_chunks[x].chunkKind = *(int32_t*) buffer;

		file.read(buffer, 4);
		list_chunks[x].version = *(int32_t*) buffer;

		file.read(buffer, 8);
		list_chunks[x].top = *(int64_t*) buffer;

	}

	for(int x = 0; x < num_chunks; x++)
	{
		cout << x << ": " << list_chunks[x].chunkKind << " " << list_chunks[x].version << " " << list_chunks[x].top << endl;
	}

	// set to wreslter load start location
	file.seekg(list_chunks[4].top);

	int number_wrestlers;
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
			cout << counter << ": " << *(int*) buffer << endl;
			counter++;
		}

		file.read(buffer, 1);
		cout << counter << ": " << *(bool*) buffer << endl;
		counter++;

		//cout << "start of name loading: " << file.tellg() << endl;
		for(int y = 0; y < 4; y++)
		{
			read_string = getString(file);
			//wr->set_name(read_string);
			//cout << read_string << endl;
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
			cout << counter << ": " << *(int*) buffer << endl;
			counter++;
			if(counter == 12) break; // paper starts at this point
		}

		//wr.set_field(*(int*) buffer, )

		file.seekg(23 * 4, file.cur); // skip 24 integers
		file.seekg((30 * 4) + 1, file.cur); // skip 30 integers and 1 bool
		file.seekg(91 * 4 * 3, file.cur); // skip 90 integers  3 times
		file.seekg(273 * 4, file.cur); // skip 274 integers
		
		file.read(buffer, 4);
		cout << "skill points: " << *(int*) buffer << endl;

		read_string = getString(file);
		cout << "move name " << read_string << endl;

		file.seekg(17, file.cur); // skip 17 bytes

		read_string = getString(file);

		cout << "file directory name " << read_string << endl;

		// costume data loading
		file.read(buffer, 4);
		cout << "ver. " << *(int*) buffer << endl;

		file.read(buffer, 4);
		cout << "stance: " << *(int*) buffer << endl;

		file.read(buffer, 4);
		cout << "form size: " << *(int*) buffer << endl;

		for(int y = 0; y < 4; y++)
		{
			file.seekg(1, file.cur); // skip bool
			for(int z = 0; z < 144; z++) read_string = getString(file);
			file.seekg((144 * 4 * 4) + (149 * 4), file.cur); // skip 144 * 4 float then 149 float
		}
		wr->set_fp_end(file.tellg());
		w_list.push_back(*wr);
		cout << "------------------FINISH LOADING WRESTLER--------------" << endl;
		delete wr;
	}

	for(auto& x : w_list)
	{
		cout << "Name: " << x.get_name() << " Size: " << x.get_fp_end() - x.get_fp_start() << endl;
	}

	file.close();

	ofstream o_file("output", ios_base::binary);

	// create writer class to write out everything except memory chunks

	cout << "Removing " << w_list[0].get_name() << " starting at " << w_list[0].get_fp_start() << " end at " << w_list[0].get_fp_end() << endl;

	cout << "file size: " << filesize << endl;

	// write to file, without first wrestler
	
	// set new number to wrestlers
	modifier = (int*) &file_memory[wrestler_num_location];
	*modifier = (number_wrestlers - 1);

	for(long x = 0; x < filesize; x++) if((x <= w_list[0].get_fp_start()) || (x > w_list[0].get_fp_end())) o_file.write(&file_memory[x], 1);

	// end what class should do
	o_file.close();
	modifier = nullptr;
	delete [] file_memory;
	delete [] list_chunks;
	delete [] buffer;
	return 0;
}

string getString(ifstream& in)
{
	int length = getLength(in);
	if(length > 0)
	{
		char* buffer = new char[length]();
		in.read(buffer, length);
		string result(buffer);
		delete [] buffer;
		return result;
	}
	else return "";
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
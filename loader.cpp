#include <fstream>
#include <iostream>
#include <stdint.h>
#include <cwchar>
#include <cstring>

#define FILE_NAME "savedata"

using namespace std;

void printChar(char* buffer, int size);

enum{
	VER_NUM,
	NUM_CHUNKS
};

struct saveDataChunk{
	int32_t chunkKind;
	int32_t version;
	int64_t top;
};

struct optionSettings{
		int seVol;
		int voiceVol;
		int bgmVol_Menu;
		int bgmVol_Entrance;
		int bgmVol_Match;
		bool vibration;
		bool entranceScene;
		int screenMode;
		int language;
		bool V_Sync;
		int ScreenSize;
		bool useTextureCache;
};

typedef struct optionSettings settings;
typedef struct saveDataChunk dataChunk;

int getLength(ifstream&);

int main(){
	ifstream file(FILE_NAME, ios::in | ios::binary);

	char* buffer = (char*) calloc(sizeof(char), 8);
	int num_chunks = -1;
	file.seekg(0L);

	// print ver and cunkdatanum
	for(int x = 0; x < 2; x++)
	{
		file.read(buffer, 4);
		if(x == NUM_CHUNKS) num_chunks = *(int32_t*) buffer;
	}

	saveDataChunk* list_chunks = new saveDataChunk[num_chunks];

	// reserved bytes for header
	file.read(buffer, 8);

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

	file.seekg(list_chunks[4].top);
	int number_wrestlers;
	file.read(buffer, 4);
	number_wrestlers = *(int*) buffer;
	cout << "Number Wrestlers: " << number_wrestlers << endl;
	int length = 0;
	char* name = nullptr;
	// does x amount of wreslers
	int counter = 0; // for printing
	for(int x = 0; x < number_wrestlers; x++)
	{
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
			memset(name, 0, length);
			length = getLength(file);
			if(length > 0)
			{
				free(name);
				name = (char*) calloc(1, length);
				file.read(name, length);
				cout << counter << ": " << "string length: " << length << " string: " << "name_" << y << ": " << name << endl;
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

		file.seekg(23 * 4, file.cur); // skip 24 integers
		file.seekg((30 * 4) + 1, file.cur); // skip 30 integers and 1 bool
		file.seekg(91 * 4 * 3, file.cur); // skip 90 integers  3 times
		file.seekg(273 * 4, file.cur); // skip 274 integers
		
		file.read(buffer, 4);
		cout << "skill points: " << *(int*) buffer << endl;

		// read string
		length = getLength(file);
		free(name);
		name = (char*) calloc(sizeof(char), length);
		file.read(name, length);
		cout << "read " << length << " bytes. move name = " << name << endl;

		file.seekg(17, file.cur); // skip 17 bytes
		int old_length = length;
		length = getLength(file);
		if(length > 0)
		{
			file.read(name, length);
			cout << "read " << length << " bytes. file directory name = " << name << endl;
		}
		else
		{
			memset(name, 0, old_length);
			free(name);
			name = nullptr;
		}

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
			for(int z = 0; z < 144; z++)
			{
				length = getLength(file);
				free(name);
				name = (char*) calloc(sizeof(char), length);
				file.read(name, length);
				//cout << name << endl;
			}
			file.seekg(144 * 4 * 4, file.cur); // skip the next 149 single byte reads
			file.seekg(5 * 4, file.cur);
			file.seekg(144 * 4, file.cur);
		}
		cout << "------------------FINISH LOADING WRESTLER--------------" << endl;
	}

	file.close();
	delete [] list_chunks;
	free(buffer);
	return 0;
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
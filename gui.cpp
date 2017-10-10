// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
// 
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "Wrestler.h"
#include "WrestlerLoader.h"
#include <cstring>
#include <vector>

#define NUM_CHUNKS 17

using fp_remover::loaders::WrestlerLoader;
using fp_remover::classes::Wrestler;
using namespace std;

unsigned long delete_section(char*& p, unsigned long start, unsigned long length, unsigned long size);

unsigned long remove_wrestler(char*& fp_m, vector<Wrestler>& w, int index, int count, unsigned long skip_byte_loc, unsigned long size, const long int* chunk_locations);

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};
class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MyFrame()
    {
       /* for(int x = 0; x < checkboxes.size(); x++) delete [] checkboxes[x];
        checkboxes.clear();
        delete wl;*/
    }
private:
    void refresh_checkboxes();
    void OnExit(wxCommandEvent& event);
    void Delete_Wrestlers(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
    vector<wxCheckBox*> checkboxes;
    WrestlerLoader* wl;
    vector<Wrestler> wrestlers;
    char* fp_m;
    void get_vars();
    unsigned long wrestler_count_loc, skip_byte_loc, size;
    const long* chunk_locations;
};
enum
{
    wxID_DEL,
    wxID_C_PANEL
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
    EVT_MENU(wxID_DEL, MyFrame::Delete_Wrestlers)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( "Wrestler Remover", wxPoint(50, 50), wxSize(450, 340) );
    frame->Show( true );
    return true;
}
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    wl = new WrestlerLoader("savedata");
    get_vars();
    wxMessageBox("Loaded: " + to_string(wrestlers.size()) + " Wrestlers");
    wxMenu* menu_commit = new wxMenu;
    menu_commit->Append(wxID_DEL, "Commit");
    wxMenuBar* menu_bar = new wxMenuBar;
    menu_bar->Append(menu_commit, "&File");
    SetMenuBar(menu_bar);

    wxBoxSizer* b1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxScrolledWindow* panel = new wxScrolledWindow(this, wxID_C_PANEL);


    for(int x = 0; x < wrestlers.size(); x++)
    {

        checkboxes.push_back(new wxCheckBox(panel, wxID_ANY, wrestlers.at(x).get_name()));
        sizer->Add(checkboxes.at(x));
    }
    panel->SetSizer(sizer);
    panel->SetScrollRate(5, 5);
}
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close( true );
}

void MyFrame::Delete_Wrestlers(wxCommandEvent& event)
{
    // scan checkboxes
    for(int x = checkboxes.size() - 1; x > 0; x--)
    {
        if(checkboxes.at(x)->IsChecked())
        {
            size = remove_wrestler(fp_m, wrestlers, x, wl->get_record_loc(), wl->get_byte_skip_loc(), wl->get_filesize(), wl->get_chunk_locations());
            wxMessageBox(wrestlers.at(x).get_name());
            wl->write(fp_m, size);
            wrestlers.clear();
            get_vars();
        }
    }
    refresh_checkboxes();
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

    cout << "removing wrestler " << w.at(index).get_name().c_str() << " with id " << *(int*) &fp_m[skip_byte_loc] << " at " << w.at(index).get_fp_start() << endl;


    size = delete_section(fp_m, skip_byte_loc, 4, size);

    size = delete_section(fp_m, w.at(index).get_fp_start(), wrestler_size, size);

    cout << "removed wrestler from memory" << endl;

    return size;
}

void MyFrame::get_vars()
{
    wrestlers.clear();
    wl->load(); // update
    wrestler_count_loc = wl->get_record_loc();
    skip_byte_loc = wl->get_byte_skip_loc();
    wrestlers = wl->get_wrestlers();
    const char* fp = wl->get_file_p();
    size = wl->get_filesize();
    chunk_locations = wl->get_chunk_locations();
    fp_m = new char[size];
    fp_m = (char*) memcpy(fp_m, fp, size);
}

void MyFrame::refresh_checkboxes()
{
    wxScrolledWindow* panel = (wxScrolledWindow*) 
}
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
    void populate_checkboxes();
    void refresh_checkboxes();
    void OnExit(wxCommandEvent& event);
    void Delete_Wrestlers(wxCommandEvent& event);
    void get_vars();
    unsigned long wrestler_count_loc, skip_byte_loc, size;
    void ChangeFilename(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
    vector<wxCheckBox*> checkboxes;
    WrestlerLoader* wl;
    vector<Wrestler> wrestlers;
    char* fp_m;

    const long* chunk_locations;
};
enum
{
    wxID_DEL,
    wxID_CHNG_F,
    wxID_C_PANEL
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
    EVT_MENU(wxID_DEL, MyFrame::Delete_Wrestlers)
    EVT_MENU(wxID_CHNG_F, MyFrame::ChangeFilename)
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
    fp_m = nullptr;
    try{
        get_vars();
    }catch(runtime_error& e)
    {
        wxMessageBox(e.what());
    }
    wxMessageBox("Loaded: " + to_string(wrestlers.size()) + " Wrestlers");
    wxMenu* menu_file = new wxMenu;
    menu_file->Append(wxID_DEL, "Commit");
    menu_file->Append(wxID_CHNG_F, "Find File");
    wxMenuBar* menu_bar = new wxMenuBar;
    menu_bar->Append(menu_file, "&File");
    SetMenuBar(menu_bar);

    wxScrolledWindow* panel = new wxScrolledWindow(this, wxID_C_PANEL);

    refresh_checkboxes();
}
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close( true );
}

void MyFrame::Delete_Wrestlers(wxCommandEvent& event)
{
    // scan checkboxes
    for(int x = checkboxes.size() - 1; x >= 0; x--)
    {
        if(checkboxes.at(x)->IsChecked())
        {
            size = remove_wrestler(fp_m, wrestlers, x, wl->get_record_loc(), wl->get_byte_skip_loc(), wl->get_filesize(), wl->get_chunk_locations());
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

    size = delete_section(fp_m, skip_byte_loc, 4, size);
    size = delete_section(fp_m, w.at(index).get_fp_start(), wrestler_size, size);

    return size;
}

void MyFrame::get_vars()
{
    delete [] fp_m;
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
    wxScrolledWindow* panel = (wxScrolledWindow*) this->FindWindow(wxID_C_PANEL);
    panel->DestroyChildren();
    populate_checkboxes();
}

void MyFrame::populate_checkboxes()
{
    wxScrolledWindow* panel = (wxScrolledWindow*) this->FindWindow(wxID_C_PANEL);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    checkboxes.clear();
    for(int x = 0; x < wrestlers.size(); x++)
    {

        checkboxes.push_back(new wxCheckBox(panel, wxID_ANY, wrestlers.at(x).get_name()));
        sizer->Add(checkboxes.at(x));
    }
    panel->SetSizer(sizer);
    panel->SetScrollRate(5, 5);
    panel->FitInside();
    panel->Layout();
    panel->Refresh();
    panel->Update();
}

void MyFrame::ChangeFilename(wxCommandEvent& e)
{
    wxFileDialog* fd = new wxFileDialog(this, "Select savedata File");
    if(fd->ShowModal() == wxID_OK)
    {
        if(!wl->open_file(fd->GetPath().ToStdString()))
        {
            wxMessageBox("Could not open file");
        }
        get_vars();
        refresh_checkboxes();
    }
}
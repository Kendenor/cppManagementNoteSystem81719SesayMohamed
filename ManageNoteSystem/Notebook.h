#ifndef NOTEBOOK_H
#define NOTEBOOK_H

#include <string>
#include <vector>
#include "Note.h"

using namespace std;

class Notebook {
public:
    // Default constructor
    Notebook();
    
    // Parameterized constructor
    Notebook(string folderName);
    
    // Getters & Setters
    string getName() const;
    void setName(const string& newName);
    vector<Note>& getNotes();
    const vector<Note>& getNotes() const;
    
    // Note Management Operations
    void addNote(const Note& note);
    bool deleteNote(int noteId);
    Note* findNote(int noteId);
    
private:
    string name;
    vector<Note> notes;
};

#endif // NOTEBOOK_H

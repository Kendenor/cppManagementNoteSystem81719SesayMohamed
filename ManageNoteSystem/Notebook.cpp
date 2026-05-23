#include "Notebook.h"

// Default constructor
Notebook::Notebook() : name("") {}

// Parameterized constructor
Notebook::Notebook(string folderName) : name(folderName) {}

// Getters & Setters
string Notebook::getName() const {
    return name;
}

void Notebook::setName(const string& newName) {
    name = newName;
}

vector<Note>& Notebook::getNotes() {
    return notes;
}

const vector<Note>& Notebook::getNotes() const {
    return notes;
}

// Add a note to this notebook
void Notebook::addNote(const Note& note) {
    notes.push_back(note);
}

// Delete a note by its ID
bool Notebook::deleteNote(int noteId) {
    for (auto it = notes.begin(); it != notes.end(); ++it) {
        if (it->getId() == noteId) {
            notes.erase(it);
            return true;
        }
    }
    return false;
}

// Find a note by its ID and return its pointer
Note* Notebook::findNote(int noteId) {
    for (auto &note : notes) {
        if (note.getId() == noteId) {
            return &note;
        }
    }
    return nullptr;
}

// Archive a note (move to trash)
bool Notebook::archiveNote(int noteId) {
    for (auto &note : notes) {
        if (note.getId() == noteId) {
            note.setIsArchived(true);
            return true;
        }
    }
    return false;
}

// Restore a note from trash
bool Notebook::restoreNote(int noteId) {
    for (auto &note : notes) {
        if (note.getId() == noteId) {
            note.setIsArchived(false);
            return true;
        }
    }
    return false;
}

#ifndef NOTESERVICE_H
#define NOTESERVICE_H

#include <vector>
#include <string>
#include "User.h"
#include "Notebook.h"

using namespace std;

class NoteService {
public:
    NoteService();
    
    // User Operations
    bool registerUser(const string& name, const string& id);
    bool loginUser(const string& username);
    void logoutUser();
    User* getCurrentUser() const;
    const vector<User>& getUsers() const;
    
    // Notebook Operations
    bool createNotebook(const string& folderName);
    bool selectNotebook(const string& folderName);
    Notebook* getCurrentNotebook() const;
    vector<Notebook>& getNotebooks();
    const vector<Notebook>& getNotebooks() const;
    
    // Persistence operations (Database Save/Load)
    bool saveToFile(const string& filepath);
    bool loadFromFile(const string& filepath);
    
    // Advanced Note Queries (across all notebooks if none selected, or within current notebook)
    vector<Note*> searchNotes(const string& query);
    vector<Note*> filterByTag(const string& tag);
    
    // ID Generator
    int getNextNoteId();

private:
    vector<User> users;
    vector<Notebook> notebooks;
    User* currentUser;
    Notebook* currentNotebook;
    int nextNoteId;
    
    // Helper to find a user pointer
    User* findUser(const string& username);
    // Helper to find a notebook pointer
    Notebook* findNotebook(const string& folderName);
};

#endif // NOTESERVICE_H

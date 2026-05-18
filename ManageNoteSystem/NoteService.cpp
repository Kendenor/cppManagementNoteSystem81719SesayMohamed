#include "NoteService.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;

// Helper to convert string to lowercase for case-insensitive search
static string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return tolower(c);
    });
    return s;
}

NoteService::NoteService() : currentUser(nullptr), currentNotebook(nullptr), nextNoteId(1) {}

// User Operations
bool NoteService::registerUser(const string& name, const string& id) {
    if (findUser(name) != nullptr) {
        return false; // User already exists
    }
    users.push_back(User(name, id));
    return true;
}

bool NoteService::loginUser(const string& username) {
    User* u = findUser(username);
    if (u != nullptr) {
        currentUser = u;
        currentNotebook = nullptr; // Reset folder selection on login
        return true;
    }
    return false;
}

void NoteService::logoutUser() {
    currentUser = nullptr;
    currentNotebook = nullptr;
}

User* NoteService::getCurrentUser() const {
    return currentUser;
}

const vector<User>& NoteService::getUsers() const {
    return users;
}

// Notebook Operations
bool NoteService::createNotebook(const string& folderName) {
    if (findNotebook(folderName) != nullptr) {
        return false; // Notebook already exists
    }
    notebooks.push_back(Notebook(folderName));
    return true;
}

bool NoteService::selectNotebook(const string& folderName) {
    Notebook* nb = findNotebook(folderName);
    if (nb != nullptr) {
        currentNotebook = nb;
        return true;
    }
    return false;
}

Notebook* NoteService::getCurrentNotebook() const {
    return currentNotebook;
}

vector<Notebook>& NoteService::getNotebooks() {
    return notebooks;
}

const vector<Notebook>& NoteService::getNotebooks() const {
    return notebooks;
}

// File Persistence - Save to File
bool NoteService::saveToFile(const string& filepath) {
    ofstream file(filepath);
    if (!file.is_open()) return false;
    
    // 1. Next ID
    file << "NEXT_ID:" << nextNoteId << "\n";
    
    // 2. Users
    file << "USERS:" << users.size() << "\n";
    for (const auto& u : users) {
        file << u.getUsername() << "|" << u.getUserId() << "\n";
    }
    
    // 3. Notebooks
    file << "NOTEBOOKS:" << notebooks.size() << "\n";
    for (const auto& nb : notebooks) {
        file << nb.getName() << "\n";
    }
    
    // 4. Notes Flat Serialization
    int totalNotes = 0;
    for (const auto& nb : notebooks) {
        totalNotes += nb.getNotes().size();
    }
    file << "NOTES:" << totalNotes << "\n";
    
    for (auto& nb : notebooks) {
        for (const auto& note : nb.getNotes()) {
            file << "NOTE_META:" << nb.getName() << "|" << note.getId() << "|" 
                 << (note.getIsPinned() ? "1" : "0") << "|" << note.getCreatedAt() << "|";
            
            // Serialize tags
            const auto& t = note.getTags();
            for (size_t i = 0; i < t.size(); ++i) {
                file << t[i];
                if (i < t.size() - 1) file << ",";
            }
            file << "\n";
            
            // Write Title
            file << note.getTitle() << "\n";
            
            // Write Content and End token
            file << note.getContent() << "\n";
            file << "[END_NOTE]\n";
        }
    }
    
    file.close();
    return true;
}

// File Persistence - Load from File
bool NoteService::loadFromFile(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) return false; // File doesn't exist yet, return true as it is clean start
    
    users.clear();
    notebooks.clear();
    currentUser = nullptr;
    currentNotebook = nullptr;
    nextNoteId = 1;
    
    string line;
    while (getline(file, line)) {
        if (line.rfind("NEXT_ID:", 0) == 0) {
            nextNoteId = stoi(line.substr(8));
        } else if (line.rfind("USERS:", 0) == 0) {
            int numUsers = stoi(line.substr(6));
            for (int i = 0; i < numUsers; ++i) {
                string userLine;
                if (getline(file, userLine)) {
                    size_t pos = userLine.find('|');
                    if (pos != string::npos) {
                        string name = userLine.substr(0, pos);
                        string id = userLine.substr(pos + 1);
                        users.push_back(User(name, id));
                    }
                }
            }
        } else if (line.rfind("NOTEBOOKS:", 0) == 0) {
            int numNotebooks = stoi(line.substr(10));
            for (int i = 0; i < numNotebooks; ++i) {
                string nbLine;
                if (getline(file, nbLine)) {
                    notebooks.push_back(Notebook(nbLine));
                }
            }
        } else if (line.rfind("NOTES:", 0) == 0) {
            string metaLine;
            while (getline(file, metaLine)) {
                if (metaLine.rfind("NOTE_META:", 0) == 0) {
                    string data = metaLine.substr(10);
                    
                    vector<string> tokens;
                    size_t pos = 0;
                    while ((pos = data.find('|')) != string::npos) {
                        tokens.push_back(data.substr(0, pos));
                        data.erase(0, pos + 1);
                    }
                    tokens.push_back(data); // remainder: tags
                    
                    if (tokens.size() >= 5) {
                        string nbName = tokens[0];
                        int id = stoi(tokens[1]);
                        bool isPinned = (tokens[2] == "1");
                        string createdAt = tokens[3];
                        string tagsJoined = tokens[4];
                        
                        vector<string> tags;
                        size_t tPos = 0;
                        while ((tPos = tagsJoined.find(',')) != string::npos) {
                            string t = tagsJoined.substr(0, tPos);
                            if (!t.empty()) tags.push_back(t);
                            tagsJoined.erase(0, tPos + 1);
                        }
                        if (!tagsJoined.empty()) tags.push_back(tagsJoined);
                        
                        string title;
                        getline(file, title);
                        
                        string content = "";
                        string contentLine;
                        bool first = true;
                        while (getline(file, contentLine)) {
                            if (contentLine == "[END_NOTE]") {
                                break;
                            }
                            if (!first) content += "\n";
                            content += contentLine;
                            first = false;
                        }
                        
                        Note loadedNote(id, title, content, createdAt, tags, isPinned);
                        Notebook* nb = findNotebook(nbName);
                        if (nb) {
                            nb->addNote(loadedNote);
                        }
                    }
                }
            }
        }
    }
    
    file.close();
    return true;
}

// Search notes by title or content matching query
vector<Note*> NoteService::searchNotes(const string& query) {
    vector<Note*> results;
    string lowQuery = toLower(query);
    
    auto searchInNotebook = [&](Notebook& nb) {
        for (auto& note : nb.getNotes()) {
            string lowTitle = toLower(note.getTitle());
            string lowContent = toLower(note.getContent());
            if (lowTitle.find(lowQuery) != string::npos || lowContent.find(lowQuery) != string::npos) {
                results.push_back(&note);
            }
        }
    };
    
    if (currentNotebook != nullptr) {
        searchInNotebook(*currentNotebook);
    } else {
        for (auto& nb : notebooks) {
            searchInNotebook(nb);
        }
    }
    
    // Sort pinned notes to the top
    sort(results.begin(), results.end(), [](const Note* a, const Note* b) {
        return a->getIsPinned() > b->getIsPinned();
    });
    
    return results;
}

// Filter notes containing a specific tag
vector<Note*> NoteService::filterByTag(const string& tag) {
    vector<Note*> results;
    string lowTag = toLower(tag);
    
    auto filterInNotebook = [&](Notebook& nb) {
        for (auto& note : nb.getNotes()) {
            for (const auto& t : note.getTags()) {
                if (toLower(t) == lowTag) {
                    results.push_back(&note);
                    break;
                }
            }
        }
    };
    
    if (currentNotebook != nullptr) {
        filterInNotebook(*currentNotebook);
    } else {
        for (auto& nb : notebooks) {
            filterInNotebook(nb);
        }
    }
    
    // Sort pinned notes to the top
    sort(results.begin(), results.end(), [](const Note* a, const Note* b) {
        return a->getIsPinned() > b->getIsPinned();
    });
    
    return results;
}

// Unique ID Generator
int NoteService::getNextNoteId() {
    return nextNoteId++;
}

// Private Helpers
User* NoteService::findUser(const string& username) {
    for (auto& u : users) {
        if (toLower(u.getUsername()) == toLower(username)) {
            return &u;
        }
    }
    return nullptr;
}

Notebook* NoteService::findNotebook(const string& folderName) {
    for (auto& nb : notebooks) {
        if (toLower(nb.getName()) == toLower(folderName)) {
            return &nb;
        }
    }
    return nullptr;
}

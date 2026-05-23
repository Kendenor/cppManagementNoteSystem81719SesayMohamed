#include "NoteService.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <map>
#include <iomanip>

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
                 << (note.getIsPinned() ? "1" : "0") << "|" 
                 << (note.getIsArchived() ? "1" : "0") << "|" 
                 << note.getCreatedAt() << "|";
            
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
                        bool isArchived = false;
                        string createdAt;
                        string tagsJoined;
                        
                        if (tokens.size() == 5) {
                            createdAt = tokens[3];
                            tagsJoined = tokens[4];
                        } else {
                            isArchived = (tokens[3] == "1");
                            createdAt = tokens[4];
                            tagsJoined = tokens[5];
                        }
                        
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
                        
                        Note loadedNote(id, title, content, createdAt, tags, isPinned, isArchived);
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
vector<Note*> NoteService::searchNotes(const string& query, bool includeArchived) {
    vector<Note*> results;
    string lowQuery = toLower(query);
    
    auto searchInNotebook = [&](Notebook& nb) {
        for (auto& note : nb.getNotes()) {
            if (!includeArchived && note.getIsArchived()) continue;
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
vector<Note*> NoteService::filterByTag(const string& tag, bool includeArchived) {
    vector<Note*> results;
    string lowTag = toLower(tag);
    
    auto filterInNotebook = [&](Notebook& nb) {
        for (auto& note : nb.getNotes()) {
            if (!includeArchived && note.getIsArchived()) continue;
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

// Export a note to a formatted text file
bool NoteService::exportNoteToTxt(int noteId, const string& folderName) {
    Notebook* nb = findNotebook(folderName);
    if (!nb) return false;
    
    Note* note = nb->findNote(noteId);
    if (!note) return false;
    
    // Sanitise title for filename
    string safeTitle = note->getTitle();
    for (char& c : safeTitle) {
        if (c == ' ' || c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            c = '_';
        }
    }
    
    string filename = "Note_Export_" + to_string(noteId) + "_" + safeTitle + ".txt";
    ofstream file(filename);
    if (!file.is_open()) return false;
    
    string divider = "============================================================";
    file << "+" << divider << "+\n";
    file << "|          NOTEIFY - EXPORTED NOTE DOCUMENT                 |\n";
    file << "+" << divider << "+\n";
    file << "| Note ID  : " << noteId << "\n";
    file << "| Notebook : " << folderName << "\n";
    file << "| Title    : " << note->getTitle() << "\n";
    file << "| Created  : " << note->getCreatedAt() << "\n";
    file << "| Status   : " << (note->getIsPinned() ? "PINNED" : "Normal") << (note->getIsArchived() ? " [ARCHIVED]" : "") << "\n";
    
    // Tags
    file << "| Tags     : ";
    const auto& tags = note->getTags();
    if (tags.empty()) {
        file << "(None)";
    } else {
        for (size_t i = 0; i < tags.size(); ++i) {
            file << tags[i];
            if (i < tags.size() - 1) file << ", ";
        }
    }
    file << "\n";
    file << "+" << divider << "+\n\n";
    
    // Content
    file << note->getContent() << "\n\n";
    file << "+" << divider << "+\n";
    
    // Word/Char stats
    int charCount = note->getContent().length();
    istringstream ss(note->getContent());
    string w;
    int wordCount = 0;
    while (ss >> w) wordCount++;
    file << "| Words: " << wordCount << " | Characters: " << charCount << "\n";
    file << "+" << divider << "+\n";
    file << "| Exported by NOTEIFY C++ Premium Note Manager               |\n";
    file << "+" << divider << "+\n";
    
    file.close();
    return true;
}

// Analytics and Statistics Dashboard
void NoteService::displayAnalytics() const {
    // ANSI colors
    string cyan    = "\033[36m";
    string green   = "\033[32m";
    string yellow  = "\033[1;33m";
    string magenta = "\033[35m";
    string red     = "\033[31m";
    string reset   = "\033[0m";
    string bold    = "\033[1m";
    
    string div = "============================================================";
    
    cout << "\n" << cyan << "+" << div << "+" << reset << "\n";
    cout << cyan << "|" << reset << bold << "         ★  NOTEIFY SYSTEM ANALYTICS DASHBOARD  ★        " << reset << cyan << "|" << reset << "\n";
    cout << cyan << "+" << div << "+" << reset << "\n";
    
    int totalNotes = 0, activeNotes = 0, archivedNotes = 0;
    int pinnedNotes = 0, totalWords = 0, totalChars = 0;
    
    string mostActiveNotebook = "";
    int maxNotebookNotes = 0;
    
    map<string, int> tagFrequency;
    
    for (const auto& nb : notebooks) {
        int nbActiveCount = 0;
        for (const auto& note : nb.getNotes()) {
            totalNotes++;
            totalChars += note.getContent().length();
            
            // Word count
            istringstream iss(note.getContent());
            string w;
            while (iss >> w) totalWords++;
            
            if (note.getIsArchived()) {
                archivedNotes++;
            } else {
                activeNotes++;
                nbActiveCount++;
            }
            if (note.getIsPinned()) pinnedNotes++;
            
            // Tag frequency
            for (const auto& tag : note.getTags()) {
                tagFrequency[tag]++;
            }
        }
        if (nbActiveCount > maxNotebookNotes) {
            maxNotebookNotes = nbActiveCount;
            mostActiveNotebook = nb.getName();
        }
    }
    
    // Calculate averages
    double avgWords = (totalNotes > 0) ? (double)totalWords / totalNotes : 0.0;
    double avgChars = (totalNotes > 0) ? (double)totalChars / totalNotes : 0.0;
    
    // Find most frequent tag
    string topTag = "(None)";
    int topTagCount = 0;
    for (const auto& kv : tagFrequency) {
        if (kv.second > topTagCount) {
            topTagCount = kv.second;
            topTag = kv.first;
        }
    }
    
    // Print stats
    auto printRow = [&](const string& label, const string& value, const string& color) {
        string row = " " + label + value;
        int padLen = 60 - (int)row.length() - 2;
        if (padLen < 0) padLen = 0;
        cout << cyan << "| " << reset << color << label << reset << value << string(padLen, ' ') << cyan << " |" << reset << "\n";
    };
    
    auto printLabel = [&](const string& label, const string& color) {
        int padLen = 60 - (int)label.length() - 2;
        if (padLen < 0) padLen = 0;
        cout << cyan << "|" << reset << " " << color << bold << label << reset << string(padLen, ' ') << cyan << " |" << reset << "\n";
    };
    
    printLabel(" >> NOTE OVERVIEW", yellow);
    cout << cyan << "|" << string(58, '-') << "|" << reset << "\n";
    printRow(" Total Notebooks:    ", to_string(notebooks.size()), green);
    printRow(" Total Notes:        ", to_string(totalNotes), green);
    printRow(" Active Notes:       ", to_string(activeNotes), green);
    printRow(" Archived (Trash):   ", to_string(archivedNotes), red);
    printRow(" Pinned Notes:       ", to_string(pinnedNotes), yellow);
    
    cout << cyan << "|" << string(58, '-') << "|" << reset << "\n";
    printLabel(" >> CONTENT STATISTICS", yellow);
    cout << cyan << "|" << string(58, '-') << "|" << reset << "\n";
    
    ostringstream avgWStr, avgCStr;
    avgWStr << fixed << setprecision(1) << avgWords;
    avgCStr << fixed << setprecision(1) << avgChars;
    
    printRow(" Avg Words/Note:     ", avgWStr.str(), magenta);
    printRow(" Avg Chars/Note:     ", avgCStr.str(), magenta);
    printRow(" Total Words:        ", to_string(totalWords), magenta);
    printRow(" Total Characters:   ", to_string(totalChars), magenta);
    
    cout << cyan << "|" << string(58, '-') << "|" << reset << "\n";
    printLabel(" >> TOP INSIGHTS", yellow);
    cout << cyan << "|" << string(58, '-') << "|" << reset << "\n";
    
    if (mostActiveNotebook.empty()) mostActiveNotebook = "(None)";
    printRow(" Most Active Folder: ", mostActiveNotebook + " (" + to_string(maxNotebookNotes) + " notes)", green);
    printRow(" Most Used Tag:      ", topTag + " (x" + to_string(topTagCount) + ")", yellow);
    
    if (!tagFrequency.empty()) {
        cout << cyan << "|" << string(58, '-') << "|" << reset << "\n";
        printLabel(" >> ALL TAGS", yellow);
        for (const auto& kv : tagFrequency) {
            printRow("   #" + kv.first + "  ", "x" + to_string(kv.second), magenta);
        }
    }
    
    cout << cyan << "+" << div << "+" << reset << "\n\n";
}

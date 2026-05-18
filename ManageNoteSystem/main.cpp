#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <ctime>
#include "NoteService.h"

using namespace std;

// Helper to get current timestamp
string getCurrentTimestamp() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ltm);
    return string(buf);
}

// Clean buffer after using cin >> choice
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Beautiful headers and formatting helpers
void printDivider() {
    cout << "============================================================\n";
}

void printASCIIBanner() {
    cout << "\n";
    printDivider();
    cout << "  _   _   ___ _____ _____ ___ _____   __  __  \n";
    cout << " | \\ | | / _ \\_   _| ____|_ _|  ___|  \\ \\/ /  \n";
    cout << " |  \\| || | | || | |  _|  | || |_      \\  /   \n";
    cout << " | |\\  || |_| || | | |___ | ||  _|     /  \\   \n";
    cout << " |_| \\_| \\___/ |_| |_____|___|_|      /_/\\_\\  \n";
    cout << "                                              \n";
    cout << "          C++ PREMIUM NOTE MANAGEMENT SYSTEM  \n";
    printDivider();
}

void printSessionInfo(const NoteService& ns) {
    if (ns.getCurrentUser() == nullptr) {
        cout << " [GUEST] Log in or Register to begin creating notes!\n";
    } else {
        cout << " Active User: " << ns.getCurrentUser()->getUsername() 
             << " [ID: " << ns.getCurrentUser()->getUserId() << "]\n";
        
        if (ns.getCurrentNotebook() == nullptr) {
            cout << " Active Notebook Folder: [NONE] (Select one first)\n";
        } else {
            cout << " Active Notebook Folder: " << ns.getCurrentNotebook()->getName() 
                 << " [" << ns.getCurrentNotebook()->getNotes().size() << " Note(s)]\n";
        }
    }
    printDivider();
}

int main() {
    NoteService ns;
    string dbFile = "notes_db.txt";
    
    // Automatically load notes from file database
    ns.loadFromFile(dbFile);
    
    int choice = 0;
    while (true) {
        printASCIIBanner();
        printSessionInfo(ns);
        
        // LEVEL 1: GUEST MENUS (Not logged in)
        if (ns.getCurrentUser() == nullptr) {
            cout << " 1. Log In (Existing User)\n";
            cout << " 2. Register New User\n";
            cout << " 3. Exit Application\n\n";
            cout << " Enter choice (1-3): ";
            
            if (!(cin >> choice)) {
                if (cin.eof()) break;
                cout << "\n [ERROR] Invalid Input! Please enter a number.\n";
                clearInput();
                continue;
            }
            clearInput();
            
            if (choice == 1) {
                string name;
                cout << "\n Enter Username: ";
                getline(cin, name);
                
                if (name.empty()) {
                    cout << " [ERROR] Username cannot be empty!\n";
                    continue;
                }
                
                if (ns.loginUser(name)) {
                    cout << " [SUCCESS] Welcome back, " << name << "!\n";
                } else {
                    cout << " [ERROR] User not found! Register first.\n";
                }
            }
            else if (choice == 2) {
                string name, id;
                cout << "\n Enter Username: ";
                getline(cin, name);
                cout << " Enter CIF/User ID (e.g. CIF001): ";
                getline(cin, id);
                
                if (name.empty() || id.empty()) {
                    cout << " [ERROR] Username and ID cannot be empty!\n";
                    continue;
                }
                
                if (ns.registerUser(name, id)) {
                    ns.saveToFile(dbFile);
                    cout << " [SUCCESS] User registered successfully! Logging you in...\n";
                    ns.loginUser(name);
                } else {
                    cout << " [ERROR] User already exists!\n";
                }
            }
            else if (choice == 3) {
                ns.saveToFile(dbFile);
                cout << "\n Thank you for using NOTEIFY! Saving database... Goodbye!\n\n";
                break;
            }
            else {
                cout << " [WARNING] Invalid choice! Choose between 1 and 3.\n";
            }
        }
        
        // LEVEL 2: LOGGED IN USER, NO NOTEBOOK SELECTED
        else if (ns.getCurrentNotebook() == nullptr) {
            cout << " 1. Create a Notebook Folder\n";
            cout << " 2. Select/Switch Notebook Folder\n";
            cout << " 3. Search All Notes (Global)\n";
            cout << " 4. Filter All Notes by Tag (Global)\n";
            cout << " 5. Log Out\n";
            cout << " 6. Exit Application\n\n";
            cout << " Enter choice (1-6): ";
            
            if (!(cin >> choice)) {
                if (cin.eof()) break;
                cout << "\n [ERROR] Invalid Input!\n";
                clearInput();
                continue;
            }
            clearInput();
            
            if (choice == 1) {
                string folderName;
                cout << "\n Enter Notebook Name (e.g. CompSci, Personal): ";
                getline(cin, folderName);
                if (folderName.empty()) {
                    cout << " [ERROR] Notebook name cannot be empty!\n";
                    continue;
                }
                
                if (ns.createNotebook(folderName)) {
                    ns.saveToFile(dbFile);
                    cout << " [SUCCESS] Notebook '" << folderName << "' created successfully!\n";
                    ns.selectNotebook(folderName); // Auto select it
                } else {
                    cout << " [ERROR] Notebook folder already exists!\n";
                }
            }
            else if (choice == 2) {
                const auto& folders = ns.getNotebooks();
                if (folders.empty()) {
                    cout << " [INFO] No notebooks found! Please create one first.\n";
                    continue;
                }
                
                cout << "\n Available Notebooks:\n";
                for (size_t i = 0; i < folders.size(); ++i) {
                    cout << "  - " << folders[i].getName() << "\n";
                }
                
                string targetFolder;
                cout << "\n Enter Notebook Name to open: ";
                getline(cin, targetFolder);
                
                if (ns.selectNotebook(targetFolder)) {
                    cout << " [SUCCESS] Switched to notebook '" << targetFolder << "'!\n";
                } else {
                    cout << " [ERROR] Notebook folder not found!\n";
                }
            }
            else if (choice == 3) {
                string query;
                cout << "\n Enter keyword to search: ";
                getline(cin, query);
                
                vector<Note*> results = ns.searchNotes(query);
                if (results.empty()) {
                    cout << " [INFO] No notes found matching query: '" << query << "'\n";
                } else {
                    cout << "\n --- Search Results (" << results.size() << ") ---\n";
                    for (const auto* n : results) {
                        cout << "  ID: " << n->getId() 
                             << " | [" << n->getTitle() << "]" 
                             << (n->getIsPinned() ? " [★ PINNED]" : "") << "\n";
                    }
                    printDivider();
                }
            }
            else if (choice == 4) {
                string tag;
                cout << "\n Enter tag to filter by: ";
                getline(cin, tag);
                
                vector<Note*> results = ns.filterByTag(tag);
                if (results.empty()) {
                    cout << " [INFO] No notes found with tag: '" << tag << "'\n";
                } else {
                    cout << "\n --- Tag Filter Results (" << results.size() << ") ---\n";
                    for (const auto* n : results) {
                        cout << "  ID: " << n->getId() 
                             << " | [" << n->getTitle() << "]" 
                             << (n->getIsPinned() ? " [★ PINNED]" : "") << "\n";
                    }
                    printDivider();
                }
            }
            else if (choice == 5) {
                ns.logoutUser();
                cout << " [SUCCESS] Logged out successfully!\n";
            }
            else if (choice == 6) {
                ns.saveToFile(dbFile);
                cout << "\n Thank you for using NOTEIFY! Saving database... Goodbye!\n\n";
                break;
            }
            else {
                cout << " [WARNING] Invalid choice! Choose between 1 and 6.\n";
            }
        }
        
        // LEVEL 3: LOGGED IN USER, NOTEBOOK ACTIVE
        else {
            cout << " 1. Create a Note\n";
            cout << " 2. View All Notes in Folder\n";
            cout << " 3. View Full Note details (by ID)\n";
            cout << " 4. Search Notes (This Notebook)\n";
            cout << " 5. Filter Notes by Tag (This Notebook)\n";
            cout << " 6. Edit an existing Note\n";
            cout << " 7. Toggle Note Pin (Priority)\n";
            cout << " 8. Delete a Note\n";
            cout << " 9. Back to Notebook selection\n";
            cout << " 10. Log Out\n";
            cout << " 11. Exit Application\n\n";
            cout << " Enter choice (1-11): ";
            
            if (!(cin >> choice)) {
                if (cin.eof()) break;
                cout << "\n [ERROR] Invalid Input!\n";
                clearInput();
                continue;
            }
            clearInput();
            
            Notebook* activeNB = ns.getCurrentNotebook();
            
            if (choice == 1) {
                string title, body, tagInput;
                cout << "\n Enter Note Title: ";
                getline(cin, title);
                cout << " Enter Note Content (Tip: type '\\n' for new lines or write simple lines):\n ";
                getline(cin, body);
                
                // Parse optional multi-line simulation (replace \n literal with actual character)
                size_t nPos = 0;
                while ((nPos = body.find("\\n")) != string::npos) {
                    body.replace(nPos, 2, "\n");
                }
                
                cout << " Enter Tags (comma-separated, e.g. school,test): ";
                getline(cin, tagInput);
                
                vector<string> tags;
                size_t pos = 0;
                while ((pos = tagInput.find(',')) != string::npos) {
                    string t = tagInput.substr(0, pos);
                    // trim spaces
                    size_t first = t.find_first_not_of(' ');
                    size_t last = t.find_last_not_of(' ');
                    if (first != string::npos && last != string::npos) {
                        t = t.substr(first, (last - first + 1));
                    }
                    if (!t.empty()) tags.push_back(t);
                    tagInput.erase(0, pos + 1);
                }
                // trim remaining tag
                size_t first = tagInput.find_first_not_of(' ');
                size_t last = tagInput.find_last_not_of(' ');
                if (first != string::npos && last != string::npos) {
                    tagInput = tagInput.substr(first, (last - first + 1));
                }
                if (!tagInput.empty()) tags.push_back(tagInput);
                
                Note newNote(ns.getNextNoteId(), title, body, getCurrentTimestamp(), tags);
                activeNB->addNote(newNote);
                ns.saveToFile(dbFile);
                cout << " [SUCCESS] Note created successfully with ID: " << newNote.getId() << "\n";
            }
            else if (choice == 2) {
                const auto& notes = activeNB->getNotes();
                if (notes.empty()) {
                    cout << " [INFO] This notebook folder is empty!\n";
                    continue;
                }
                
                cout << "\n --- Notes in '" << activeNB->getName() << "' ---\n";
                // First print Pinned notes
                for (const auto& note : notes) {
                    if (note.getIsPinned()) {
                        cout << "  [★ PINNED] ID: " << note.getId() << " | " << note.getTitle() << "\n";
                    }
                }
                // Then print Unpinned notes
                for (const auto& note : notes) {
                    if (!note.getIsPinned()) {
                        cout << "            ID: " << note.getId() << " | " << note.getTitle() << "\n";
                    }
                }
                printDivider();
            }
            else if (choice == 3) {
                int noteId;
                cout << "\n Enter Note ID to view: ";
                if (!(cin >> noteId)) {
                    cout << " [ERROR] Invalid ID format!\n";
                    clearInput();
                    continue;
                }
                clearInput();
                
                Note* note = activeNB->findNote(noteId);
                if (note != nullptr) {
                    note->display();
                } else {
                    cout << " [ERROR] Note with ID " << noteId << " not found in active notebook!\n";
                }
            }
            else if (choice == 4) {
                string query;
                cout << "\n Enter keyword to search inside '" << activeNB->getName() << "': ";
                getline(cin, query);
                
                vector<Note*> results = ns.searchNotes(query);
                if (results.empty()) {
                    cout << " [INFO] No matching notes found!\n";
                } else {
                    cout << "\n --- Notebook Search Results (" << results.size() << ") ---\n";
                    for (const auto* n : results) {
                        cout << "  ID: " << n->getId() 
                             << " | [" << n->getTitle() << "]" 
                             << (n->getIsPinned() ? " [★ PINNED]" : "") << "\n";
                    }
                    printDivider();
                }
            }
            else if (choice == 5) {
                string tag;
                cout << "\n Enter tag to filter inside '" << activeNB->getName() << "': ";
                getline(cin, tag);
                
                vector<Note*> results = ns.filterByTag(tag);
                if (results.empty()) {
                    cout << " [INFO] No notes found with tag: '" << tag << "'\n";
                } else {
                    cout << "\n --- Tag Filter Results (" << results.size() << ") ---\n";
                    for (const auto* n : results) {
                        cout << "  ID: " << n->getId() 
                             << " | [" << n->getTitle() << "]" 
                             << (n->getIsPinned() ? " [★ PINNED]" : "") << "\n";
                    }
                    printDivider();
                }
            }
            else if (choice == 6) {
                int noteId;
                cout << "\n Enter Note ID to edit: ";
                if (!(cin >> noteId)) {
                    cout << " [ERROR] Invalid ID format!\n";
                    clearInput();
                    continue;
                }
                clearInput();
                
                Note* note = activeNB->findNote(noteId);
                if (note == nullptr) {
                    cout << " [ERROR] Note with ID " << noteId << " not found in active notebook!\n";
                    continue;
                }
                
                string newTitle, newBody, newTagInput;
                cout << " Current Title: " << note->getTitle() << "\n";
                cout << " Enter New Title (press Enter to keep current): ";
                getline(cin, newTitle);
                if (newTitle.empty()) newTitle = note->getTitle();
                
                cout << " Current Content: " << note->getContent() << "\n";
                cout << " Enter New Content (press Enter to keep current, use '\\n' for line breaks): ";
                getline(cin, newBody);
                if (newBody.empty()) {
                    newBody = note->getContent();
                } else {
                    size_t nPos = 0;
                    while ((nPos = newBody.find("\\n")) != string::npos) {
                        newBody.replace(nPos, 2, "\n");
                    }
                }
                
                cout << " Enter New Tags (comma-separated, press Enter to keep current): ";
                getline(cin, newTagInput);
                
                vector<string> newTags;
                if (newTagInput.empty()) {
                    newTags = note->getTags();
                } else {
                    size_t pos = 0;
                    while ((pos = newTagInput.find(',')) != string::npos) {
                        string t = newTagInput.substr(0, pos);
                        size_t first = t.find_first_not_of(' ');
                        size_t last = t.find_last_not_of(' ');
                        if (first != string::npos && last != string::npos) t = t.substr(first, (last - first + 1));
                        if (!t.empty()) newTags.push_back(t);
                        newTagInput.erase(0, pos + 1);
                    }
                    size_t first = newTagInput.find_first_not_of(' ');
                    size_t last = newTagInput.find_last_not_of(' ');
                    if (first != string::npos && last != string::npos) newTagInput = newTagInput.substr(first, (last - first + 1));
                    if (!newTagInput.empty()) newTags.push_back(newTagInput);
                }
                
                note->update(newTitle, newBody, newTags);
                ns.saveToFile(dbFile);
                cout << " [SUCCESS] Note updated successfully!\n";
            }
            else if (choice == 7) {
                int noteId;
                cout << "\n Enter Note ID to pin/unpin: ";
                if (!(cin >> noteId)) {
                    cout << " [ERROR] Invalid ID format!\n";
                    clearInput();
                    continue;
                }
                clearInput();
                
                Note* note = activeNB->findNote(noteId);
                if (note != nullptr) {
                    note->setIsPinned(!note->getIsPinned());
                    ns.saveToFile(dbFile);
                    cout << " [SUCCESS] Note " << noteId << (note->getIsPinned() ? " PINNED!" : " UNPINNED!") << "\n";
                } else {
                    cout << " [ERROR] Note with ID " << noteId << " not found!\n";
                }
            }
            else if (choice == 8) {
                int noteId;
                cout << "\n Enter Note ID to delete: ";
                if (!(cin >> noteId)) {
                    cout << " [ERROR] Invalid ID format!\n";
                    clearInput();
                    continue;
                }
                clearInput();
                
                if (activeNB->deleteNote(noteId)) {
                    ns.saveToFile(dbFile);
                    cout << " [SUCCESS] Note with ID " << noteId << " has been deleted.\n";
                } else {
                    cout << " [ERROR] Note with ID " << noteId << " not found!\n";
                }
            }
            else if (choice == 9) {
                ns.selectNotebook(""); // Clears active notebook
                cout << " [SUCCESS] Returned to Notebook select menu.\n";
            }
            else if (choice == 10) {
                ns.logoutUser();
                cout << " [SUCCESS] Logged out successfully!\n";
            }
            else if (choice == 11) {
                ns.saveToFile(dbFile);
                cout << "\n Thank you for using NOTEIFY! Saving database... Goodbye!\n\n";
                break;
            }
            else {
                cout << " [WARNING] Invalid choice! Choose between 1 and 11.\n";
            }
        }
    }
    
    return 0;
}

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#else
void enableANSI() {}
#endif

#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <ctime>
#include <algorithm>
#include "NoteService.h"

// ANSI Color Code Constants
#define ANSI_CYAN    "\033[36m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_RED     "\033[31m"
#define ANSI_YELLOW  "\033[1;33m"
#define ANSI_RESET   "\033[0m"
#define ANSI_BOLD    "\033[1m"
#define ANSI_GRAY    "\033[90m"

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
    cout << ANSI_CYAN << "============================================================\n" << ANSI_RESET;
}

void printASCIIBanner() {
    cout << "\n";
    printDivider();
    cout << ANSI_CYAN << ANSI_BOLD;
    cout << "  _   _   ___ _____ _____ ___ _____   __  __  \n";
    cout << " | \\ | | / _ \\_   _| ____|_ _|  ___|  \\ \\/ /  \n";
    cout << " |  \\| || | | || | |  _|  | || |_      \\  /   \n";
    cout << " | |\\  || |_| || | | |___ | ||  _|     /  \\   \n";
    cout << " |_| \\_| \\___/ |_| |_____|___|_|      /_/\\_\\  \n";
    cout << "                                              \n";
    cout << "          C++ PREMIUM NOTE MANAGEMENT SYSTEM  \n" << ANSI_RESET;
    printDivider();
}

void printSessionInfo(const NoteService& ns) {
    if (ns.getCurrentUser() == nullptr) {
        cout << ANSI_GRAY << " [GUEST] Log in or Register to begin creating notes!\n" << ANSI_RESET;
    } else {
        cout << " Active User: " << ANSI_GREEN << ns.getCurrentUser()->getUsername() << ANSI_RESET 
             << " [ID: " << ANSI_YELLOW << ns.getCurrentUser()->getUserId() << ANSI_RESET << "]\n";
        
        if (ns.getCurrentNotebook() == nullptr) {
            cout << " Active Notebook Folder: " << ANSI_RED << "[NONE]" << ANSI_RESET << " (Select one first)\n";
        } else {
            // Count active notes in this notebook
            int activeNotesCount = 0;
            for (const auto& note : ns.getCurrentNotebook()->getNotes()) {
                if (!note.getIsArchived()) activeNotesCount++;
            }
            cout << " Active Notebook Folder: " << ANSI_GREEN << ns.getCurrentNotebook()->getName() << ANSI_RESET 
                 << " [" << ANSI_YELLOW << activeNotesCount << ANSI_RESET << " Active Note(s)]\n";
        }
    }
    printDivider();
}

int main() {
    enableANSI();
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
            cout << " 1. " << ANSI_CYAN << "Log In" << ANSI_RESET << " (Existing User)\n";
            cout << " 2. " << ANSI_GREEN << "Register" << ANSI_RESET << " New User\n";
            cout << " 3. " << ANSI_RED << "Exit" << ANSI_RESET << " Application\n\n";
            cout << " Enter choice (1-3): ";
            
            if (!(cin >> choice)) {
                if (cin.eof()) break;
                cout << "\n" << ANSI_RED << " [ERROR] Invalid Input! Please enter a number." << ANSI_RESET << "\n";
                clearInput();
                continue;
            }
            clearInput();
            
            if (choice == 1) {
                string name;
                cout << "\n Enter Username: ";
                getline(cin, name);
                
                if (name.empty()) {
                    cout << ANSI_RED << " [ERROR] Username cannot be empty!" << ANSI_RESET << "\n";
                    continue;
                }
                
                if (ns.loginUser(name)) {
                    cout << ANSI_GREEN << " [SUCCESS] Welcome back, " << name << "!" << ANSI_RESET << "\n";
                } else {
                    cout << ANSI_RED << " [ERROR] User not found! Register first." << ANSI_RESET << "\n";
                }
            }
            else if (choice == 2) {
                string name, id;
                cout << "\n Enter Username: ";
                getline(cin, name);
                cout << " Enter CIF/User ID (e.g. CIF001): ";
                getline(cin, id);
                
                if (name.empty() || id.empty()) {
                    cout << ANSI_RED << " [ERROR] Username and ID cannot be empty!" << ANSI_RESET << "\n";
                    continue;
                }
                
                if (ns.registerUser(name, id)) {
                    ns.saveToFile(dbFile);
                    cout << ANSI_GREEN << " [SUCCESS] User registered successfully! Logging you in..." << ANSI_RESET << "\n";
                    ns.loginUser(name);
                } else {
                    cout << ANSI_RED << " [ERROR] User already exists!" << ANSI_RESET << "\n";
                }
            }
            else if (choice == 3) {
                ns.saveToFile(dbFile);
                cout << "\n" << ANSI_GREEN << " Thank you for using NOTEIFY! Saving database... Goodbye!" << ANSI_RESET << "\n\n";
                break;
            }
            else {
                cout << ANSI_YELLOW << " [WARNING] Invalid choice! Choose between 1 and 3." << ANSI_RESET << "\n";
            }
        }
        
        // LEVEL 2: LOGGED IN USER, NO NOTEBOOK SELECTED
        else if (ns.getCurrentNotebook() == nullptr) {
            cout << " 1. " << ANSI_CYAN << "Create" << ANSI_RESET << " a Notebook Folder\n";
            cout << " 2. " << ANSI_CYAN << "Select/Switch" << ANSI_RESET << " Notebook Folder\n";
            cout << " 3. " << ANSI_YELLOW << "Search All Notes" << ANSI_RESET << " (Global)\n";
            cout << " 4. " << ANSI_YELLOW << "Filter All Notes by Tag" << ANSI_RESET << " (Global)\n";
            cout << " 5. " << ANSI_GREEN << "View Notebook System Analytics" << ANSI_RESET << "\n";
            cout << " 6. " << ANSI_RED << "Log Out" << ANSI_RESET << "\n";
            cout << " 7. " << ANSI_RED << "Exit Application" << ANSI_RESET << "\n\n";
            cout << " Enter choice (1-7): ";
            
            if (!(cin >> choice)) {
                if (cin.eof()) break;
                cout << "\n" << ANSI_RED << " [ERROR] Invalid Input!" << ANSI_RESET << "\n";
                clearInput();
                continue;
            }
            clearInput();
            
            if (choice == 1) {
                string folderName;
                cout << "\n Enter Notebook Name (e.g. CompSci, Personal): ";
                getline(cin, folderName);
                if (folderName.empty()) {
                    cout << ANSI_RED << " [ERROR] Notebook name cannot be empty!" << ANSI_RESET << "\n";
                    continue;
                }
                
                if (ns.createNotebook(folderName)) {
                    ns.saveToFile(dbFile);
                    cout << ANSI_GREEN << " [SUCCESS] Notebook '" << folderName << "' created successfully!" << ANSI_RESET << "\n";
                    ns.selectNotebook(folderName); // Auto select it
                } else {
                    cout << ANSI_RED << " [ERROR] Notebook folder already exists!" << ANSI_RESET << "\n";
                }
            }
            else if (choice == 2) {
                const auto& folders = ns.getNotebooks();
                if (folders.empty()) {
                    cout << ANSI_YELLOW << " [INFO] No notebooks found! Please create one first." << ANSI_RESET << "\n";
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
                    cout << ANSI_GREEN << " [SUCCESS] Switched to notebook '" << targetFolder << "'!" << ANSI_RESET << "\n";
                } else {
                    cout << ANSI_RED << " [ERROR] Notebook folder not found!" << ANSI_RESET << "\n";
                }
            }
            else if (choice == 3) {
                string query;
                cout << "\n Enter keyword to search: ";
                getline(cin, query);
                
                vector<Note*> results = ns.searchNotes(query);
                if (results.empty()) {
                    cout << ANSI_YELLOW << " [INFO] No notes found matching query: '" << query << "'" << ANSI_RESET << "\n";
                } else {
                    cout << "\n --- Search Results (" << results.size() << ") ---\n";
                    for (const auto* n : results) {
                        cout << "  ID: " << n->getId() 
                             << " | [" << n->getTitle() << "]" 
                             << (n->getIsPinned() ? ANSI_YELLOW " [★ PINNED]" ANSI_RESET : "") << "\n";
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
                    cout << ANSI_YELLOW << " [INFO] No notes found with tag: '" << tag << "'" << ANSI_RESET << "\n";
                } else {
                    cout << "\n --- Tag Filter Results (" << results.size() << ") ---\n";
                    for (const auto* n : results) {
                        cout << "  ID: " << n->getId() 
                             << " | [" << n->getTitle() << "]" 
                             << (n->getIsPinned() ? ANSI_YELLOW " [★ PINNED]" ANSI_RESET : "") << "\n";
                    }
                    printDivider();
                }
            }
            else if (choice == 5) {
                ns.displayAnalytics();
            }
            else if (choice == 6) {
                ns.logoutUser();
                cout << ANSI_GREEN << " [SUCCESS] Logged out successfully!" << ANSI_RESET << "\n";
            }
            else if (choice == 7) {
                ns.saveToFile(dbFile);
                cout << "\n" << ANSI_GREEN << " Thank you for using NOTEIFY! Saving database... Goodbye!" << ANSI_RESET << "\n\n";
                break;
            }
            else {
                cout << ANSI_YELLOW << " [WARNING] Invalid choice! Choose between 1 and 7." << ANSI_RESET << "\n";
            }
        }
        
        // LEVEL 3: LOGGED IN USER, NOTEBOOK ACTIVE
        else {
            cout << " 1. " << ANSI_CYAN << "Create" << ANSI_RESET << " a Note\n";
            cout << " 2. " << ANSI_CYAN << "View All" << ANSI_RESET << " Notes in Folder\n";
            cout << " 3. " << ANSI_CYAN << "View Full Note" << ANSI_RESET << " details (by ID)\n";
            cout << " 4. " << ANSI_YELLOW << "Search Notes" << ANSI_RESET << " (This Notebook)\n";
            cout << " 5. " << ANSI_YELLOW << "Filter Notes by Tag" << ANSI_RESET << " (This Notebook)\n";
            cout << " 6. " << ANSI_CYAN << "Edit" << ANSI_RESET << " an existing Note\n";
            cout << " 7. " << ANSI_YELLOW << "Toggle Note Pin" << ANSI_RESET << " (Priority)\n";
            cout << " 8. " << ANSI_RED << "Move Note to Trash" << ANSI_RESET << " (Archive)\n";
            cout << " 9. " << ANSI_GREEN << "Open Recycle Bin" << ANSI_RESET << " (Trash Dashboard)\n";
            cout << " 10. " << ANSI_GREEN << "Export a Note" << ANSI_RESET << " to Text File\n";
            cout << " 11. " << ANSI_CYAN << "Back" << ANSI_RESET << " to Notebook selection\n";
            cout << " 12. " << ANSI_RED << "Log Out" << ANSI_RESET << "\n";
            cout << " 13. " << ANSI_RED << "Exit Application" << ANSI_RESET << "\n\n";
            cout << " Enter choice (1-13): ";
            
            if (!(cin >> choice)) {
                if (cin.eof()) break;
                cout << "\n" << ANSI_RED << " [ERROR] Invalid Input!" << ANSI_RESET << "\n";
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
                cout << ANSI_GREEN << " [SUCCESS] Note created successfully with ID: " << newNote.getId() << ANSI_RESET << "\n";
            }
            else if (choice == 2) {
                const auto& notes = activeNB->getNotes();
                bool hasActiveNotes = false;
                for (const auto& note : notes) {
                    if (!note.getIsArchived()) {
                        hasActiveNotes = true;
                        break;
                    }
                }
                
                if (!hasActiveNotes) {
                    cout << ANSI_YELLOW << " [INFO] This notebook folder has no active notes." << ANSI_RESET << "\n";
                    continue;
                }
                
                cout << "\n --- Notes in '" << activeNB->getName() << "' ---\n";
                // First print Pinned notes
                for (const auto& note : notes) {
                    if (!note.getIsArchived() && note.getIsPinned()) {
                        cout << "  " << ANSI_YELLOW << "[★ PINNED]" << ANSI_RESET << " ID: " << note.getId() << " | " << note.getTitle() << "\n";
                    }
                }
                // Then print Unpinned notes
                for (const auto& note : notes) {
                    if (!note.getIsArchived() && !note.getIsPinned()) {
                        cout << "            ID: " << note.getId() << " | " << note.getTitle() << "\n";
                    }
                }
                printDivider();
            }
            else if (choice == 3) {
                int noteId;
                cout << "\n Enter Note ID to view: ";
                if (!(cin >> noteId)) {
                    cout << ANSI_RED << " [ERROR] Invalid ID format!" << ANSI_RESET << "\n";
                    clearInput();
                    continue;
                }
                clearInput();
                
                Note* note = activeNB->findNote(noteId);
                // Can view note even if archived, but display() will format it correctly
                if (note != nullptr) {
                    note->display();
                } else {
                    cout << ANSI_RED << " [ERROR] Note with ID " << noteId << " not found in active notebook!" << ANSI_RESET << "\n";
                }
            }
            else if (choice == 4) {
                string query;
                cout << "\n Enter keyword to search inside '" << activeNB->getName() << "': ";
                getline(cin, query);
                
                vector<Note*> results = ns.searchNotes(query);
                if (results.empty()) {
                    cout << ANSI_YELLOW << " [INFO] No matching notes found!" << ANSI_RESET << "\n";
                } else {
                    cout << "\n --- Notebook Search Results (" << results.size() << ") ---\n";
                    for (const auto* n : results) {
                        cout << "  ID: " << n->getId() 
                             << " | [" << n->getTitle() << "]" 
                             << (n->getIsPinned() ? ANSI_YELLOW " [★ PINNED]" ANSI_RESET : "") << "\n";
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
                    cout << ANSI_YELLOW << " [INFO] No notes found with tag: '" << tag << "'" << ANSI_RESET << "\n";
                } else {
                    cout << "\n --- Tag Filter Results (" << results.size() << ") ---\n";
                    for (const auto* n : results) {
                        cout << "  ID: " << n->getId() 
                             << " | [" << n->getTitle() << "]" 
                             << (n->getIsPinned() ? ANSI_YELLOW " [★ PINNED]" ANSI_RESET : "") << "\n";
                    }
                    printDivider();
                }
            }
            else if (choice == 6) {
                int noteId;
                cout << "\n Enter Note ID to edit: ";
                if (!(cin >> noteId)) {
                    cout << ANSI_RED << " [ERROR] Invalid ID format!" << ANSI_RESET << "\n";
                    clearInput();
                    continue;
                }
                clearInput();
                
                Note* note = activeNB->findNote(noteId);
                if (note == nullptr || note->getIsArchived()) {
                    cout << ANSI_RED << " [ERROR] Active note with ID " << noteId << " not found in active notebook!" << ANSI_RESET << "\n";
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
                cout << ANSI_GREEN << " [SUCCESS] Note updated successfully!" << ANSI_RESET << "\n";
            }
            else if (choice == 7) {
                int noteId;
                cout << "\n Enter Note ID to pin/unpin: ";
                if (!(cin >> noteId)) {
                    cout << ANSI_RED << " [ERROR] Invalid ID format!" << ANSI_RESET << "\n";
                    clearInput();
                    continue;
                }
                clearInput();
                
                Note* note = activeNB->findNote(noteId);
                if (note != nullptr && !note->getIsArchived()) {
                    note->setIsPinned(!note->getIsPinned());
                    ns.saveToFile(dbFile);
                    cout << ANSI_GREEN << " [SUCCESS] Note " << noteId << (note->getIsPinned() ? " PINNED!" : " UNPINNED!") << ANSI_RESET << "\n";
                } else {
                    cout << ANSI_RED << " [ERROR] Active note with ID " << noteId << " not found!" << ANSI_RESET << "\n";
                }
            }
            else if (choice == 8) {
                int noteId;
                cout << "\n Enter Note ID to move to Recycle Bin: ";
                if (!(cin >> noteId)) {
                    cout << ANSI_RED << " [ERROR] Invalid ID format!" << ANSI_RESET << "\n";
                    clearInput();
                    continue;
                }
                clearInput();
                
                Note* note = activeNB->findNote(noteId);
                if (note != nullptr && !note->getIsArchived()) {
                    activeNB->archiveNote(noteId);
                    ns.saveToFile(dbFile);
                    cout << ANSI_GREEN << " [SUCCESS] Note with ID " << noteId << " has been moved to Recycle Bin (Trash)." << ANSI_RESET << "\n";
                } else {
                    cout << ANSI_RED << " [ERROR] Note with ID " << noteId << " not found!" << ANSI_RESET << "\n";
                }
            }
            else if (choice == 9) {
                // Recycle Bin Sub-menu
                int trashChoice = 0;
                while (true) {
                    printDivider();
                    cout << ANSI_CYAN << " ======= RECYCLE BIN (TRASH DASHBOARD) =======\n" << ANSI_RESET;
                    cout << " 1. List Trash Notes\n";
                    cout << " 2. Restore a Trash Note\n";
                    cout << " 3. Permanently Delete a Note\n";
                    cout << " 4. Empty Recycle Bin\n";
                    cout << " 5. Return to Notebook\n\n";
                    cout << " Enter choice (1-5): ";
                    
                    if (!(cin >> trashChoice)) {
                        cout << ANSI_RED << " [ERROR] Invalid Input!" << ANSI_RESET << "\n";
                        clearInput();
                        continue;
                    }
                    clearInput();
                    
                    if (trashChoice == 1) {
                        const auto& notes = activeNB->getNotes();
                        bool found = false;
                        cout << "\n --- Archived Notes in '" << activeNB->getName() << "' Trash ---\n";
                        for (const auto& note : notes) {
                            if (note.getIsArchived()) {
                                cout << "  ID: " << note.getId() << " | " << note.getTitle() << "\n";
                                found = true;
                            }
                        }
                        if (!found) {
                            cout << ANSI_YELLOW << " [INFO] Recycle bin is empty!" << ANSI_RESET << "\n";
                        }
                        printDivider();
                    }
                    else if (trashChoice == 2) {
                        int noteId;
                        cout << "\n Enter Note ID to restore: ";
                        if (!(cin >> noteId)) {
                            cout << ANSI_RED << " [ERROR] Invalid ID format!" << ANSI_RESET << "\n";
                            clearInput();
                            continue;
                        }
                        clearInput();
                        
                        Note* note = activeNB->findNote(noteId);
                        if (note != nullptr && note->getIsArchived()) {
                            activeNB->restoreNote(noteId);
                            ns.saveToFile(dbFile);
                            cout << ANSI_GREEN << " [SUCCESS] Note restored successfully!" << ANSI_RESET << "\n";
                        } else {
                            cout << ANSI_RED << " [ERROR] Note ID not found in trash!" << ANSI_RESET << "\n";
                        }
                    }
                    else if (trashChoice == 3) {
                        int noteId;
                        cout << "\n Enter Note ID to permanently delete: ";
                        if (!(cin >> noteId)) {
                            cout << ANSI_RED << " [ERROR] Invalid ID format!" << ANSI_RESET << "\n";
                            clearInput();
                            continue;
                        }
                        clearInput();
                        
                        Note* note = activeNB->findNote(noteId);
                        if (note != nullptr && note->getIsArchived()) {
                            activeNB->deleteNote(noteId);
                            ns.saveToFile(dbFile);
                            cout << ANSI_GREEN << " [SUCCESS] Note with ID " << noteId << " permanently deleted." << ANSI_RESET << "\n";
                        } else {
                            cout << ANSI_RED << " [ERROR] Note ID not found in trash!" << ANSI_RESET << "\n";
                        }
                    }
                    else if (trashChoice == 4) {
                        auto& notes = activeNB->getNotes();
                        int initialSize = notes.size();
                        notes.erase(remove_if(notes.begin(), notes.end(), [](const Note& n) {
                            return n.getIsArchived();
                        }), notes.end());
                        
                        int deletedCount = initialSize - notes.size();
                        if (deletedCount > 0) {
                            ns.saveToFile(dbFile);
                            cout << ANSI_GREEN << " [SUCCESS] Emptied Recycle Bin! Permanently deleted " << deletedCount << " note(s)." << ANSI_RESET << "\n";
                        } else {
                            cout << ANSI_YELLOW << " [INFO] Recycle bin is already empty!" << ANSI_RESET << "\n";
                        }
                    }
                    else if (trashChoice == 5) {
                        cout << " Returning to Notebook menu...\n";
                        break;
                    }
                    else {
                        cout << ANSI_YELLOW << " [WARNING] Invalid choice! Choose between 1 and 5." << ANSI_RESET << "\n";
                    }
                }
            }
            else if (choice == 10) {
                int noteId;
                cout << "\n Enter Note ID to export: ";
                if (!(cin >> noteId)) {
                    cout << ANSI_RED << " [ERROR] Invalid ID format!" << ANSI_RESET << "\n";
                    clearInput();
                    continue;
                }
                clearInput();
                
                if (ns.exportNoteToTxt(noteId, activeNB->getName())) {
                    cout << ANSI_GREEN << " [SUCCESS] Note exported successfully to text file!" << ANSI_RESET << "\n";
                } else {
                    cout << ANSI_RED << " [ERROR] Note with ID " << noteId << " not found in active notebook!" << ANSI_RESET << "\n";
                }
            }
            else if (choice == 11) {
                ns.selectNotebook(""); // Clears active notebook
                cout << ANSI_GREEN << " [SUCCESS] Returned to Notebook select menu." << ANSI_RESET << "\n";
            }
            else if (choice == 12) {
                ns.logoutUser();
                cout << ANSI_GREEN << " [SUCCESS] Logged out successfully!" << ANSI_RESET << "\n";
            }
            else if (choice == 13) {
                ns.saveToFile(dbFile);
                cout << "\n" << ANSI_GREEN << " Thank you for using NOTEIFY! Saving database... Goodbye!" << ANSI_RESET << "\n\n";
                break;
            }
            else {
                cout << ANSI_YELLOW << " [WARNING] Invalid choice! Choose between 1 and 13." << ANSI_RESET << "\n";
            }
        }
    }
    
    return 0;
}

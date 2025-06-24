#include <iostream>
#include <string>
#include <fstream>
#include "playlist.h"

std::string get_current_playlist() {
    std::ifstream file("current_playlist.txt");
    if (!file.is_open()) return "";
    std::string name;
    std::getline(file, name);
    return name;
}

void set_current_playlist(const std::string& name) {
    std::ofstream file("current_playlist.txt");
    if (file.is_open()) {
        file << name;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: app2.exe <command> [args]\n";
        return 1;
    }

    std::string command = argv[1];
    PlaylistManager manager;
    manager.load_from_file("file.txt");

    if (command == "manage_playlist") {
        if (argc != 3) {
            std::cout << "Usage: app2.exe manage_playlist <name>\n";
            return 1;
        }
        std::string name = argv[2];
        const Playlist* pl = manager.find_playlist(name);
        if (!pl) {
            std::cout << "Playlist '" << name << "' not found.\n";
            return 1;
        }
        set_current_playlist(name);
        std::cout << "Now managing playlist: " << name << "\n";
    } else if (command == "view_songs") {
        std::string playlist_name = get_current_playlist();
        if (playlist_name.empty()) {
            std::cout << "No playlist selected. Use manage_playlist <name> first.\n";
            return 1;
        }
        manager.view_songs_in_playlist(playlist_name);
    } else if (command == "add_song") {
        if (argc != 4) {
            std::cout << "Usage: app2.exe add_song <title> <artist>\n";
            return 1;
        }
        std::string playlist_name = get_current_playlist();
        if (playlist_name.empty()) {
            std::cout << "No playlist selected. Use manage_playlist <name> first.\n";
            return 1;
        }
        std::string title = argv[2];
        std::string artist = argv[3];
        manager.add_song_to_playlist(playlist_name, title, artist);
        manager.save_to_file("file.txt");
    } else if (command == "delete_song") {
        if (argc != 3) {
            std::cout << "Usage: app2.exe delete_song <title>\n";
            return 1;
        }
        std::string playlist_name = get_current_playlist();
        if (playlist_name.empty()) {
            std::cout << "No playlist selected. Use manage_playlist <name> first.\n";
            return 1;
        }
        std::string title = argv[2];
        manager.remove_song_from_playlist(playlist_name, title);
        manager.save_to_file("file.txt");
    } else if (command == "exit") {
        std::ofstream file("current_playlist.txt", std::ios::trunc); // Clear current playlist
        manager.save_to_file("file.txt");
    } else {
        std::cout << "Unknown command.\n";
        return 1;
    }

    return 0;
}
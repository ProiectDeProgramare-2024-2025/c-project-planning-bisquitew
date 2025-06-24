#include <iostream>
#include <string>
#include "playlist.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: app1.exe <command> [args]\n";
        return 1;
    }

    std::string command = argv[1];
    PlaylistManager manager;
    manager.load_from_file("file.txt");

    if (command == "view_playlists") {
        manager.view_playlists();
    } else if (command == "create_playlist") {
        if (argc != 4) {
            std::cout << "Usage: app1.exe create_playlist <name> <rating>\n";
            return 1;
        }
        std::string name = argv[2];
        float rating;
        try {
            rating = std::stof(argv[3]);
        } catch (...) {
            std::cout << "Invalid rating.\n";
            return 1;
        }
        manager.create_playlist(name, rating);
        manager.save_to_file("file.txt");
    } else if (command == "delete_playlist") {
        if (argc != 3) {
            std::cout << "Usage: app1.exe delete_playlist <name>\n";
            return 1;
        }
        std::string name = argv[2];
        manager.delete_playlist_by_name(name);
        manager.save_to_file("file.txt");
    } else if (command == "exit") {
        manager.save_to_file("file.txt");
    } else {
        std::cout << "Unknown command.\n";
        return 1;
    }

    return 0;
}
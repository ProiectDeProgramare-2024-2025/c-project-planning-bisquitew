#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>
#include "playlist.h"

void menu_clear() {
#ifdef _WIN32
    std::system("CLS");
#else
    std::system("clear");
#endif
}

void Playlist::add_song() {
    std::string title, artist;
    std::cout << "\nEnter song title: ";
    std::getline(std::cin, title);
    if (title.empty()) {
        std::cout << COLOR_RED << "\nError: Song title cannot be empty!" << COLOR_RESET << "\n";
        return;
    }

    std::cout << "Enter artist: ";
    std::getline(std::cin, artist);
    if (artist.empty()) {
        std::cout << COLOR_RED << "\nError: Artist name cannot be empty!" << COLOR_RESET << "\n";
        return;
    }

    songs.emplace_back(title, artist);
    std::cout << COLOR_GREEN << "\n'" << title << "' by " << artist << " added to '" << name << "'!" << COLOR_RESET << "\n";
}

void Playlist::remove_song() {
    if (songs.empty()) {
        std::cout << "No songs in this playlist.\n";
        return;
    }

    std::vector<Song> sorted_songs = songs;
    std::sort(sorted_songs.begin(), sorted_songs.end(),
              [](const Song& a, const Song& b) { return a.title < b.title; });

    std::cout << "Songs in " << name << ":\n";
    for (size_t i = 0; i < sorted_songs.size(); ++i) {
        std::cout << i + 1 << ". " << sorted_songs[i].title << " by " << sorted_songs[i].artist << "\n";
    }

    int choice;
    std::cout << "Enter the number of the song to remove: ";
    std::cin >> choice;
    std::cin.ignore();

    if (choice < 1 || choice > static_cast<int>(sorted_songs.size())) {
        std::cout << "Invalid choice.\n";
        return;
    }

    const Song& selected = sorted_songs[choice - 1];
    auto it = std::find_if(songs.begin(), songs.end(),
                           [&selected](const Song& s) { return s.title == selected.title && s.artist == selected.artist; });
    if (it != songs.end()) {
        songs.erase(it);
        std::cout << "Song removed.\n";
    }
}

void Playlist::view_songs() const {
    if (songs.empty()) {
        std::cout << "No songs in this playlist.\n";
    } else {
        std::vector<Song> sorted_songs = songs;
        std::sort(sorted_songs.begin(), sorted_songs.end(),
                  [](const Song& a, const Song& b) { return a.title < b.title; });

        std::cout << "Songs in " << name << ":\n";
        for (size_t i = 0; i < sorted_songs.size(); ++i) {
            std::cout << i + 1 << ". " << sorted_songs[i].title << " by " << sorted_songs[i].artist << "\n";
        }
    }
}

bool PlaylistManager::playlist_exists(const std::string& name) const {
    for (const auto& pl : playlists) {
        if (pl.name == name) return true;
    }
    return false;
}

Playlist* PlaylistManager::select_playlist() {
    if (playlists.empty()) {
        std::cout << "No playlists available.\n";
        return nullptr;
    }

    std::vector<Playlist*> sorted_playlists;
    for (auto& pl : playlists) {
        sorted_playlists.push_back(&pl);
    }
    std::sort(sorted_playlists.begin(), sorted_playlists.end(),
              [](const Playlist* a, const Playlist* b) { return a->rating > b->rating; });

    std::cout << "\n" << COLOR_CYAN << "=== Available Playlists ===" << COLOR_RESET << "\n";
    for (size_t i = 0; i < sorted_playlists.size(); ++i) {
        std::cout << i + 1 << ". " << COLOR_GREEN << sorted_playlists[i]->name << COLOR_RESET
                  << " - Rating: " << COLOR_YELLOW << sorted_playlists[i]->rating << COLOR_RESET << "\n";
    }

    int choice;
    std::cout << "Enter the number of the playlist: ";
    std::cin >> choice;
    std::cin.ignore();

    if (choice < 1 || choice > static_cast<int>(sorted_playlists.size())) {
        std::cout << "Invalid choice.\n";
        return nullptr;
    }
    return sorted_playlists[choice - 1];
}

void PlaylistManager::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') {
            size_t comma_pos = line.find(',');
            if (comma_pos != std::string::npos) {
                std::string name = line.substr(1, comma_pos - 1);
                float rating = std::stof(line.substr(comma_pos + 1));
                playlists.emplace_back(name, rating);
            }
        } else if (!playlists.empty()) {
            size_t comma_pos = line.find(',');
            if (comma_pos != std::string::npos) {
                std::string title = line.substr(0, comma_pos);
                std::string artist = line.substr(comma_pos + 1);
                playlists.back().songs.emplace_back(title, artist);
            }
        }
    }
}

void PlaylistManager::save_to_file(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error saving to file.txt\n";
        return;
    }

    for (const auto& playlist : playlists) {
        file << "#" << playlist.name << "," << playlist.rating << "\n";
        for (const auto& song : playlist.songs) {
            file << song.title << "," << song.artist << "\n";
        }
        file << "\n";
    }
}

void PlaylistManager::create_playlist() {
    std::string name;
    std::cout << "Enter playlist name (max 99 characters): ";
    std::getline(std::cin, name);
    if (name.length() > 99) name = name.substr(0, 99);

    if (playlist_exists(name)) {
        std::cout << COLOR_RED << "\nError: Playlist '" << name << "' already exists!" << COLOR_RESET << "\n";
        return;
    }

    std::string rating_str;
    float rating;
    std::cout << "Enter rating (0.0 to 5.0): ";
    std::getline(std::cin, rating_str);
    try {
        rating = std::stof(rating_str);
        if (rating < 0 || rating > 5) throw std::out_of_range("Rating out of bounds");
    } catch (...) {
        std::cout << COLOR_RED << "\nInvalid rating! Please enter a number between 0.0 and 5.0." << COLOR_RESET << "\n";
        return;
    }

    playlists.emplace_back(name, rating);
    std::cout << COLOR_GREEN << "\nPlaylist '" << name << "' created successfully!" << COLOR_RESET << "\n";
}

void PlaylistManager::view_playlists() const {
    if (playlists.empty()) {
        std::cout << COLOR_YELLOW << "No playlists available." << COLOR_RESET << "\n";
    } else {
        std::vector<const Playlist*> sorted_playlists;
        for (const auto& pl : playlists) {
            sorted_playlists.push_back(&pl);
        }
        std::sort(sorted_playlists.begin(), sorted_playlists.end(),
                  [](const Playlist* a, const Playlist* b) { return a->rating > b->rating; });

        std::cout << "\n" << COLOR_CYAN << "=== Playlists sorted by rating ===" << COLOR_RESET << "\n";
        for (size_t i = 0; i < sorted_playlists.size(); ++i) {
            std::cout << i + 1 << ". " << COLOR_GREEN << sorted_playlists[i]->name << COLOR_RESET
                      << " - Rating: " << COLOR_YELLOW << sorted_playlists[i]->rating << COLOR_RESET << "\n";
        }
    }
}

void PlaylistManager::delete_playlist() {
    Playlist* selected = select_playlist();
    if (!selected) return;

    std::cout << COLOR_RED << "\nAre you sure you want to delete '" << selected->name << "'? (y/n): " << COLOR_RESET;
    char confirm;
    std::cin >> confirm;
    std::cin.ignore();

    if (confirm != 'y' && confirm != 'Y') {
        std::cout << "Deletion canceled.\n";
        return;
    }

    auto it = std::find_if(playlists.begin(), playlists.end(),
                           [selected](const Playlist& p) { return &p == selected; });
    if (it != playlists.end()) {
        playlists.erase(it);
        std::cout << COLOR_GREEN << "\nPlaylist '" << selected->name << "' deleted successfully!" << COLOR_RESET << "\n";
    }
}

void PlaylistManager::manage_playlist() {
    Playlist* selected = select_playlist();
    if (!selected) return;

    int choice;
    do {
        menu_clear();
        std::cout << "\n" << COLOR_ORANGE << "Songs submenu" << COLOR_RESET << "\n";
        std::cout << "Manage " << selected->name << ":\n";
        std::cout << "1. Add song\n";
        std::cout << "2. View songs\n";
        std::cout << "3. Delete song\n";
        std::cout << "4. Back to main menu\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1:
                selected->add_song();
                break;
            case 2:
                selected->view_songs();
                break;
            case 3:
                selected->remove_song();
                break;
            case 4:
                menu_clear();
                break;
            default:
                std::cout << "Invalid choice\n";
        }
    } while (choice != 4);
}

// New method implementations
const Playlist* PlaylistManager::find_playlist(const std::string& name) const {
    for (const auto& pl : playlists) {
        if (pl.name == name) return &pl;
    }
    return nullptr;
}

void PlaylistManager::create_playlist(const std::string& name, float rating) {
    if (playlist_exists(name)) {
        std::cout << COLOR_RED << "Error: Playlist '" << name << "' already exists!" << COLOR_RESET << "\n";
        return;
    }
    if (rating < 0 || rating > 5) {
        std::cout << COLOR_RED << "Invalid rating! Must be between 0.0 and 5.0." << COLOR_RESET << "\n";
        return;
    }
    playlists.emplace_back(name, rating);
    std::cout << COLOR_GREEN << "Playlist '" << name << "' created successfully!" << COLOR_RESET << "\n";
}

void PlaylistManager::delete_playlist_by_name(const std::string& name) {
    auto it = std::find_if(playlists.begin(), playlists.end(),
                           [&name](const Playlist& p) { return p.name == name; });
    if (it != playlists.end()) {
        playlists.erase(it);
        std::cout << COLOR_GREEN << "Playlist '" << name << "' deleted successfully!" << COLOR_RESET << "\n";
    } else {
        std::cout << COLOR_RED << "Playlist '" << name << "' not found." << COLOR_RESET << "\n";
    }
}

void PlaylistManager::add_song_to_playlist(const std::string& playlist_name, const std::string& title, const std::string& artist) {
    Playlist* pl = const_cast<Playlist*>(find_playlist(playlist_name));
    if (!pl) {
        std::cout << COLOR_RED << "Playlist '" << playlist_name << "' not found." << COLOR_RESET << "\n";
        return;
    }
    if (title.empty() || artist.empty()) {
        std::cout << COLOR_RED << "Error: Song title and artist cannot be empty!" << COLOR_RESET << "\n";
        return;
    }
    pl->songs.emplace_back(title, artist);
    std::cout << COLOR_GREEN << "'" << title << "' by " << artist << " added to '" << playlist_name << "'!" << COLOR_RESET << "\n";
}

void PlaylistManager::remove_song_from_playlist(const std::string& playlist_name, const std::string& title) {
    Playlist* pl = const_cast<Playlist*>(find_playlist(playlist_name));
    if (!pl) {
        std::cout << COLOR_RED << "Playlist '" << playlist_name << "' not found." << COLOR_RESET << "\n";
        return;
    }
    auto it = std::find_if(pl->songs.begin(), pl->songs.end(),
                           [&title](const Song& s) { return s.title == title; });
    if (it != pl->songs.end()) {
        pl->songs.erase(it);
        std::cout << "Song '" << title << "' removed from '" << playlist_name << "'.\n";
    } else {
        std::cout << "Song '" << title << "' not found in '" << playlist_name << "'.\n";
    }
}

void PlaylistManager::view_songs_in_playlist(const std::string& playlist_name) const {
    const Playlist* pl = find_playlist(playlist_name);
    if (!pl) {
        std::cout << COLOR_RED << "Playlist '" << playlist_name << "' not found." << COLOR_RESET << "\n";
        return;
    }
    if (pl->songs.empty()) {
        std::cout << "No songs in this playlist.\n";
    } else {
        std::vector<Song> sorted_songs = pl->songs;
        std::sort(sorted_songs.begin(), sorted_songs.end(),
                  [](const Song& a, const Song& b) { return a.title < b.title; });
        std::cout << "Songs in " << pl->name << ":\n";
        for (size_t i = 0; i < sorted_songs.size(); ++i) {
            std::cout << i + 1 << ". " << sorted_songs[i].title << " by " << sorted_songs[i].artist << "\n";
        }
    }
}
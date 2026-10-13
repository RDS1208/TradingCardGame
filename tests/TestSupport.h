#pragma once

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

// Gives each test its own deck file and restores the original directory.
class TestDeckDirectory {
public:
    explicit TestDeckDirectory(const std::vector<std::string>& records,
                               const std::string& deckPath = "deck.txt")
        : originalDirectory(std::filesystem::current_path()) {
        int directoryNumber = 0;
        // Creating the directory reserves it for this test.
        do {
            testDirectory = std::filesystem::temp_directory_path()
                / ("tcg-test-" + std::to_string(directoryNumber));
            ++directoryNumber;
        } while (!std::filesystem::create_directory(testDirectory));

        try {
            const auto deckFile = testDirectory / deckPath;
            std::filesystem::create_directories(deckFile.parent_path());
            std::ofstream deck(deckFile);
            for (const std::string& record : records) {
                deck << record << '\n';
            }
            deck.close();
            if (!deck) throw std::runtime_error("Could not write the test deck");
            std::filesystem::current_path(testDirectory);
        } catch (...) {
            std::filesystem::remove_all(testDirectory);
            throw;
        }
    }

    TestDeckDirectory(const TestDeckDirectory&) = delete;
    TestDeckDirectory& operator=(const TestDeckDirectory&) = delete;

    ~TestDeckDirectory() {
        // Cleanup must not throw from a destructor.
        std::error_code error;
        std::filesystem::current_path(originalDirectory, error);
        std::filesystem::remove_all(testDirectory, error);
    }

private:
    std::filesystem::path originalDirectory;
    std::filesystem::path testDirectory;
};

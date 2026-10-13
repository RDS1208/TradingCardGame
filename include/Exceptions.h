#pragma once
#ifndef TRADINGCARDGAME_EXCEPTIONS_H
#define TRADINGCARDGAME_EXCEPTIONS_H

#include <exception>
#include <cstddef>
#include <string>
#include <iostream>
#include <utility>

class InputEndedException : public std::exception {
public:
    const char* what() const noexcept override { return "Input ended."; }
};

// Common base for game exceptions.
class GameExceptionBase : public std::exception {
protected:
    std::string message;

public:
    explicit GameExceptionBase(const std::string& message) : message(message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

template <typename T>
class GameException : public GameExceptionBase {
protected:
    T details;

public:
    GameException(const std::string& message, T details)
        : GameExceptionBase(message), details(std::move(details)) {}

    T getDetails() const { return details; }

    template <typename U>
    friend void printExceptionDetails(const GameException<U>& ex);
};

// Prints the typed details stored by a game exception.
template <typename T>
void printExceptionDetails(const GameException<T>& ex) {
    std::cout << "  Details: " << ex.details << "\n";
}

class DeckLoadException : public GameException<std::string> {
public:
    explicit DeckLoadException(const std::string& file)
        : GameException<std::string>("Could not open deck file: " + file, file) {}

    DeckLoadException(const std::string& file, const std::string& reason)
        : GameException<std::string>("Could not load deck " + file + ": " + reason, file) {}

    DeckLoadException(const std::string& file, std::size_t line, const std::string& reason)
        : GameException<std::string>(
              "Invalid record in " + file + ":" + std::to_string(line) + ": " + reason,
              file + ":" + std::to_string(line)) {}
};

class InvalidDeckRecordException : public GameException<std::string> {
public:
    explicit InvalidDeckRecordException(const std::string& reason)
        : GameException<std::string>("Invalid deck record: " + reason, reason) {}
};

class InvalidCardStatException : public GameExceptionBase {
public:
    InvalidCardStatException(const std::string& cardName, unsigned manaCost)
        : GameExceptionBase("Card '" + cardName + "' has an invalid mana cost: "
                            + std::to_string(manaCost)) {}

    InvalidCardStatException(const std::string& cardName, const std::string& statName, int value)
        : GameExceptionBase("Card '" + cardName + "' has invalid " + statName + ": "
                            + std::to_string(value)) {}
};

class NotEnoughManaException : public GameException<unsigned> {
public:
    NotEnoughManaException(const std::string& cardName, unsigned manaCost)
        : GameException<unsigned>("Not enough mana to play '" + cardName + "'.", manaCost) {}
};

#endif // TRADINGCARDGAME_EXCEPTIONS_H

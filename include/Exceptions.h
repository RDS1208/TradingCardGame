#pragma once
#ifndef TRADINGCARDGAME_EXCEPTIONS_H
#define TRADINGCARDGAME_EXCEPTIONS_H

#include <exception>
#include <string>
#include <iostream>

// Clasa de baza simpla (non-template). Ne permite sa prindem TOATE erorile jocului
// intr-un singur loc folosind 'catch (const GameExceptionBase& e)'.
class GameExceptionBase : public std::exception {
protected:
    std::string mesaj;

public:
    explicit GameExceptionBase(const std::string& msg) : mesaj(msg) {}

    const char* what() const override {
        return mesaj.c_str();
    }
};

// Clasa sablon (template): 'template <typename T>' inseamna ca aceasta clasa
// poate stoca orice tip de date pentru detalii (T poate fi un text/string sau un numar/unsigned),
// fara sa fim nevoiti sa scriem o clasa complet noua pentru fiecare tip.
template <typename T>
class GameException : public GameExceptionBase {
protected:
    T detalii;

public:
    GameException(const std::string& msg, T det) : GameExceptionBase(msg), detalii(det) {}

    T getDetalii() const { return detalii; }

    // Declararea ca 'friend' (prieten) ii ofera functiei de mai jos permisiunea speciala
    // de a accesa direct variabila protejata 'detalii'.
    template <typename U>
    friend void printExceptionDetails(const GameException<U>& ex);
};

// Functie sablon (template) libera care afiseaza detaliile tehnice ale oricarei exceptii.
template <typename T>
void printExceptionDetails(const GameException<T>& ex) {
    std::cout << "  [Detalii tehnice]: " << ex.detalii << "\n";
}

class DeckLoadException : public GameException<std::string> {
public:
    explicit DeckLoadException(const std::string& fisier)
        : GameException<std::string>("Nu am putut deschide fisierul " + fisier, fisier) {}
};

class InvalidCardStatException : public GameException<unsigned> {
public:
    InvalidCardStatException(const std::string& numeCarte, unsigned manaCost)
        : GameException<unsigned>("Cartea '" + numeCarte + "' are cost de mana invalid!", manaCost) {}
};

class NotEnoughManaException : public GameException<unsigned> {
public:
    NotEnoughManaException(const std::string& numeCarte, unsigned costMana)
        : GameException<unsigned>("Nu ai suficienta mana pentru a juca '" + numeCarte + "'!", costMana) {}
};

#endif // TRADINGCARDGAME_EXCEPTIONS_H

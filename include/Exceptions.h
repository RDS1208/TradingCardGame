#pragma once
#ifndef TRADINGCARDGAME_EXCEPTIONS_H
#define TRADINGCARDGAME_EXCEPTIONS_H

#include <exception>
#include <string>
// Clasa de bază pentru toate erorile din jocul nostru.
// Moștenește din std::exception ca să poată fi prinsă cu catch(const std::exception&).
class GameException : public std::exception {
protected:
    std::string mesaj; // Mesajul de eroare

public:
    // 'explicit' previne conversii accidentale din string în GameException.
    explicit GameException(const std::string& msg) : mesaj(msg) {}

    // Suprascriem what() din std::exception.
    // 'noexcept' promite că această funcție nu aruncă alte excepții.
    // .c_str() convertește std::string în char* (formatul cerut de std::exception).
    const char* what() const override {
        return mesaj.c_str();
    }
};

// 1. Eroare aruncata cand fisierul deck.txt nu poate fi deschis.
//    Se arunca in: Game.cpp -> makeBaseDeck()
class DeckLoadException : public GameException {
public:
    explicit DeckLoadException(const std::string& fisier)
        : GameException("Eroare fatala: Nu am putut deschide fisierul " + fisier + "!") {}
};

// 2. Eroare aruncata cand o carte este creata cu statistici invalide (ex: mana > 10).
//    Se arunca in: Card.cpp -> constructorul Card()
class InvalidCardStatException : public GameException {
public:
    explicit InvalidCardStatException(const std::string& numeCarte)
        : GameException("Eroare la crearea cartii: '" + numeCarte + "' are statistici invalide (mana > 10)!") {}
};

// 3. Eroare aruncata cand jucatorul incearca sa joace o carte fara suficienta mana.
//    Se arunca in: Game.cpp -> tryPlayCardFromHand() si tryPlaySpellFromHand()
class NotEnoughManaException : public GameException {
public:
    explicit NotEnoughManaException(const std::string& numeCarte)
        : GameException("Eroare de joc: Nu ai suficienta mana pentru a juca '" + numeCarte + "'!") {}
};

#endif // TRADINGCARDGAME_EXCEPTIONS_H

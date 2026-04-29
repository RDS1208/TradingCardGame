#include "GameHelpers.h"
#include "Exceptions.h"

#include <exception>
#include <iostream>

int main() {

    try {
        std::string plaerOneName;
        std::string plaerTwoName;

        std::cout << "Enter player one name: ";
        std::getline(std::cin, plaerOneName);
        std::cout << "Enter player two name: ";
        std::getline(std::cin, plaerTwoName);

        Game game(plaerOneName, plaerTwoName);
        game.startGame();

        std::cout << "Match started between " << plaerOneName << " and " << plaerTwoName << ".\n";
        std::cout << "Each turn: you can play, inspect hand/boards, attack, or end turn.\n\n";

        // Bucla principală a jocului: se repetă până când un jucător pierde.
        while (!game.isGameOver()) {
            bool endTurn = false;

            // Bucla turn-ului: jucătorul alege acțiuni până decide să termine turul.
            while (!endTurn && !game.isGameOver()) {
                showTurnMenu(game);

                const int choice = readInt("Choose action: ");
               
                try {
                    switch (choice) {
                        case 1:
                            handlePlayCard(game);
                            break;
                        case 2:
                            game.displayCurrentHand(std::cout);
                            break;
                        case 3:
                            game.displayBoards(std::cout);
                            break;
                        case 4:
                            handleAttack(game);
                            break;
                        case 5:
                            endTurn = true;
                            break;
                        case 6:
                            std::cout << "Exiting game...\n";
                            return 0;
                        default:
                            std::cout << "Unknown option.\n";
                            break;
                    }
                } catch (const GameException& ex) {
                    // Prindem DOAR erorile noastre de joc (GameException și derivatele).
                    std::cout << "\n[!] " << ex.what() << "\n\n";
                }
            }

            if (!game.isGameOver()) {
                game.changeTurn();
            }
        }

        std::cout << "Game over! Un jucator a distrus 6 carti adverse.\n";

    } catch (const std::exception& ex) {
        // Prinde ORICE eroare standard (inclusiv GameException, fiindcă moștenește din std::exception).
        // Ajung aici doar erorile care n-au fost prinse de stratul interior.
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
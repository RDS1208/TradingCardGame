#include "GameHelpers.h"
#include "Exceptions.h"

#include <exception>
#include <iostream>

int main() {

    try {
        // Citim numele jucatorilor de la tastatura
        std::string playerOneName;
        std::string playerTwoName;

        std::cout << "Enter player one name: ";
        std::getline(std::cin, playerOneName);
        std::cout << "Enter player two name: ";
        std::getline(std::cin, playerTwoName);

        // Initializam motorul de joc si pornim meciul
        Game game(playerOneName, playerTwoName);
        game.startGame();

        std::cout << "Match started between " << playerOneName << " and " << playerTwoName << ".\n";
        std::cout << "Each turn: you can play, inspect hand/boards, attack, or end turn.\n\n";

        while (!game.isGameOver()) {
            bool endTurn = false;

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
                } catch (const GameException<unsigned>& ex) {
                    // Prindem exceptiile care contin un numar (de exemplu: "nu ai destula mana")
                    std::cout << "\n[!] " << ex.what() << "\n";
                    printExceptionDetails(ex);
                } catch (const GameExceptionBase& ex) {
                    // Prindem restul exceptiilor specifice jocului nostru
                    std::cout << "\n[!] " << ex.what() << "\n\n";
                }
            }

            if (!game.isGameOver()) {
                game.changeTurn();
            }
        }

        std::cout << "Game over! Un jucator a distrus 6 carti adverse.\n";

    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
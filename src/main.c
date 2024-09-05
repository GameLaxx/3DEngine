#include <SDL2/SDL.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    // Initialisation de la SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation de la SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Création d'une fenêtre
    SDL_Window* window = SDL_CreateWindow(
        "Canvas SDL",                     // Titre de la fenêtre
        SDL_WINDOWPOS_UNDEFINED,          // Position X de la fenêtre
        SDL_WINDOWPOS_UNDEFINED,          // Position Y de la fenêtre
        800, 800,                         // Largeur et hauteur de la fenêtre
        SDL_WINDOW_SHOWN                 // Option pour montrer la fenêtre
    );

    if (window == NULL) {
        printf("Erreur lors de la création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Création d'un renderer pour dessiner
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Erreur lors de la création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Couleur du canvas (blanc)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Remplir le canvas avec la couleur définie
    SDL_RenderClear(renderer);

    // Afficher le canvas
    SDL_RenderPresent(renderer);

    // Boucle pour garder la fenêtre ouverte
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
    }

    // Nettoyage
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

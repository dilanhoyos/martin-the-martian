#include "entitymanager.h"

#include "Tech_SDLBridge.h"
#include "ShipFactory.h"
#include "SignalSlot/signalslot.h"
#include "Event/eventkeyboard.h"


#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>


const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;



namespace ACTION {

    template <typename T>
    struct Action{
        std::weak_ptr<GTech2D::Tech2DEvent> e;
        void(*fpointer)(std::weak_ptr<GTech2D::Tech2DEvent>, std::weak_ptr<T>) ;
        std::weak_ptr<T> param;
        void operator()(){
            fpointer(e, param);
        }
    };



}


namespace GAME {

    enum class GameResult   {GAME_OK, GAME_FINISHED};
    GTech2D::GTPTech2D  ptech;

    bool bGameIsOn;

    enum class MovingState { STAND_STILL = -1,  GOING_RIGHT = 0, GOING_LEFT = 1, DODGE_RIGHT = 2, DODGE_LEFT = 3};
    MovingState movingState;

    void OnEscPressed();
    ECS::Signal<> signalOnEscPressed;

    void OnArrowKeyPressed(const GTech2D::Tech2DEventKeyboard&);
    ECS::Signal<const GTech2D::Tech2DEventKeyboard& > signalOnArrowKeyPressed;

    void InitGameTech(){

        ptech = nullptr;
        ptech = GTech2D::Tech2DFactory::StartTechInstance();
        ptech->Init();

        GTech2D::Rectangle2D rectangle2D(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT);
        GTech2D::WindowConfiguration windowConfiguration{
                "Rendering to a texture!",
                rectangle2D
        };

        ptech->CreateWindow(windowConfiguration, 0);
        ptech->CreateRenderer();

        //SDL Specific Code.
        auto sdl_ptech = dynamic_cast<Tech_SDLBridge*>(ptech.get());
        ptech->Assert(sdl_ptech->InitImageLoading() != 0);
        ptech->Assert(sdl_ptech->DetectJoysticks() != 0);



    }
    namespace SCENE{
        void Init(){

            /* Connect signals to Scene slots */
            signalOnEscPressed.connect(GAME::OnEscPressed);
            signalOnArrowKeyPressed.connect(GAME::OnArrowKeyPressed);


        }
    }
    void ProcessSDLEvents(){

        SDL_Event e;
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_KEYDOWN){
                if (e.key.keysym.sym  == SDLK_ESCAPE){
                    signalOnEscPressed.emit();
                }
            }
        }
    }
    unsigned int MainLoop() {

        bGameIsOn = true;
        auto sdl_ptech = dynamic_cast<Tech_SDLBridge*>(ptech.get());
        while (bGameIsOn){

            //Get events from loop
            SDL_Event e;
            while(SDL_PollEvent(&e)){

                if (e.type == SDL_KEYDOWN ){

                    if ( e.key.keysym.sym == SDLK_ESCAPE ){
                        signalOnEscPressed.emit();
                    } else if ( e.key.keysym.sym == SDLK_UP){
                        signalOnArrowKeyPressed.emit(Tech2DEventKeyboard{Tech2DEventKeyboard::KBEvent::KEY_PRESSED, Tech2DEventKeyboard::KBKey::K_UP});
                    } else if ( e.key.keysym.sym == SDLK_DOWN){
                        signalOnArrowKeyPressed.emit(Tech2DEventKeyboard{Tech2DEventKeyboard::KBEvent::KEY_PRESSED, Tech2DEventKeyboard::KBKey::K_DOWN});
                    } else if ( e.key.keysym.sym == SDLK_LEFT){
                        signalOnArrowKeyPressed.emit(Tech2DEventKeyboard{Tech2DEventKeyboard::KBEvent::KEY_PRESSED, Tech2DEventKeyboard::KBKey::K_LEFT});
                    } else if ( e.key.keysym.sym == SDLK_RIGHT){
                        signalOnArrowKeyPressed.emit(Tech2DEventKeyboard{Tech2DEventKeyboard::KBEvent::KEY_PRESSED, Tech2DEventKeyboard::KBKey::K_RIGHT});
                    }

                }
            }

        }
        return 0;
    };
    void OnEscPressed(){
        std::cout << "GAME::OnEscPressed "  << __FUNCTION__ << std::endl;
        bGameIsOn = false;
    }
    void OnArrowKeyPressed(const GTech2D::Tech2DEventKeyboard& kbEv){


        using GTech2D::Tech2DEventKeyboard;

        bool bGoingLeft = true;
        bGoingLeft = movingState == MovingState::GOING_LEFT || movingState == MovingState::DODGE_LEFT;
        bool bGoingRight = true;
        bGoingRight = movingState == MovingState::GOING_RIGHT || movingState == MovingState::DODGE_RIGHT;

        if (kbEv.m_key == GTech2D::Tech2DEventKeyboard::KBKey::K_LEFT && !bGoingLeft){
            movingState = MovingState::GOING_LEFT;
            std::cout << __FUNCTION__ << ": ";
            std::cout << " Left";
            std::cout << "\n";
        } else if (kbEv.m_key == GTech2D::Tech2DEventKeyboard::KBKey::K_RIGHT && !bGoingRight) {
            movingState = MovingState::GOING_RIGHT;
            std::cout << __FUNCTION__ << ": ";
            std::cout << " Right";
            std::cout << "\n";
        } else if (kbEv.m_key == GTech2D::Tech2DEventKeyboard::KBKey::K_DOWN) {
            std::cout << __FUNCTION__ << ": ";
            std::cout << " Down";
            std::cout << "\n";
        } else if (kbEv.m_key == GTech2D::Tech2DEventKeyboard::KBKey::K_UP) {
            std::cout << __FUNCTION__ << ": ";
            std::cout << " Up";
            std::cout << "\n";
        }

    }
}
using namespace GAME;


int main(int argc, char **argv) {

    /* Init Game Technology */
    InitGameTech();
    GAME::SCENE::Init();

    /* Create Ship */
    auto ship = GAME::ShipFactory::CreateShip(ptech);
    GAME::ShipFactory::SetShipPosition(ship, WIN_WIDTH>>1, WIN_HEIGHT>>1);
    GAME::RenderingSystem::SubscribeEntity(ship);

    /* Create a texture to draw on */
    GTech2D::Texture2DSize textureSize{WIN_WIDTH, WIN_HEIGHT};
    auto pATexture = ptech->CreateTextureWithSize(textureSize);

    /* Set the texture as the drawing texture */
    ptech->SetRenderTarget(pATexture);
    ptech->RenderClear();

    /* Draw the ship */
    GAME::RenderingSystem::DrawSprites(ptech);

    /* Stop having pATexture as the drawing texture */
    ptech->DetachRenderTexture();

    /* Clear the screen */
    ptech->RenderClear();

    /* Take pATexture and render it into the screen */
    ptech->RenderTextureEx(pATexture, GTech2D::Zero, GTech2D::Zero, 0, pATexture->Center(), GTech2D::FlipType::FLIP_NO);

    /* Ok, show the result */
    ptech->UpdateScreen();

    /* Set a 3 seconds delay */
    MainLoop();

    /* Destroy the texture */
    ptech->DestroyTexture(pATexture);

    /* Finish all the tech system (SDL for this case) */
    ptech->Finish();
    return 0;
}

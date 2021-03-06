#include "Game.h"

Game::Game(MovementTimer &moveTimer)
{
    this->moveTimer = &moveTimer;
    pieceMan = PieceManager(1);
    init();
}

void Game::init()
{
    srand((unsigned int) time(0));
    gameOver = false;
    currentPiece = Tetromino(pieceMan.next());
    well = Well(sf::Vector2f(150, 50));
    moveTimer->init();
    well.init();
    pieceMan.init();
    currentPiece = Tetromino(pieceMan.next());
    holdBox = PieceBox(sf::Vector2f(25, 150), 25);
    holdBox.init();
    bag = Bag(sf::Vector2f(450, 125), pieceMan, 5, 25);
    bag.init();
}

void Game::tick(KeyManager &keyManager)
{
    auto moveLeft = [](Tetromino &currentPiece, Well &well)
    {
        currentPiece.move(sf::Vector2i(-1, 0));
        if(!well.inBounds(currentPiece))
        {
            currentPiece.move(sf::Vector2i(1, 0));
        }
    };
    auto moveRight = [](Tetromino &currentPiece, Well &well)
    {
        currentPiece.move(sf::Vector2i(1, 0));
        if(!well.inBounds(currentPiece))
        {
            currentPiece.move(sf::Vector2i(-1, 0));
        }
    }; 
    auto moveDown = [](Tetromino &currentPiece, Well &well)
    {
        bool success = true;
        currentPiece.move(sf::Vector2i(0, 1));
        if(!well.inBounds(currentPiece))
        {
            success = false;
            currentPiece.setAtBottom(true);
            currentPiece.move(sf::Vector2i(0, -1));
        }
        return success;
    };

    if(keyManager.isPressed(sf::Keyboard::Left))
    {
        moveLeft(currentPiece, well);
        moveTimer->restartMoveTimer();
        moveTimer->restartDASTimer();
    }
    if(keyManager.isDown(sf::Keyboard::Left))
    {
        if(moveTimer->shouldDAS() && moveTimer->shouldMove())
        {
            moveLeft(currentPiece, well);
            moveTimer->restartMoveTimer();
        }
        if(currentPiece.isAtBottom())
        {
            moveTimer->restartFallTimer();
        }
    }
    if(keyManager.isPressed(sf::Keyboard::Right))
    {
        moveRight(currentPiece, well);
        moveTimer->restartMoveTimer();
        moveTimer->restartDASTimer();
    }
    if(keyManager.isDown(sf::Keyboard::Right))
    {
        if(moveTimer->shouldDAS() && moveTimer->shouldMove())
        {
            moveRight(currentPiece, well);
            moveTimer->restartMoveTimer();
        }
        if(currentPiece.isAtBottom())
        {
            moveTimer->restartFallTimer();
        }
    }
    if(keyManager.isDown(sf::Keyboard::Down))
    {
        if(moveTimer->shouldSoftDrop())
        {
            if(moveDown(currentPiece, well))
            {
                moveTimer->setLockTimerRunning(false);
            }
            moveTimer->restartSoftDropTimer();
        }
        moveTimer->restartFallTimer();
    }
    if(keyManager.isPressed(sf::Keyboard::LControl))
    {
        currentPiece.rotateCounterClockwise();
        moveTimer->setLockTimerRunning(true);
        if(currentPiece.isAtBottom())
        {
            moveTimer->restartFallTimer();
        }
    }
    if(keyManager.isPressed(sf::Keyboard::Up))
    {
        currentPiece.rotateClockwise();
        moveTimer->setLockTimerRunning(true);
        if(currentPiece.isAtBottom())
        {
            moveTimer->restartFallTimer();
        }
    }
    if(keyManager.isPressed(sf::Keyboard::LShift))
    {
        pieceMan.swap(currentPiece);
        holdBox.setPieceType(pieceMan.getHold());
        if(!currentPiece.isAtBottom() && moveTimer->shouldSoftDrop())
        {
            moveTimer->restartSoftDropTimer();
        }
        moveTimer->restartFallTimer();
    }
    if(keyManager.isPressed(sf::Keyboard::Space))
    {
        dropPiece();
        pieceMan.resetHoldRepeat();
    }
    if(keyManager.isPressed(sf::Keyboard::R))
    {
        init();
    }
}

void Game::update()
{
    holdBox.update();
    bag.update();

    if(!gameOver)
    {
        // game over check
        if(well.getOccupiedHeight() > well.getWellHeight())
        {
            gameOver = true;
        }

        // checks if the piece cannot move down
        currentPiece.move(sf::Vector2i(0, 1));
        if(!well.inBounds(currentPiece))
        {
            currentPiece.setAtBottom(true);
        }
        else
        {
            currentPiece.setAtBottom(false);
        }
        currentPiece.move(sf::Vector2i(0, -1));

        if(currentPiece.isAtBottom())
        {
            // the locking mechanism
            if(moveTimer->isLockTimerRunning())
            {
                if(moveTimer->shouldLock())
                {
                    dropPiece();
                    moveTimer->restartLockDelay();
                    moveTimer->setLockTimerRunning(false);
                }
                else
                {
                    moveTimer->updateLockTimer();
                }
            }
            else
            {
                moveTimer->setLockTimerRunning(true);
                moveTimer->restartLockDelay();
            }
        }
        else
        {
            // moves the piece down
            if(moveTimer->shouldFall())
            {
                currentPiece.move(sf::Vector2i(0, 1));
                moveTimer->restartFallTimer();
                moveTimer->restartLockDelay();
                moveTimer->setLockTimerRunning(false);
            }
        }

        // collision checking
        if(!well.inBounds(currentPiece))
        {
            well.findValidGrid(currentPiece);
        }

        well.previewDrop(currentPiece);
        well.showCurrentPiece(currentPiece);
        well.update();
    }
}

void Game::render(sf::RenderWindow &window)
{
    window.clear();

    well.render(window);
    holdBox.render(window);
    bag.render(window);

    window.display();
}

bool Game::isGameOver()
{
    return gameOver;
}

void Game::dropPiece()
{
    well.dropCurrentPiece();
    currentPiece = Tetromino(pieceMan.next());
    moveTimer->restartFallTimer();
}
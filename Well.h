#pragma once

#include <SFML/Graphics.hpp>
#include "Tetromino.h"

class Well
{
public:
    Well(sf::Vector2f pos = sf::Vector2f(0,0))
    {
        this->pos = pos;
        wellRect.setPosition(pos);
        wellRect.setFillColor(sf::Color(50, 50, 50));
        wellRect.setSize(sf::Vector2f(WELL_WIDTH * BLOCK_SIZE, WELL_HEIGHT * BLOCK_SIZE));
        for(unsigned int i = 0; i < WELL_HEIGHT; i++)
        {
            std::vector<char> row;
            for(unsigned int j = 0; j < WELL_WIDTH; j++)
            {
                row.push_back('0');
                sf::RectangleShape rs(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE));
                rs.setPosition(sf::Vector2f(j * BLOCK_SIZE, i * BLOCK_SIZE) + pos);
                rs.setOutlineColor(sf::Color::Black);
                rs.setOutlineThickness(1);
                blockRects.push_back(rs);
            }
            well.push_back(row);
        }
        newWell = well;
    }

    void update()
    {
        for(int i = 0; i < WELL_WIDTH * WELL_HEIGHT; i++)
        {
            switch(well[i / WELL_WIDTH][i % WELL_WIDTH])
            {
            case '0':
                blockRects[i].setFillColor(sf::Color(50, 50, 50));
                break;
            case '1':
                blockRects[i].setFillColor(sf::Color(51, 255, 255));
                break;
            case '2':
                blockRects[i].setFillColor(sf::Color(51, 51, 255));
                break;
            case '3':
                blockRects[i].setFillColor(sf::Color(255, 187, 51));
                break;
            case '4':
                blockRects[i].setFillColor(sf::Color(255, 255, 51));
                break;
            case '5':
                blockRects[i].setFillColor(sf::Color(255, 51, 85));
                break;
            case '6':
                blockRects[i].setFillColor(sf::Color(51, 255, 51));
                break;
            case '7':
                blockRects[i].setFillColor(sf::Color(255, 51, 255));
                break;
            case '1' + 10:
                blockRects[i].setFillColor(sf::Color(51, 255, 255, 25));
                break;
            case '2' + 10:
                blockRects[i].setFillColor(sf::Color(51, 51, 255, 25));
                break;
            case '3' + 10:
                blockRects[i].setFillColor(sf::Color(255, 187, 51, 25));
                break;
            case '4' + 10:
                blockRects[i].setFillColor(sf::Color(255, 255, 51, 25));
                break;
            case '5' + 10:
                blockRects[i].setFillColor(sf::Color(255, 51, 85, 25));
                break;
            case '6' + 10:
                blockRects[i].setFillColor(sf::Color(51, 255, 51, 25));
                break;
            case '7' + 10:
                blockRects[i].setFillColor(sf::Color(255, 51, 255, 25));
                break;
            }
        }
        well = newWell;
    }

    void render(sf::RenderWindow *window)
    {
        window->draw(wellRect);
        for(sf::RectangleShape wellRect : blockRects)
        {
            window->draw(wellRect);
        }
    }

    void showCurrentPiece(Tetromino tetromino)
    {
        currentPiece = tetromino;
        std::vector<std::vector<char>> shape = tetromino.getShape();
        sf::Vector2i gridPos = tetromino.getGridPos();
        for(unsigned int i = 0; i < shape.size(); i++)
        {
            for(unsigned int j = 0; j < shape[i].size(); j++)
            {
                if(shape[i][j] != '0')
                {
                    well[gridPos.y + i][gridPos.x + j] = shape[i][j];
                }
            }
        }
    }

    void previewDrop(Tetromino tetromino)
    {
        while(inBounds(tetromino))
        {
            tetromino.move(sf::Vector2i(0, 1));
        }
        tetromino.move(sf::Vector2i(0, -1));
        previewPiece = tetromino;
        std::vector<std::vector<char>> shape = tetromino.getShape();
        sf::Vector2i gridPos = tetromino.getGridPos();
        for(unsigned int i = 0; i < shape.size(); i++)
        {
            for(unsigned int j = 0; j < shape[i].size(); j++)
            {
                if(shape[i][j] != '0')
                {
                    well[gridPos.y + i][gridPos.x + j] = shape[i][j] + 10;
                }
            }
        }
    }

    void placePiece(Tetromino tetromino)
    {
        if(inBounds(tetromino))
        {
            std::vector<std::vector<char>> shape = tetromino.getShape();
            sf::Vector2i gridPos = tetromino.getGridPos();
            for(unsigned int i = 0; i < shape.size(); i++)
            {
                for(unsigned int j = 0; j < shape[i].size(); j++)
                {
                    if(shape[i][j] != '0')
                    {
                        newWell[gridPos.y + i][gridPos.x + j] = shape[i][j];
                    }
                }
            }
        }
    }

    void dropCurrentPiece()
    {
        placePiece(previewPiece);
    }

    bool inBounds(Tetromino tetromino)
    {
        std::vector<std::vector<char>> shape = tetromino.getShape();
        sf::Vector2i gridPos = tetromino.getGridPos();
        for(unsigned int i = 0; i < shape.size(); i++)
        {
            for(unsigned int j = 0; j < shape[i].size(); j++)
            {
                unsigned int x = gridPos.x + j;
                unsigned int y = gridPos.y + i;
                if(shape[i][j] != '0')
                {
                    if((y < 0 || x < 0 || y >= WELL_HEIGHT || x >= WELL_WIDTH))
                    {
                        return false;
                    }
                    else if(newWell[y][x] != '0')
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void findValidGrid(Tetromino &tetromino)
    {
        int intervalX = 0;
        int intervalY = 0;
        bool success = false;
        while(!success && intervalX < WELL_WIDTH)
        {
            intervalY = 0;
            while(intervalY < WELL_HEIGHT)
            {
                tetromino.move(sf::Vector2i(intervalX, -intervalY));
                if(inBounds(tetromino))
                {
                    success = true;
                    break;
                }
                else
                {
                    tetromino.move(sf::Vector2i(-intervalX, intervalY));
                }
                tetromino.move(sf::Vector2i(-intervalX, -intervalY));
                if(inBounds(tetromino))
                {
                    success = true;
                    break;
                }
                else
                {
                    tetromino.move(sf::Vector2i(intervalX, intervalY));
                }
                intervalY++;
            }
            intervalX++;
            if(success)
            {
                break;
            }
        }
    }

private:
    static const int WELL_WIDTH = 10;
    static const int WELL_HEIGHT = 20;
    static const int BLOCK_SIZE = 25;

    std::vector<std::vector<char>> well;
    std::vector<std::vector<char>> newWell;
    std::vector<sf::RectangleShape> blockRects;
    sf::RectangleShape wellRect;
    sf::Vector2f pos;

    Tetromino currentPiece;
    Tetromino previewPiece;
};
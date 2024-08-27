#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "raylib.h"

enum Suit { HEARTS, CLUBS, DIAMONDS, SPADES };

struct Card {
    Suit suit;
    int value;
    bool isFaceUp;
    Rectangle sourceRect;
    Vector2 position;

    void drawCard(Vector2 size, Texture2D& facecard, Texture2D& backcard) {
        if (isFaceUp) {
            DrawTexturePro(facecard, this->sourceRect,
                           {position.x, position.y, size.x, size.y}, {0, 0},
                           0.0f, WHITE);
        } else {
            DrawTexturePro(backcard, this->sourceRect,
                           {position.x, position.y, size.x, size.y}, {0, 0},
                           0.0f, WHITE);
        }
    }

    void getCardRectangle(Vector2 size) {
        int column = (this->value) - 1;
        int row = (this->suit);
        this->sourceRect = {column * size.x, row * size.y, size.x, size.y};
    }
};

class MainDeck {
   public:
    std::vector<Card> cards;

    void initializeDeck() {
        for (int suit = HEARTS; suit <= SPADES; ++suit) {
            for (int value = 1; value <= 13; ++value) {
                Card card = {static_cast<Suit>(suit), static_cast<int>(value),
                             false};
                card.getCardRectangle({225, 315});
                cards.push_back(card);
            }
        }
        shuffleDeck();
    }

    void shuffleDeck() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    }

    Card takeCard() {
        if (!cards.empty()) {
            Card card = cards.back();
            cards.pop_back();
            return card;
        } else {
            return {};
        }        
    }
};

class Table {
   public:
    std::vector<Card> columns[7];

    void initializeTable(MainDeck& deck, Vector2 size) {
        for (int i = 0; i < 7; ++i) {
            Vector2 position = {GetScreenWidth() / 16 + i * (size.x + 10),
                                GetScreenHeight() / 2 - (size.y * 1 / 2)};
            for (int j = 0; j <= i; ++j) {
                Card card = deck.takeCard();
                card.position = position;
                if (j == i) card.isFaceUp = true;
                columns[i].push_back(card);
                position.y += 30;
            }
        }
    }

    void refreshCardPosition(Vector2 size) {
        for (int i = 0; i < 7; ++i) {
            Vector2 position = {GetScreenWidth() / 16 + i * (size.x + 10),
                                GetScreenHeight() / 2 - (size.y * 1 / 2)};
            for (int j = 0; j <= i; ++j) {
                columns[i][j].position = position;
                position.y += 30;
            }
        }
    }

    void drawTable(Vector2 size, Texture2D& facecard, Texture2D& backcard) {
        for (int i = 0; i < 7; ++i) {
            for (size_t j = 0; j < columns[i].size(); ++j) {
                columns[i][j].drawCard(size, facecard, backcard);
            }
        }
    }

    void moveCardToColumn(int column, int row, int selectedColumn) {
        Card card = columns[column][row];
        columns[column].erase(columns[column].begin() + row);
        columns[selectedColumn].push_back(card);
    }

};

class HiddenPool {
   public:
    std::vector<Card> cards;

    void initializeHiddenPool(MainDeck& deck) { 
        while (deck.cards.size() != 0) {
            Card card = deck.takeCard();
            this->cards.push_back(card);
        }
    }

    void AddCard(Card card) { cards.push_back(card); }

    Card DrawCard() {
        Card card = cards.back();
        cards.pop_back();
        return card;
    }

    void DrawWastePile(Vector2 size, Texture2D& facecard, Texture2D& backcard) {
        Vector2 position = {GetScreenWidth() / 16, GetScreenHeight() / 16};
        if (!cards.empty()) {
            if (!cards.back().isFaceUp) {
                cards.back().position = position;
                cards.back().drawCard(size, facecard, backcard);
            } else {
                position.x += 40;
                cards.back().position = position;
                cards.back().drawCard(size, facecard, backcard);
            }
        }
    }
};

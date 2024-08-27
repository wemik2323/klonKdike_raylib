#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "raylib.h"

enum Suit { HEARTS, CLUBS, DIAMONDS, SPADES };
enum GameState { MENU, GAME, GAME_OVER };

struct Card {
    Suit suit;
    int value;
    bool isFaceUp;
    Rectangle sourceRect;
    Vector2 position;

    void drawCard(Vector2 size, Texture2D& facecard, Texture2D& backcard) {
        if (isFaceUp) {
            DrawTexturePro(facecard, sourceRect,
                           {position.x, position.y, size.x, size.y}, {0, 0},
                           0.0f, WHITE);
        } else {
            DrawTexturePro(backcard, sourceRect,
                           {position.x, position.y, size.x, size.y}, {0, 0},
                           0.0f, WHITE);
        }
    }

    void faceToggle() {
        if (this->isFaceUp) {
            isFaceUp = false;
        } else {
            isFaceUp = true;
        }
    }

    bool suitReliable(Card secondCard) {
        switch (this->suit) {
            case HEARTS:
                switch (secondCard.suit) { 
                    case HEARTS:
                        return false;
                    case CLUBS:
                        return true;
                    case DIAMONDS:
                        return false;
                    case SPADES:
                        return true;
                }
            case CLUBS:
                switch (secondCard.suit) {
                    case HEARTS:
                        return true;
                    case CLUBS:
                        return false;
                    case DIAMONDS:
                        return true;
                    case SPADES:
                        return false;
                }
            case DIAMONDS:
                switch (secondCard.suit) {
                    case HEARTS:
                        return false;
                    case CLUBS:
                        return true;
                    case DIAMONDS:
                        return false;
                    case SPADES:
                        return true;
                }
            case SPADES:
                switch (secondCard.suit) {
                    case HEARTS:
                        return true;
                    case CLUBS:
                        return false;
                    case DIAMONDS:
                        return true;
                    case SPADES:
                        return false;
                }
        }
    }

    void setCardRectangle() {
        int column = value - 1;
        int row = static_cast<int>(suit);
        sourceRect = {column * 225.0f, row * 315.0f, 225, 315};
    }
};

class MainDeck {
   public:
    std::vector<Card> cards;

    void initializeDeck(Vector2 size) {
        for (int suit = HEARTS; suit <= SPADES; ++suit) {
            for (int value = 1; value <= 13; ++value) {
                Card card = {static_cast<Suit>(suit), value, false};
                card.setCardRectangle();
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
        Card card = cards.back();
        cards.pop_back();
        return card;
    }

    bool isEmpty() { return cards.empty(); }
};

class HiddenPool {
   public:
    std::vector<Card> cards;
    std::vector<Card> buffer;
    Vector2 position;

    void initializeHiddenPool(MainDeck& deck) {
        position = {float(GetScreenWidth()) / 8, float(GetScreenHeight()) / 16};
        while (!deck.isEmpty()) {
            cards.push_back(deck.takeCard());
            cards.back().position = position;
        }
    }

    void showNextCard(Vector2 size) { 
        if (!cards.empty()) {
            Card card = cards.back();
            card.position.x = position.x + (size.x * 5/4);
            card.isFaceUp = true;
            buffer.push_back(card);
            cards.pop_back();

            for (auto card : buffer) {
                std::cout << card.suit << " " << card.value << '\n';
            }
        } else {
            for (auto card : buffer) {
                card.position = position;
                card.isFaceUp = false;
                cards.push_back(card);
            }
            buffer.clear();
        }
    }

    void updateCoords() {
        position = {float(GetScreenWidth()) / 8, float(GetScreenHeight()) / 16};
    }

    void drawHiddenPool(Texture2D& facecard, Texture2D& backcard,
                        Vector2 size) {
        for (int i = 0; i < cards.size(); i++) {
             cards[i].drawCard(size, facecard, backcard);
        }
        for (int i = 0; i < buffer.size(); i++) {
            buffer[i].drawCard(size, facecard, backcard);
        }
    }

    void refreshPositions(Vector2 size) { 
        for (int i = 0; i < cards.size(); i++) {
            cards[i].position = position;
        }
        for (int i = 0; i < buffer.size(); i++) {
            buffer[i].position.x = position.x + (size.x * 5/4);
            buffer[i].position.y = position.y;
        }
    }

};

class Table {
   public:
    std::vector<Card> columns[7];

    void initializeTable(MainDeck& deck, Vector2 size, HiddenPool& hiddenPool) {
        for (int i = 0; i < 7; ++i) {
            Vector2 position = {float(GetScreenWidth()/8 + i * (size.x + size.x/20)), hiddenPool.position.y + size.y + size.y * 5/30};
            for (int j = 0; j <= i; ++j) {
                Card card = deck.takeCard();
                card.position = position;
                if (j == i) card.isFaceUp = true;
                columns[i].push_back(card);
                position.y += 30;
            }
        }
    }

    void refreshPositions(Vector2 size, HiddenPool& hiddenPool) {
        for (int i = 0; i < 7; ++i) {
            Vector2 position = {
                float(GetScreenWidth() / 8 + i * (size.x + size.x / 20)),
                                hiddenPool.position.y + size.y + size.y * 5/30};
            for (size_t j = 0; j < columns[i].size(); ++j) {
                columns[i][j].position = position;
                position.y += 30;
            }
        }
    }

    void drawTable(Vector2 size, Texture2D& facecard, Texture2D& backcard, Texture2D& cock, HiddenPool& hiddenPool) {
        for (int i = 0; i < 7; ++i) {
            if (!columns[i].empty()) {
                for (size_t j = 0; j < columns[i].size(); ++j) {
                    columns[i][j].drawCard(size, facecard, backcard);
                }
            } else {
                Vector2 position = {
                    float(GetScreenWidth() / 8 + i * (size.x + size.x / 20)),
                    hiddenPool.position.y + size.y + size.y * 5 / 30};
                DrawTexturePro(cock, {0, 0, 225, 315}, {position.x, position.y, size.x, size.y}, {0, 0}, 0.0f, WHITE);
            }
        }
    }

    void moveCards(int fromColumn, int fromRow, int toColumn) {
        auto& source = columns[fromColumn];
        auto& destination = columns[toColumn];
        destination.insert(destination.end(), source.begin() + fromRow,
                           source.end());
        source.erase(source.begin() + fromRow, source.end());
    }

    void moveFromHiddenPool(HiddenPool& hiddenPool, int toColumn) {
        auto& destination = columns[toColumn];
        destination.push_back(hiddenPool.buffer.back());
        hiddenPool.buffer.pop_back();
    }
};

class HomeCell {
   public:
    std::vector<Card> cells[4];
    Vector2 position = {float(GetScreenWidth() / 2), float(GetScreenHeight() / 16)
};

    void drawHomeCells(Vector2 size, Texture2D& facecard, Texture2D& backcard, Texture2D& slot) {
        for (int i = 0; i < 4; ++i) {
            if (!cells[i].empty()) {
                cells[i].back().drawCard(size, facecard, backcard);
            } else {
                DrawRectangle(position.x + i * (size.x + size.x / 10),
                              position.y, size.x, size.y, DARKGRAY);
                DrawTexturePro(slot, {0, 0, 225, 315},
                               { position.x + i * (size.x + size.x / 10),
                                position.y, size.x, size.y},
                               {0, 0}, 0.0f, WHITE);
            }
        }
    }

    void updateCoords() {
        position = {float(GetScreenWidth() / 2), float(GetScreenHeight() / 16)};
    }

    bool canPlaceCard(Card card) {
        if (cells[card.suit].empty()) {
            return card.value == 1;
        } else {
            Card topCard = cells[card.suit].back();
            return topCard.value ==
                   card.value - 1;
        }
    }

    void placeCard(Card card, Vector2 size) { 
        card.position.x = this->position.x + card.suit * (size.x/10 + size.x);
        card.position.y = this->position.y;
        cells[card.suit].push_back(card); 
    }
};

Card* selectedCard = nullptr;
int selectedColumn = -1;
int selectedRow = -1;

void CheckMouseInput(Table& table, HiddenPool& hiddenPool, HomeCell& homeCell,
                     Vector2 size, Texture2D& facecard, Texture2D& backcard);

int main() {
    GameState gameState = MENU;
    const int screenWidth = 1000;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Solitaire");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    Texture2D facecard =
        LoadTextureFromImage(LoadImage("../resources/spritesheet.png"));
    Texture2D backcard =
        LoadTextureFromImage(LoadImage("../resources/backcard.png"));
    Texture2D slot = 
        LoadTextureFromImage(LoadImage("../resources/slot.png"));
    Texture2D cock = LoadTextureFromImage(LoadImage("../resources/niceCock.png"));
    Texture2D bg =
        LoadTextureFromImage(LoadImage("../resources/bg.jpg"));

    Vector2 cardSize = {GetScreenWidth() / 10, GetScreenHeight() / 6};

    MainDeck deck;
    deck.initializeDeck(cardSize);

    HiddenPool hiddenPool;
    Table table;
    table.initializeTable(deck, cardSize, hiddenPool);
    hiddenPool.initializeHiddenPool(deck);

    HomeCell homeCell;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        int width = GetScreenWidth();
        int height = GetScreenHeight();
        if (height < width/13*9) {
            cardSize = {float(GetScreenWidth() / 10),
                        float(GetScreenWidth() / 10 / 5 * 7)};
        } else {
            cardSize = {float(GetScreenHeight() / 6 / 7 * 5),
                        float(GetScreenHeight() / 6)};
        }
        

        switch (gameState) {
            case MENU:
                if (IsKeyPressed(KEY_ENTER) or IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    gameState = GAME;
                }
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("Press ENTER to Start", screenWidth / 4,
                         screenHeight / 2, 40, DARKGRAY);
                EndDrawing();
                break;
            case GAME:
                BeginDrawing();
                ClearBackground(RAYWHITE);

                DrawTexturePro(bg, {0, 0, 884, 1080},
                               {0, 0, 200, 300},
                    {0, 0}, 0.0f, WHITE);

                if (hiddenPool.cards.empty() and hiddenPool.buffer.empty()) {

                    DrawText("Keep going >:')", screenWidth * 3 / 4,
                             screenHeight * 3 / 4, 20, DARKGRAY);

                    if (table.columns->empty()) {
                        gameState = GAME_OVER;
                    }
                }

                CheckMouseInput(table, hiddenPool, homeCell, cardSize, facecard, backcard);

                table.drawTable(cardSize, facecard, backcard, cock, hiddenPool);
                hiddenPool.drawHiddenPool(facecard, backcard, cardSize);
                hiddenPool.updateCoords();
                homeCell.drawHomeCells(cardSize, facecard, backcard, slot);
                homeCell.updateCoords();

                if (selectedCard != nullptr) {
                    selectedCard->drawCard(cardSize, facecard, backcard);
                }

                table.refreshPositions(cardSize, hiddenPool);
                hiddenPool.refreshPositions(cardSize);

                EndDrawing();
                break;
            case GAME_OVER:
                BeginDrawing();
                ClearBackground(RAYWHITE);

                DrawText("Ebat tbl lacker, olyx", screenWidth / 4,
                         screenHeight / 2, 40, DARKGRAY);

                EndDrawing();
                break;
        }
    }

    UnloadTexture(facecard);
    UnloadTexture(backcard);
    UnloadTexture(slot);
    UnloadTexture(cock);
    UnloadTexture(bg);

    CloseWindow();

    return 0;
}

void CheckMouseInput(Table& table, HiddenPool& hiddenPool, HomeCell& homeCell, Vector2 size, Texture2D& facecard, Texture2D& backcard) {
    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

        Rectangle hiddenPoolRec = {hiddenPool.position.x, hiddenPool.position.y,
                                   size.x, size.y};
        if (CheckCollisionPointRec(mousePos, hiddenPoolRec)) {
            hiddenPool.showNextCard(size);
            return;
        }

        hiddenPoolRec = {hiddenPool.position.x + size.x + 40, hiddenPool.position.y, size.x, size.y};

        if (CheckCollisionPointRec(mousePos, hiddenPoolRec)) {
            if (!hiddenPool.buffer.empty()) {
                Card& card = hiddenPool.buffer.back();
                selectedCard = &card;
                for (int i = 0; i < hiddenPool.buffer.size(); ++i) {
                    if (card.value == hiddenPool.buffer[i].value &&
                        card.suit == hiddenPool.buffer[i].suit) {
                        selectedRow = i;
                        break;
                    }
                }
            }
            return;
        }

        for (int i = 0; i < 7; ++i) {
            for (int j = table.columns[i].size() - 1; j >= 0; --j) {
                Card& card = table.columns[i][j];
                if (card.isFaceUp) {
                    Rectangle cardRect = {card.position.x, card.position.y,
                                          size.x, size.y};
                    if (CheckCollisionPointRec(mousePos, cardRect)) {
                        selectedCard = &card;
                        selectedColumn = i;
                        selectedRow = j;
                        return;
                    }
                }
            }
        }
    }

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && selectedColumn == -1 && selectedRow != -1) {
        selectedCard->position.x = mousePos.x - size.x / 2;
        selectedCard->position.y = mousePos.y - size.y / 2;
        std::cout << selectedCard->suit << " " << selectedCard->value << '\n';
    }

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && selectedColumn != -1 && selectedRow != -1) {
        for (int j = selectedRow; j < table.columns[selectedColumn].size();
             ++j) {
            table.columns[selectedColumn][j].position.x =
                mousePos.x - size.x / 2;
            table.columns[selectedColumn][j].position.y =
                mousePos.y + (j - selectedRow) * 30 - size.y / 2;
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && selectedCard != nullptr) {
        bool cardMoved = false;

        for (int i = 0; i < 7; ++i) {
            if (i != selectedColumn) {
                if (!table.columns[i].empty()) {
                    Card& targetCard = table.columns[i].back();
                    Rectangle targetRect = {targetCard.position.x,
                                            targetCard.position.y, size.x,
                                            size.y};

                    if (CheckCollisionPointRec(mousePos, targetRect)) {
                        if (targetCard.value - selectedCard->value != 1 or !selectedCard->suitReliable(targetCard)) {
                            continue;
                        }
                        if (selectedColumn != -1) {
                            table.moveCards(selectedColumn, selectedRow, i);
                            if (selectedRow > 0) {
                                table.columns[selectedColumn][selectedRow - 1].isFaceUp =
                                    true;
                            }
                        } else {
                            table.moveFromHiddenPool(hiddenPool, i);
                        }
                        cardMoved = true;
                        break;
                    }
                } else {
                    Rectangle emptyRect = {float(100 + i * (size.x + 10)),
                                           200.0f, size.x, size.y};

                    if (CheckCollisionPointRec(mousePos, emptyRect)) {
                        if (selectedCard->value == 13) {
                            if (selectedColumn != -1) {
                            table.moveCards(selectedColumn, selectedRow, i);
                                if (selectedRow > 0) {
                                    table
                                        .columns[selectedColumn]
                                                [selectedRow - 1]
                                        .faceToggle();
                                }
                            } else {
                                table.moveFromHiddenPool(hiddenPool, i);
                            }
                            cardMoved = true;
                            break;
                        }
                    }
                }
            }
        }

        if (!cardMoved) {
            for (int i = 0; i < 4; ++i) {
                Rectangle homeCellRect = {
                    homeCell.position.x + i * (size.x + 20),
                    homeCell.position.y, size.x, size.y};
                if (CheckCollisionPointRec(mousePos, homeCellRect)) {
                    if (homeCell.canPlaceCard(*selectedCard)) {
                        homeCell.placeCard(*selectedCard, size);
                        if (selectedColumn != -1) {
                            table.columns[selectedColumn].erase(
                                table.columns[selectedColumn].begin() +
                                    selectedRow,
                                table.columns[selectedColumn].end());
                            if (!table.columns[selectedColumn][selectedRow - 1]
                                     .isFaceUp) {
                                table.columns[selectedColumn][selectedRow - 1]
                                    .faceToggle();
                            }
                        } else {
                            hiddenPool.buffer.pop_back();
                        }
                        cardMoved = true;
                        break;
                    }
                }
            }
        }

        selectedCard = nullptr;
        selectedColumn = -1;
        selectedRow = -1;
    }
}

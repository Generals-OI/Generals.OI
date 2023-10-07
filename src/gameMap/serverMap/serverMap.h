#ifndef SERVER_MAP_H
#define SERVER_MAP_H

#include <utility>

#include "basicMap.h"
#include "gameInformation.h"

class ServerMap : public BasicMap {
private:
    std::vector<int> roundLose; // In which round did the player lose
    std::vector<int> loseInfo; // Id of the capturer (their own id if surrendered)
    std::vector<std::vector<bool>> flagDiff;
public:
    int cntPlayer{}, cntTeam{}, round{};
    std::vector<int> idTeam;

    ServerMap() = default;

    ServerMap(int width, int length, int cntPlayer, int cntTeam, const std::vector<int> &idTeam);

    ServerMap(ServerMap &&) = default;

    ServerMap(const ServerMap &) = default;

    ServerMap &operator=(const ServerMap &) = default;

    ServerMap &operator=(ServerMap &&) = default;

    QVector<qint32> toVectorSM();

    QVector<qint32> exportDiff();

    void importSM(const QVector<qint32> &);

    // Note that this function removes the part of the QByteArray that it uses
    void loadByteArray(QByteArray &);

    // Copies from another ServerMap of the same game and marks flagDiff in the process
    void copyWithDiff(const ServerMap &);

    QByteArray toByteArray();

    // Moves army after checking validity
    bool move(int, Point, int, int, bool, int);

    // Adds game round, returns id of newly-lost players and their reason of losing
    std::vector<std::pair<int, int>> addRound();

    // Make player with specified ID surrender
    void surrender(int);

    // Returns whether the game has ended
    bool gameOver() const;
};

#endif // SERVER_MAP_H

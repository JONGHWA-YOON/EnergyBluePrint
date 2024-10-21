#include "YunutyEngine.h"
#include "NavigationField2D.h"

unordered_set<NavigationField2D*> NavigationField2D::navFields;
const NavigationField2D::Tile* NavigationField2D::Tile::destination = nullptr;

NavigationField2D::NavigationField2D()
{
    navFields.insert(this);
}
NavigationField2D::~NavigationField2D()
{
    navFields.erase(this);
}

Vector2d NavigationField2D::TileToWorldLocaction(const Tile& tile)
{

    Vector2d indexVector(tile.col, tile.row);
    return indexVector * minTileSize + GetTransform()->GetWorldPosition()
        - 0.5 * Vector2d(fieldRect.width, fieldRect.height);
}
NavigationField2D::Tile* NavigationField2D::WorldLocactionToTile(const Vector2d& worldPosition)
{
    if (!fieldRect.Contains(worldPosition - GetTransform()->GetWorldPosition()))
        return nullptr;

    Vector2d indexVector =
        (worldPosition - GetTransform()->GetWorldPosition() + 0.5 * Vector2d(fieldRect.width, fieldRect.height)) /
        minTileSize;
    return &tileMap[indexVector.y][indexVector.x];
}

void NavigationField2D::SetField(Rect fieldRect, int minTileSize, int layerNum)
{
    this->fieldRect = fieldRect;
    this->minTileSize = minTileSize;
    this->layerNum = layerNum;
    tileWidth = ceil(fieldRect.width / minTileSize);
    tileHeight = ceil(fieldRect.height / minTileSize);
    this->fieldRect.width = minTileSize * tileWidth;
    this->fieldRect.height = minTileSize * tileHeight;

    for (int i = 0; i < tileHeight; i++)
    {
        tileMap.push_back(vector<Tile>());
        for (int j = 0; j < tileWidth; j++)
            tileMap[i].push_back(Tile(i, j));
    }
}

NavigationField2D* NavigationField2D::GetAssignedField(const Vector2d& worldLocation)
{
    for (auto each : navFields)
        if (each->fieldRect.Contains(worldLocation, each->GetTransform()->GetWorldPosition()))
            return each;

    return nullptr;
}
NavigationField2D* NavigationField2D::GetAssignedField(const Vector2d& worldLocation, const Vector2d& worldLocation2)
{
    for (auto each : navFields)
        if (each->fieldRect.Contains(worldLocation, each->GetTransform()->GetWorldPosition()))
            if (each->fieldRect.Contains(worldLocation2, each->GetTransform()->GetWorldPosition()))
                return each;

    return nullptr;
}

queue<Vector2d> NavigationField2D::RequestPath(Vector2d origin, Vector2d destination, int maxPathLength, unordered_set<const Tile*>* openSetOut, unordered_set<const Tile*>* closedSetOut, unordered_set<const Tile*>* pathOut)
{
    auto tile_src = WorldLocactionToTile(origin);
    Tile::destination = WorldLocactionToTile(destination);
    if (tile_src == nullptr || Tile::destination == nullptr)
        return queue<Vector2d>();

    priority_queue<Tile*, vector<Tile*>, Tile::TileCompare> openSet;
    unordered_set<Tile*> closedSet;

    stack<const Tile*> path;
    openSet.push(tile_src);
    tile_src->g_score = 0;

    while (!openSet.empty())
    {
        auto currentTile = openSet.top();
        openSet.pop();

        //if (closedSet.find(currentTile) != closedSet.end())
        //    continue;

        if (currentTile == Tile::destination || currentTile->g_score > maxPathLength)
        {
            const Tile* pathTile = currentTile;
            while (pathTile != tile_src)
            {
                path.push(pathTile);
                pathTile = pathTile->previous;
            }
            break;
        }

        for (int i = 0; i < 8; i++)
        {
            auto row = currentTile->row + dy[i];
            auto col = currentTile->col + dx[i];
            if (row < 0 || col < 0 || row >= tileHeight || col >= tileWidth)
                continue;

            if (tileMap[row][col].state == Tile::State::Closed)
                continue;
            if (tileMap[row][col].state == Tile::State::Closed || tileMap[row][col].state == Tile::State::Open_Occupied)
                continue;

            if (closedSet.find(&tileMap[row][col]) != closedSet.end())
                continue;

            tileMap[row][col].g_score = currentTile->g_score + (dy[i] * dx[i] == 0 ? 1 : 1.41421);
            tileMap[row][col].previous = currentTile;

            closedSet.insert(&tileMap[row][col]);
            openSet.push(&tileMap[row][col]);
        }
    }
    if (openSetOut != nullptr)
    {
        openSetOut->clear();
        while (!openSet.empty())
        {
            openSetOut->insert(openSet.top());
            openSet.pop();
        }
    }
    if (closedSetOut != nullptr)
    {
        closedSetOut->clear();
        for (auto each : closedSet)
            closedSetOut->insert(each);
    }
    if (pathOut != nullptr)
        pathOut->clear();

    queue<Vector2d> vectorPath;
    while (!path.empty())
    {
        vectorPath.push(TileToWorldLocaction(*path.top()));
        if (pathOut)
            pathOut->insert(path.top());
        path.pop();
    }

    return vectorPath;
}
void NavigationField2D::ReceiveUnitStatus(const NavigationUnit2DReport& newStatus, const NavigationUnit2DReport& lastStatus)
{
    auto currentTile = WorldLocactionToTile(newStatus.position);
    auto previousTile = WorldLocactionToTile(lastStatus.position);
    if (previousTile)
    {
        switch (lastStatus.state)
        {
        case NavigationUnit2DState::Occupying:
        case NavigationUnit2DState::OccupyingTemporarilly:
            previousTile->state = Tile::State::Open;
            break;
        }
    }
    if (currentTile)
    {
        switch (newStatus.state)
        {
        case NavigationUnit2DState::Occupying:
            currentTile->state = Tile::State::Open_Occupied;
            break;
        case NavigationUnit2DState::OccupyingTemporarilly:
            currentTile->state = Tile::State::Open_Occupied_Temporarily;
            break;
        }
    }
}

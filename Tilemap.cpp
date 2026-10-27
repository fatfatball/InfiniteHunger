#include "Tilemap.h"
#include "BaseGameObject.h"
#include "Constants.h"
#include "AABB.h"
#include "Vector2.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

Tilemap::Tilemap(GamesEngineeringBase::Window& canvas)
	: BaseGameObject(canvas) {
	for (int i = 0; i < ROOM_HEIGHT; ++i) {
		for (int j = 0; j < ROOM_WIDTH; ++j) {
			tileIds[j][i] = 0;
			tileCollisions[j][i] = false;
		}
	}
	renderOrder = RenderOrder::First;
	isLoaded = false;
	isFocused = false;
}

void Tilemap::OnStart() {
	LoadTiles(GetTileFolderPath());
	LoadMap(GetMapFolderPath());
	isLoaded = true;
}

void Tilemap::FixedUpdate(const float& dt) {

}

void Tilemap::VisualUpdate(const float& dt) {

}

void Tilemap::OnDestroy() {

}

void Tilemap::UpdateRenderState(const float& alpha, const Camera& camera) {
	if (!camera.followTarget) {
		isFocused = false;
	}
	else {
		isFocused = GetAABB().IsInside(camera.followTarget->globalPosition);
	}
	renderAABB = GetAABB().GetIntersectionAABB(camera.GetAABB());
	renderScreenAABB = camera.GetScreenAABB(renderAABB);
}

bool Tilemap::DepthTest(float alpha, const Camera & camera, const short& curDepth, const ScreenAABB & tile) const {
	if (!isLoaded || !isFocused) {
		return false;
	}

	if (!renderScreenAABB.Intersects(tile)) {
		return false;
	}

	return true;
}

void Tilemap::RenderSelf(float alpha, const Camera& camera, const short& curDepth, const ScreenAABB& renderTile) const {
	if (!isLoaded || !isFocused) {
		return;
	}

	if (!renderScreenAABB.Intersects(renderTile)) {
		return;
	}

	ScreenAABB screenAABB = renderScreenAABB.GetIntersectionScreenAABB(renderTile);

	unsigned char* backBuffer = env.getBackBuffer();
	Vector2 beginPos = camera.GetGlobalPosition(screenAABB.minX, screenAABB.minY);
	beginPos = beginPos - globalPosition;
	Vector2 tileOffset = Vector2{ ROOM_WIDTH, ROOM_HEIGHT } * 0.5f * TILE_LENGTH;

	Vector2 localPos, offset;
	int tileX, tileY;
	for (int j = screenAABB.minY; j <= screenAABB.maxY; ++j) {
		for (int i = screenAABB.minX; i <= screenAABB.maxX; ++i) {
			if (camera.depthGraph[j * WINDOW_WIDTH + i] < curDepth) {
				continue;
			}
			offset.x = (i - screenAABB.minX) * STEP_X;
			offset.y = (j - screenAABB.minY) * STEP_Y;
			localPos = beginPos + offset;
			// now use localPos to find the tile, and the pixel on that pos.
			if (!LocalPos2TilePos(localPos, tileX, tileY)) {
				continue;
			}
			// now use localPos to find the exact pos in that imgTile
			localPos = localPos + tileOffset;
			localPos.x -= tileX * TILE_LENGTH;
			localPos.y -= tileY * TILE_LENGTH;
			
			env.draw(i, j, tiles[tileIds[tileX][tileY]].image->atUnchecked(localPos.x, localPos.y));
		}
	}
}

ColorRGBA Tilemap::GetColorAt(const float& u, const float& v) const {
	return ColorRGBA();
}

unsigned char Tilemap::GetAlphaAt(const float& u, const float& v) const {
	return 255;
}

bool Tilemap::LocalPos2TilePos(const Vector2& localPos, int& tileX, int& tileY) const {
	int x = std::floor(localPos.x / TILE_LENGTH + ROOM_WIDTH / 2.f);
	int y = std::floor(localPos.y / TILE_LENGTH + ROOM_HEIGHT / 2.f);
	if (x < 0 || x > ROOM_WIDTH - 1 || y < 0 || y > ROOM_HEIGHT - 1) {
		return false;
	}
	tileX = x;
	tileY = y;
	return true;
}

bool Tilemap::IsSolid(const int& tileX, const int& tileY) const
{
	if (tileX < 0 || tileX > ROOM_WIDTH - 1 || tileY < 0 || tileY > ROOM_HEIGHT - 1) {
		return false;
	}
	return tileCollisions[tileX][tileY];
}

AABB Tilemap::GetAABB() const
{
	return AABB(
		globalPosition - Vector2{ROOM_WIDTH, ROOM_HEIGHT} / 2.f * TILE_LENGTH,
		globalPosition + Vector2{ROOM_WIDTH, ROOM_HEIGHT} / 2.f * TILE_LENGTH
	);
}

AABB Tilemap::GetTileLocalAABB(const int& tileX, const int& tileY) const
{
	float minX = (tileX - ROOM_WIDTH / 2.f) * TILE_LENGTH;
	float minY = (tileY - ROOM_HEIGHT / 2.f) * TILE_LENGTH;
	float maxX = minX + TILE_LENGTH;
	float maxY = minY + TILE_LENGTH;
	return AABB({ minX, minY }, { maxX, maxY });
}

bool Tilemap::ResolveCollision(BasePhysicalObject* obj, Vector2& normal, Vector2& position) const
{
	if (!isLoaded) {
		return false;
	}

	AABB objAABB(
		obj->globalPosition - Vector2(obj->collisionRadius, obj->collisionRadius),
		obj->globalPosition + Vector2(obj->collisionRadius, obj->collisionRadius)
	);
	if (!GetAABB().Intersects(objAABB)) {
		return false;
	}

	Vector2 localPos = obj->globalPosition - globalPosition;
	float halfW = ROOM_WIDTH * TILE_LENGTH * 0.5;
	float halfH = ROOM_HEIGHT * TILE_LENGTH * 0.5;

	bool collided = false;

	int tx0 = (int)std::floor((localPos.x - obj->collisionRadius + halfW) / TILE_LENGTH);
	int tx1 = (int)std::floor((localPos.x + obj->collisionRadius + halfW) / TILE_LENGTH);
	int ty0 = (int)std::floor((localPos.y - obj->collisionRadius + halfH) / TILE_LENGTH);
	int ty1 = (int)std::floor((localPos.y + obj->collisionRadius + halfH) / TILE_LENGTH);

	for (int j = ty0; j <= ty1; ++j) {
		for (int i = tx0; i <= tx1; ++i) {
			if (!IsSolid(i, j)) {
				continue;
			}

			AABB tileAABB = GetTileLocalAABB(i, j);
			float closestX = std::clamp(localPos.x, tileAABB.min.x, tileAABB.max.x);
			float closestY = std::clamp(localPos.y, tileAABB.min.y, tileAABB.max.y);
			Vector2 dir = localPos - Vector2{ closestX, closestY };
			float l = dir.Length();

			if (l > obj->collisionRadius) {
				continue;
			}

			Vector2 normal;
			float penetration;

			if (l > 1e-4) { // obj is outside of the tile, and colliding
				normal = dir.Normalized();
				penetration = obj->collisionRadius - l;
			}
			else { // obj is inside the tile, push it out
				float leftL = localPos.x - tileAABB.min.x;
				float rightL = tileAABB.max.x - localPos.x;
				float topL = localPos.y - tileAABB.min.y;
				float bottomL = tileAABB.max.y - localPos.y;

				float minDist = leftL;
				normal = { -1, 0 };
				if (rightL < minDist) {
					minDist = rightL;
					normal = { 1, 0 };
				}
				if (topL < minDist) {
					minDist = topL;
					normal = { 0, -1 };
				}
				if (bottomL < minDist) {
					minDist = bottomL;
					normal = { 0, 1 };
				}
				penetration = minDist + obj->collisionRadius;
			}
			localPos = localPos + normal * penetration * 2;
			collided = true;
		}
	}

	obj->globalPosition = globalPosition + localPos;
	return collided;
}

void Tilemap::LoadTiles(const std::string& folderPath) {
	std::ifstream infile;
	infile.open(folderPath + "/tiles.txt", std::ios::in);

	int id;
	std::string filename;
	bool isSolid;
	std::string line;
	std::string strTemp;
	tiles.Clear();
	while (std::getline(infile, line)) {
		std::stringstream ss(line);
		std::getline(ss, strTemp, '\t');
		id = std::stoi(strTemp); // id doesnt matter, just use line order
		std::getline(ss, filename, '\t');
		std::getline(ss, strTemp, '\t');
		isSolid = std::stoi(strTemp) != 0;

		auto img = std::make_shared<GamesEngineeringBase::Image>();
		img->load(folderPath + '/' + filename);
		ImgTile newTile{ img, isSolid };
		tiles.PushBack(newTile);
	}
}

void Tilemap::LoadMap(const std::string & folderPath) {
	std::ifstream infile;
	infile.open(folderPath + "/map.txt", std::ios::in);

	for (int i = 0; i < ROOM_HEIGHT; ++i) {
		for (int j = 0; j < ROOM_WIDTH; ++j) {
			infile >> tileIds[j][i];
			tileCollisions[j][i] = tiles[tileIds[j][i]].isSolid;
		}
	}
}

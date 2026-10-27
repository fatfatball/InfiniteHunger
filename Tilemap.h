#pragma once
#include "BaseGameObject.h"
#include "BasePhysicalObject.h"
#include "GamesEngineeringBase.h"
#include "List.h"
#include <string>
#include <memory>

// each tilemap would be a room of around 1500*1500 (world axis), all corners blocked, like Issac rooms
// there should be a room manager:
//		monitor the camera's position, always keep the 5 rooms around are instantiated
//		and destroy other rooms.
//		also, change the boundary of camera in real time.
//		all spawned rooms (and the enemies) should be recorded
// this is an abstract class
class Tilemap : public BaseGameObject {
public:
	bool isFocused; // only the tilemap with camera inside is focused

	Tilemap(GamesEngineeringBase::Window& canvas);

	void OnStart() override;
	void FixedUpdate(const float& dt) override;
	void VisualUpdate(const float& dt) override;
	void OnDestroy() override;

	void UpdateRenderState(const float& alpha, const Camera& camera) override;
	bool DepthTest(float alpha, const Camera& camera, const short& curDepth, const ScreenAABB& tile)
		const override;
	void RenderSelf(float alpha, const Camera& camera, const short& curDepth, const ScreenAABB& tile)
		const override;
	ColorRGBA GetColorAt(const float& u, const float& v) const override; // just don't use them
	unsigned char GetAlphaAt(const float& u, const float& v) const override;

	bool LocalPos2TilePos(const Vector2& localPos, int& tileX, int& tileY) const;
	bool IsSolid(const int& tileX, const int& tileY) const;
	AABB GetAABB() const;
	AABB GetTileLocalAABB(const int& tileX, const int& tileY) const;
	bool ResolveCollision(BasePhysicalObject* obj, Vector2& normal, Vector2& position) const;

protected:
	struct ImgTile {
		std::shared_ptr<GamesEngineeringBase::Image> image;
		bool isSolid = false;
	};
	List<ImgTile> tiles;
	int tileIds[ROOM_WIDTH][ROOM_HEIGHT]; // tile id in each position, width * height
	bool tileCollisions[ROOM_WIDTH][ROOM_HEIGHT]; // tile collision in each position, width * height
	bool isLoaded;

	void LoadTiles(const std::string& folderPath);
	void LoadMap(const std::string& folderPath);
	virtual std::string GetTileFolderPath() = 0;
	virtual std::string GetMapFolderPath() = 0;
};
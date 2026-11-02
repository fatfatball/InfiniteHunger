#include "ClickableText.h"
#include "TextRenderer.h"
#include "AABB.h"
#include "SmallStructs.h"

ClickableText::ClickableText(GamesEngineeringBase::Window& canvas,
	const std::string& text, const Vector2& position, const int& scale) 
	: text(text), env(canvas), position(position), scale(scale) {
	color = { 200, 200, 200, 255 };
	isPendingDestroy = false;
	prevMousePressed = env.mouseButtonPressed(GamesEngineeringBase::MouseButton::MouseLeft);
}

ClickableText::ClickableText(GamesEngineeringBase::Window& canvas,
	const std::string& text, const Vector2& position, const int& scale, std::function<void()> OnClick) 
	: ClickableText(canvas, text, position, scale){
	this->OnClick = OnClick;
	screenAABB = TextRenderer::GetTextScreenAABB(text, position.x, position.y, scale);
}

void ClickableText::OnStart() {

}

void ClickableText::VisualUpdate(const float& dt) {
	if (OnClick != nullptr && !isPendingDestroy) {
		if (env.getMouseInWindowX() >= screenAABB.minX && env.getMouseInWindowX() <= screenAABB.maxX &&
			env.getMouseInWindowY() >= screenAABB.minY && env.getMouseInWindowY() <= screenAABB.maxY) {
			color = { 255, 255, 255, 255 };
			if (env.mouseButtonPressed(GamesEngineeringBase::MouseButton::MouseLeft) && !prevMousePressed) {
				OnClick();
			}
		}
		else {
			color = { 200, 200, 200, 255 };
		}
	}
	prevMousePressed = env.mouseButtonPressed(GamesEngineeringBase::MouseButton::MouseLeft);
}

void ClickableText::OnDestroy() {

}

void ClickableText::RenderSelf() {
	TextRenderer::DrawText(env, text, position.x, position.y, scale, color);
}

void ClickableText::SetText(const std::string& newText) {
	text = newText;
	screenAABB = TextRenderer::GetTextScreenAABB(text, position.x, position.y, scale);
}

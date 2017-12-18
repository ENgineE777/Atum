
#include "GUIWidget.h"

Texture* GUIWidget::tex;
Quad GUIWidget::quad;

GUIWidget::GUIWidget()
{

}

void GUIWidget::DrawSelf()
{
	quad.Draw(tex, Vector2(pos.Pos().x, pos.Pos().y), size);
}

void GUIWidget::Draw()
{
	DrawSelf();

	for (auto* child : childs)
	{
		child->Draw();
	}
}
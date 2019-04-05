
#include "SpriteWidget.h"
#include "SceneObjects/2D/SpriteWindow.h"

void SpriteWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	openBtn = new EUIButton(panel, "Edit", 90, 5, 95, 20);
	openBtn->SetListener(-1, this, 0);

}

void SpriteWidget::SetData(void* owner, void* set_data)
{
	sprite = (Sprite::Data*)set_data;
}

void SpriteWidget::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	SpriteWindow::StartEdit(sprite);
}
#include "UILabelAsset.h"
#include "Services/Font/Fonts.h"

CLASSREG(UIWidgetAsset, UILabelAsset, "Label")

META_DATA_DESC(UILabelAsset)
BASE_WIDGET_ASSET_PROP(UILabelAsset)
FLOAT_PROP(UILabelAsset, trans.pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(UILabelAsset, trans.pos.y, 0.0f, "Prop", "y")
ENUM_PROP(UILabelAsset, horzAlign, 0, "Prop", "horz_align")
	ENUM_ELEM("Left", 0)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Right", 2)
ENUM_END
ENUM_PROP(UILabelAsset, vertAlign, 0, "Prop", "vert_align")
	ENUM_ELEM("Top", 3)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Bottom", 4)
ENUM_END
FLOAT_PROP(UILabelAsset, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(UILabelAsset, trans.size.y, 100.0f, "Prop", "height")
ENUM_PROP(UILabelAsset, horzSize, 0, "Prop", "horz_size")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
ENUM_PROP(UILabelAsset, vertSize, 0, "Prop", "vert_size")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
FLOAT_PROP(UILabelAsset, trans.offset.x, 0.0f, "Prop", "anchorn_x")
FLOAT_PROP(UILabelAsset, trans.offset.y, 0.0f, "Prop", "anchorn_y")
FLOAT_PROP(UILabelAsset, left_padding.x, 0.0f, "Prop", "left_padding")
FLOAT_PROP(UILabelAsset, left_padding.y, 0.0f, "Prop", "top_padding")
FLOAT_PROP(UILabelAsset, right_padding.x, 0.0f, "Prop", "right_padding")
FLOAT_PROP(UILabelAsset, right_padding.y, 0.0f, "Prop", "bottom_padding")
FLOAT_PROP(UILabelAsset, rotate, 0.0f, "Prop", "rotate")
COLOR_PROP(UILabelAsset, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UILabelAsset, color.a, 1.0f, "Prop", "alpha")
BOOL_PROP(UILabelAsset, scaleChilds, false, "Prop", "scale_childs")
FILENAME_PROP(UILabelAsset, font_name, "settings/helvetica", "Prop", "font_name")
INT_PROP(UILabelAsset, font_height, 15, "Prop", "font_height")
STRING_PROP(UILabelAsset, text, "Label", "Prop", "text")
ENUM_PROP(UILabelAsset, textHorzAlign, 0, "Prop", "text_horz_align")
	ENUM_ELEM("Left", 0)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Right", 2)
ENUM_END
ENUM_PROP(UILabelAsset, textVertAlign, 0, "Prop", "text_vert_align")
	ENUM_ELEM("Top", 3)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Bottom", 4)
ENUM_END
META_DATA_DESC_END()

void UILabelAsset::Init()
{
}

void UILabelAsset::ApplyProperties()
{
	RELEASE(font)

	font = fonts.LoadFont(font_name.c_str(), false, false, (int)(font_height * render.GetDevice()->GetHeight() / 1024.0f));
}

void UILabelAsset::SetText(string& set_text)
{
	text = set_text;
}

void UILabelAsset::Draw(float dt)
{
#ifdef EDITOR
	if (edited)
	{
		//GetMetaData()->UpdateWidgets();
	}
#endif

	if (GetState() == Invisible)
	{
		return;
	}


	if (horzSize == wrap_context)
	{
		Vector2 parent_size;

		if (parent)
		{
			parent_size = parent->trans.size;
		}
		else
		{
			parent_size.x = render.GetDevice()->GetWidth() * 1024.0f / render.GetDevice()->GetHeight();
			parent_size.y = 1024.0f;
		}

		trans.size.x = parent_size.x - right_padding.x - left_padding.x;
	}

	std::vector<FontRes::LineBreak> line_breaks;
	float hgt = font->GetLineBreak(line_breaks, text.c_str(), (int)trans.size.x);

	if (horzSize == wrap_context && line_breaks.size() == 1)
	{
		trans.size.x = line_breaks[0].width * 1024.0f / render.GetDevice()->GetHeight();
	}

	if (vertSize == wrap_context)
	{
		trans.size.y = hgt;
	}

	CalcState();

	float scale = render.GetDevice()->GetHeight() / 1024.0f;
	Matrix mat = trans.mat_global;
	mat.Pos().x -= trans.size.x * trans.offset.x;
	mat.Pos().y -= trans.size.y * trans.offset.y;

	if (textHorzAlign == align_center && line_breaks.size() == 1)
	{
		mat.Pos().x += (trans.size.x - line_breaks[0].width * 1024.0f / render.GetDevice()->GetHeight()) * 0.5f;
	}

	if (textVertAlign == align_center)
	{
		mat.Pos().y += (trans.size.y - hgt) * 0.5f;
	}

	mat.Pos().x *= scale;
	mat.Pos().y *= scale;
	mat.Pos().z = 0.2f;

	font->Print(line_breaks, mat, 1.0f, cur_color, text.c_str());

	for (auto child : childs)
	{
		child->Draw(dt);
	}
}

CLASSREG(UIWidgetAsset, UILabelAssetInst, "UILabel")

META_DATA_DESC(UILabelAssetInst)
BASE_WIDGET_INST_PROP(UILabelAssetInst)
COLOR_PROP(UILabelAssetInst, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UILabelAssetInst, color.a, 1.0f, "Prop", "alpha")
STRING_PROP(UILabelAssetInst, text, "Label", "Prop", "text")
META_DATA_DESC_END()

void UILabelAssetInst::BindClassToScript()
{
	BIND_INST_TYPE_TO_SCRIPT(UILabelAssetInst, UILabelAsset)
	scripts.engine->RegisterObjectMethod(script_class_name, "void SetText(string&in)", WRAP_MFN(UILabelAssetInst, SetText), asCALL_GENERIC);
}

#ifdef EDITOR
UILabelAssetInst* UILabelAssetInst::temp = nullptr;

bool UILabelAssetInst::AddedToTreeByParent()
{
	return true;
}

void UILabelAssetInst::StoreProperties()
{
	if (!temp)
	{
		temp = new UILabelAssetInst();
	}

	GetMetaData()->Prepare(temp);
	GetMetaData()->Copy(this);
}

void UILabelAssetInst::RestoreProperties()
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Copy(temp);
}
#endif
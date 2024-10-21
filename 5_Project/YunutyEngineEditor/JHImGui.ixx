/// 기존 imgui의 button은 string 값인 button의 이름을 ID로 구별해 작동하는 방식
/// 이름이 같은 버튼이 존재한다면 정상적으로 동작하지 않음
/// gameObject를 button의 ID로 구별해 이름이 같은 버튼이 존재하여도 
/// 정상적으로 동작하도록하는 커스텀 버튼 구현
/// 2023.08.23 윤종화

module;
#include <string>
#include "imgui.h"
#include "imgui_internal.h"
export module JHImGui;

export namespace ImGui
{
	inline bool JHButton(const void* buttonId, const const char* label, const ImVec2& size_arg = ImVec2{ 0, 0 }, ImGuiButtonFlags flags = ImGuiButtonFlags_None)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		
		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(buttonId);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
			pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
		ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		ImVec2 posSizeSum{0,0};
		posSizeSum.x = pos.x + size.x;
		posSizeSum.y = pos.y + size.y;

		const ImRect bb(pos, posSizeSum);
		ItemSize(size, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
			flags |= ImGuiButtonFlags_Repeat;

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

		// Render
		const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		RenderNavHighlight(bb, id);
		RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

		if (g.LogEnabled)
			LogSetNextTextDecoration("[", "]");
		ImVec2 tempvec{ 0,0 };
		tempvec.x = bb.Min.x + style.FramePadding.x;
		tempvec.y = bb.Min.y + style.FramePadding.y;

		ImVec2 tempvec2{ 0,0 };
		tempvec2.x = bb.Max.x - style.FramePadding.x;
		tempvec2.y = bb.Max.y - style.FramePadding.y;

		RenderTextClipped(tempvec, tempvec2, label, NULL, &label_size, style.ButtonTextAlign, &bb);

		// Automatically close popups
		//if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
		//    CloseCurrentPopup();

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		return pressed;
	}
}
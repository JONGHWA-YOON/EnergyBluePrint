#pragma once
#include "Transform.h"
#include "Vector.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Rect.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif


namespace yunutyEngine
{
    class YUNUTY_API RectTransform :
        public Transform
    {
    private:
        //
        // Summary:
        //     The position of the pivot of this RectTransform relative to the anchor reference
        //     point.
        Vector2d anchoredPosition;
        //
        // Summary:
        //     The offset of the upper right corner of the rectangle relative to the upper right
        //     anchor.
        Vector2d offsetMax;
        //
        // Summary:
        //     The offset of the lower left corner of the rectangle relative to the lower left
        //     anchor.
        Vector2d offsetMin;
        //
        // Summary:
        //     The normalized position in the parent RectTransform that the lower left corner
        //     is anchored to.
        Vector2d anchorMin;
        //
        // Summary:
        //     The normalized position in the parent RectTransform that the upper right corner
        //     is anchored to.
        Vector2d anchorMax;
        //
        // Summary:
        //     The 3D position of the pivot of this RectTransform relative to the anchor reference
        //     point.
        Vector3d anchoredPosition3D;
        //
        // Summary:
        //     The size of this RectTransform relative to the distances between the anchors.
        Vector2d sizeDelta;
        //
        // Summary:
        //     The normalized position in this RectTransform that it rotates around.
        Vector2d pivot;
    public:
        Vector2d GetAnchoredPosition()const
        {
            return anchoredPosition;
        }
        void SetAnchoredPosition(const Vector2d& anchoredPosition)
        {
            //Vector<double>(this->anchoredPosition).operator=(anchoredPosition);
            this->anchoredPosition = anchoredPosition;
        }
        Vector2d GetOffsetMax()const
        {
            return offsetMax;
        }
        void SetOffsetMax(const Vector2d& offsetMax)
        {
            this->offsetMax = offsetMax;
        }
        Vector2d GetOffsetMin()const
        {
            return offsetMin;
        }
        void SetOffsetMin(const Vector2d& offsetMin)
        {
            this->offsetMin = offsetMin;
        }
        Vector2d GetAnchorMin()const
        {
            return anchorMin;
        }
        void SetAnchorMin(const Vector2d& anchorMin)
        {
            this->anchorMin = anchorMin;
        }
        Vector2d GetAnchorMax()const
        {
            return anchorMax;
        }
        void SetAnchorMax(const Vector2d& anchorMax)
        {
            this->anchorMax = anchorMax;
        }
        Vector3d GetAnchoredPosition3D()const
        {
            return anchoredPosition3D;
        }
        void SetAnchoredPosition3D(const Vector3d& anchoredPosition3D)
        {
            this->anchoredPosition3D = anchoredPosition3D;
        }
        Vector2d GetSizeDelta()const
        {
            return sizeDelta;
        }
        void SetSizeDelta(const Vector2d& sizeDelta)
        {
            this->sizeDelta = sizeDelta;
        }
        Vector2d GetPivot()const
        {
            return pivot;
        }
        void SetPivot(const Vector2d& pivot)
        {
            this->pivot = pivot;
        }

        Rect rect();

    };
}

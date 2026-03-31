#include "Tilc/DllGlobals.h"
#include "Tilc/Game2D/Sprite/BaseAnimation.h"

namespace Tilc::Game2D::Sprite
{
	class DECLSPEC TDirectionalAnimation : public TBaseAnimation
	{
	protected:
		virtual void FrameStep();
		virtual void CropSprite();
		virtual void ReadIn(std::stringstream& Stream);
	};

}
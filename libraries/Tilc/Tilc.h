#pragma once

#include "Tilc/DllGlobals.h"
#include "SDL3/SDL.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <string>

#define GLAD_GL_IMPLEMENTATION
#ifdef APIENTRY
# undef APIENTRY
#endif
#include <glad/glad.h>
#ifdef APIENTRY
# undef APIENTRY
#endif

#include "Tilc/Globals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Audio/Music.h"
#include "Tilc/Commerce/Money.h"
#include "Tilc/Commerce/Message.h"
#include "Tilc/Data/Db.h"
#include "Tilc/Data/DbSQLite.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Graphics/Camera.h"
#include "Tilc/Graphics/GraphicsUtils.h"
#include "Tilc/Graphics/Light.h"
#include "Tilc/Graphics/Mesh.h"
#include "Tilc/Graphics/Model.h"
#include "Tilc/Graphics/ShapeOutline.h"
#include "Tilc/Graphics/TextRenderer.h"
#include "Tilc/Graphics/OpenGL/OpenGLErrors.h"
#include "Tilc/Graphics/OpenGL/Shader.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Gui/Menu.h"
#include "Tilc/Gui/Textbox.h"
#include "Tilc/Utils/Object.h"
#include "Tilc/Utils/Archives.h"
#include "Tilc/Utils/ArrayUtils.h"
#include "Tilc/Utils/CommandLine.h"
#include "Tilc/Utils/CodePageConverter.h"
#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Utils/Locale.h"
#include "Tilc/Utils/Math.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Utils/Profiler.h"
#include "Tilc/Utils/StdObject.h"
#include "Tilc/Utils/SystemUtils.h"
#include "Tilc/Utils/Timer.h"
#include "Tilc/Utils/Tree.h"
#include "Tilc/Utils/Types.h"
#include "Tilc/Utils/Vector.h"
#include "Tilc/Utils/Matrix.h"
#include "Tilc/Utils/Quaternion.h"
#include "Tilc/Utils/VectorContainer.h"
#include "Tilc/Resources/Resources.h"
#include "Tilc/Resources/ResourceManager.h"
#include "Tilc/Resources/TextureManager.h"
#include "Tilc/EventManager.h"
#include "Tilc/Game.h"
#include "Tilc/GameState.h"
#include "Tilc/Localization.h"
#include "Tilc/StateMainMenu.h"
#include "Tilc/StateManager.h"
#include "Tilc/Level.h"
#include "Tilc/Window.h"
#include "Tilc/Game2D/World/Tile.h"
#include "Tilc/Game2D/World/World.h"
#include "Tilc/Game2D/World/MapLayer.h"
#include "Tilc/Game2D/Entity.h"
#include "Tilc/Game2D/EntityManager.h"
#include "Tilc/Game2D/Sprite/CommonTypes.h"
#include "Tilc/Game2D/Sprite/SpriteSheet.h"
#include "Tilc/Game2D/Sprite/BaseAnimation.h"
#include "Tilc/Game2D/Sprite/DirectionalAnimation.h"
#include "Tilc/Game3D/CommonGLGameState.h"
#include "Tilc/Game3D/World/World.h"
#include <random>

namespace Tilc
{
	DECLSPEC SDL_AppResult InitTilc(const Tilc::TExtString& WindowTitle, const unsigned int WindowWidth, const unsigned int WindowHeight, int Flags, bool WithGLContext, const Tilc::TExtString& ResourcesDataFile = "data/Resources.cfg", const Tilc::TExtString& DefaultThemeName = "Blue");
	DECLSPEC void CleanupTilc();
}

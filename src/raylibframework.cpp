#include "raylibFramework.h"
using namespace RaylibFramework;


bool IntVector2::operator==(const IntVector2& other) const
{
    return x == other.x && y == other.y;
}


Texture2DStore::Texture2DStore() = default;

Texture2D Texture2DStore::LoadCallback(const char* filePath)
{
    return LoadTexture(filePath);

}

void Texture2DStore::UnloadCallback(Texture2D& texture)
{
    //UnloadTexture(texture);
}


FontStore::FontStore() = default;

Font FontStore::LoadCallback(const char* filePath)
{
    return LoadFontEx(filePath, 50, 0, 255);
}

void FontStore::UnloadCallback(Font& font)
{
    //UnloadFont(font);
}


SoundStore::SoundStore() = default;
//SoundStore::SoundStore() = default;

Sound SoundStore::LoadCallback(const char* filePath)
{
    return LoadSound(filePath);
}

void SoundStore::UnloadCallback(Sound& sound)
{
    //UnloadSound(sound);
}


AssetManager::AssetManager() = default;

AssetManager::~AssetManager() = default;


Drawable::Drawable(const IntVector2 dimensions, const IntVector2 margin)
    : m_dimensions(dimensions), m_margin(margin)
{
}

int Drawable::GetWidth() const
{
    return m_dimensions.x;
}

void Drawable::SetWidth(const int width)
{
    m_dimensions.x = width;
}

int Drawable::GetHeight() const
{
    return m_dimensions.y;
}

void Drawable::SetHeight(const int height)
{
    m_dimensions.y = height;
}

int Drawable::GetMarginWidth() const
{
    return m_margin.x;
}

void Drawable::SetMarginWidth(const int marginWidth)
{
    m_margin.x = marginWidth;
}

int Drawable::GetMarginHeight() const
{
    return m_margin.y;
}

void Drawable::SetMarginHeight(const int marginHeight)
{
    m_margin.y = marginHeight;
}

void Drawable::SetPositionOnScreen(const int x, const int y)
{
    m_positionOnScreen = { x, y };
}

IntVector2 Drawable::GetPositionOnScreen() const
{
    return m_positionOnScreen;
}

void Drawable::SetGridCoords(const IntVector2 coords)
{
    m_coords = coords;
}

IntVector2 Drawable::GetGridCoords() const
{
    return m_coords;
}


DrawableTexture::DrawableTexture(Texture2D* texture, const IntVector2 pixalDimensions, const IntVector2 margin)
    : Drawable(pixalDimensions, margin), m_renderedTexture(texture)
{
}

Texture2D* DrawableTexture::GetTexture() const
{
    return m_renderedTexture;
}

void DrawableTexture::SetTexture(Texture2D* texture)
{
    m_renderedTexture = texture;
}

void DrawableTexture::Render() const
{
    IntVector2 positionOnScreen = GetPositionOnScreen();
    float scaleY = (float)GetHeight() / (float)m_renderedTexture->height;
    float scaleX = (float)GetWidth() / (float)m_renderedTexture->width;

    float scale = std::min(scaleX, scaleY);

    DrawTextureEx(*m_renderedTexture, { (float)positionOnScreen.x, (float)positionOnScreen.y }, 0, scale, WHITE);
}


Text::Text(std::string text, int fontSize, Color colour, Font* font)
    : Drawable(IntVector2{ MeasureText(text.c_str(), fontSize), 10 }, IntVector2{ 0,0 }),
    m_text(text), m_fontSize(fontSize), m_colour(colour), m_font(font)
{
}

std::string Text::GetText() const
{
    return m_text;
}

void Text::SetText(const std::string text)
{
    m_text = text;
}

int Text::GetFontSize() const
{
    return m_fontSize;
}

void Text::SetFontSize(const int fontSize)
{
    m_fontSize = fontSize;
}

int Text::GetWidth() const
{
    return (int)MeasureTextEx(*m_font, m_text.c_str(), (float)m_fontSize, 0).x;
}

int Text::GetHeight() const
{
    return (int)MeasureTextEx(*m_font, m_text.c_str(), (float)m_fontSize, 0).y;
}

AnchorPoints Text::GetAnchorPoint() const
{
    return m_anchorPoint;
}

void Text::SetAnchorPoint(const AnchorPoints anchorpoint)
{
    m_anchorPoint = anchorpoint;
    IntVector2 positionOnScreen = GetPositionOnScreen();
    SetPositionOnScreen(positionOnScreen.x, positionOnScreen.y);
}

void Text::SetPositionOnScreen(int x, int y)
{
    const int height = GetHeight();
    const int width = GetWidth();
    IntVector2 offset = { 0,0 };

    switch (m_anchorPoint)
    {
    case RaylibFramework::TOP_MIDDLE:
        offset.x = width / 2;
        break;

    case RaylibFramework::TOP_RIGHT:
        offset.x = width;
        break;

    case RaylibFramework::MIDDLE_LEFT:
        offset.y = height / 2;
        break;

    case RaylibFramework::MIDDLE:
        offset.x = width / 2;
        offset.y = height / 2;
        break;

    case RaylibFramework::MIDDLE_RIGHT:
        offset.x = width;
        offset.y = height / 2;
        break;

    case RaylibFramework::BOTTOM_LEFT:
        offset.y = height;
        break;

    case RaylibFramework::BOTTOM_MIDDLE:
        offset.x = width / 2;
        offset.y = height;
        break;

    case RaylibFramework::BOTTOM_RIGHT:
        offset.x = width;
        offset.y = height;
        break;

    default:
        break;
    }

    x -= offset.x;
    y -= offset.y;

    m_positionOnScreen = { x, y };
}

Color Text::GetColour() const
{
    return m_colour;
}

void Text::SetColour(const Color colour)
{
    m_colour = colour;
}

void Text::Render() const
{
    IntVector2 positionOnScreen = GetPositionOnScreen();
    DrawTextEx(*m_font, m_text.c_str(), { (float)positionOnScreen.x, (float)positionOnScreen.y }, m_fontSize, 1, m_colour);
}

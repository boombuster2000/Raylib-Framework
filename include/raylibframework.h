#pragma once
#include <vector>
#include <raylib.h>
#include <random>
#include <type_traits>
#include <string>
#include <map>
#include <iostream>

struct IntVector2
{
    int x, y;

    bool operator==(const IntVector2& other) const;
};

namespace RaylibFramework
{

    enum AnchorPoints
    {
        TOP_LEFT,
        TOP_MIDDLE,
        TOP_RIGHT,

        MIDDLE_LEFT,
        MIDDLE,
        MIDDLE_RIGHT,

        BOTTOM_LEFT,
        BOTTOM_MIDDLE,
        BOTTOM_RIGHT,
    };


    template <typename T> class AssetStore
    {
    public:
        std::map<std::string, T> m_assets = {};
        const char* m_assetDirectory = "./";

    public:
        AssetStore() = default;

        AssetStore(const AssetStore&) = delete;
        AssetStore& operator=(const AssetStore&) = delete;


        ~AssetStore()
        {
            std::vector<std::string> keys;
            for (const auto& [key, _] : m_assets) {
                keys.push_back(key);
            }

            for (const auto& key : keys) {
                Unload(key);
            }

            m_assets.clear();  // optional, already empty
        }


        virtual T LoadCallback(const char* filePath) = 0;
        virtual void UnloadCallback(T& asset) = 0;

        T Load(const char* filePath)
        {
            return LoadCallback(filePath);
        }

        void Unload(std::string assetID)
        {
            if (!m_assets.contains(assetID))
            {
                std::cerr << "[WARN] Tried to unload missing asset: " << assetID << "\n";
                return;
            }

            // TODO: FIX ABORT ERROR WHEN UNLOADING ASSET
            // UnloadCallback(m_assets.at(assetID)); 
            m_assets.erase(assetID);
        }

        void LoadAll(const char* assetDirectory)
        {
            FilePathList assetsFilePaths = LoadDirectoryFiles(assetDirectory);

            for (int i = 0; i < assetsFilePaths.count; i++)
            {
                const char* currentFilePath = assetsFilePaths.paths[i];

                if (IsPathFile(currentFilePath))
                {
                    std::string key = GetFileNameWithoutExt(currentFilePath);
                    m_assets[key] = Load(currentFilePath);
                    std::cout << "Asset key loaded: " << key << std::endl;
                }
            }

        }

        T& Get(const char* id)
        {
            if (!m_assets.contains(id))
            {
                std::cerr << "[ERROR] Missing asset: " << id << std::endl;
                throw std::runtime_error("Asset not found");
            }
            return m_assets.at(id);
        }

    };

    class Texture2DStore : public AssetStore<Texture2D>
    {

    public:
        Texture2DStore();

        Texture2D LoadCallback(const char* filePath) override;

        void UnloadCallback(Texture2D& texture) override;
    };

    class FontStore : public AssetStore<Font>
    {
    public:

        FontStore();

        Font LoadCallback(const char* filePath) override;

        void UnloadCallback(Font& font) override;
    };

    class SoundStore : public AssetStore<Sound>
    {
    public:
        SoundStore();

        Sound LoadCallback(const char* filePath) override;

        void UnloadCallback(Sound& sound) override;
    };

    class AssetManager
    {
    public:

        Texture2DStore textures = Texture2DStore();
        FontStore fonts = FontStore();
        SoundStore sounds = SoundStore();

    public:
        AssetManager();
        ~AssetManager();
    };


    class Drawable
    {
    protected:
        IntVector2 m_margin = { 0,0 };
        IntVector2 m_dimensions = { 0,0 };
        IntVector2 m_positionOnScreen = { 0,0 };
        IntVector2 m_coords = { 0,0 };

    public:
        Drawable(const IntVector2 dimensions, const IntVector2 margin);

        virtual void Render() const = 0;

        virtual int GetWidth() const;
        virtual void SetWidth(const int width);

        virtual int GetHeight() const;
        virtual void SetHeight(const int height);

        virtual int GetMarginWidth() const;
        virtual void SetMarginWidth(const int marginWidth);

        virtual int GetMarginHeight() const;
        virtual void SetMarginHeight(const int marginHeight);

        virtual void SetPositionOnScreen(const int x, const int y);
        virtual IntVector2 GetPositionOnScreen() const;

        virtual void SetGridCoords(const IntVector2 coords);
        virtual IntVector2 GetGridCoords() const;
    };

    class DrawableTexture : public Drawable
    {
    private:
        Texture2D* m_renderedTexture = nullptr;

    public:
        DrawableTexture(Texture2D* texture, const IntVector2 pixalDimensions, const IntVector2 margin);

        Texture2D* GetTexture() const;
        void SetTexture(Texture2D* texture);

        void Render() const override;
    };


    class Text : public Drawable
    {
    private:

        std::string m_text;
        int m_fontSize;
        Font* m_font = nullptr;
        Color m_colour;
        AnchorPoints m_anchorPoint = AnchorPoints::TOP_LEFT;

    public:
        Text(std::string text, int fontSize, Color colour, Font* font);

        std::string GetText() const;
        void SetText(const std::string text);

        int GetFontSize() const;
        void SetFontSize(const int fontSize);

        int GetWidth() const override;
        int GetHeight() const override;

        AnchorPoints GetAnchorPoint() const;
        void SetAnchorPoint(const AnchorPoints anchorpoint);

        void SetPositionOnScreen(int x, int y) override;

        Color GetColour() const;
        void SetColour(const Color colour);

        void Render() const override;
    };


    template <typename T_Entity = Drawable>
    class Grid
    {
    protected:
        AnchorPoints m_anchorPoint = AnchorPoints::TOP_LEFT;
        typedef std::vector<std::vector<T_Entity>> T_Grid;
        T_Grid m_grid;

    protected:
        T_Grid GenerateBoard(const IntVector2 dimensions, T_Entity sampleSquare)
        {
            static_assert(std::is_base_of<Drawable, T_Entity>::value, "T_Entity must derive from RaylibFramework::Drawable");

            if (dimensions.x <= 0 || dimensions.y <= 0) {
                throw std::invalid_argument("Board dimensions must be positive");
            }

            T_Grid board;

            // Populate board with tiles.
            for (int y = 0; y < dimensions.y; y++)
            {
                std::vector<T_Entity> row;

                for (int x = 0; x < dimensions.x; x++)
                {
                    sampleSquare.SetGridCoords(IntVector2{ x, y });
                    row.push_back(sampleSquare);
                }

                board.push_back(row);
            }

            return board;
        }

        IntVector2 GetBoardPixelDimensions() const
        {
            if (m_grid.empty() || m_grid[0].empty()) {
                throw std::runtime_error("Empty grid");
            }

            IntVector2 dimensions = {};
            T_Entity square = m_grid[0][0];

            dimensions.x = (int)m_grid[0].size() * square.GetWidth() + ((int)m_grid[0].size() - 1) * square.GetMarginWidth();
            dimensions.y = (int)m_grid.size() * square.GetHeight() + ((int)m_grid.size() - 1) * square.GetMarginHeight();

            return dimensions;
        }

        virtual bool ShouldRenderEntity(const IntVector2 coords) const
        {
            return true;
        }

    public:
        Grid(const IntVector2 dimensions, const T_Entity sampleEntity, const AnchorPoints anchorPoint, const IntVector2 position)
        {
            m_grid = GenerateBoard(dimensions, sampleEntity);
            SetAnchorPoint(anchorPoint);
            SetPositionsOnScreen(position);
        }

        void SetAnchorPoint(const AnchorPoints anchorPoint) {
            m_anchorPoint = anchorPoint;
        }

        void SetPositionsOnScreen(const IntVector2 position)
            // Function adds position of each square on the screen to it's attributes so other methods can access it.
        {
            IntVector2 offset = { 0,0 }; // Default for top left anchor point;
            IntVector2 pixelDimensions = GetBoardPixelDimensions();

            switch (m_anchorPoint)
            {
            case TOP_MIDDLE: offset.x = int(pixelDimensions.x / 2); break;
            case TOP_RIGHT: offset.x = pixelDimensions.x; break;
            case MIDDLE_LEFT: offset.y = int(pixelDimensions.y / 2); break;

            case MIDDLE:
                offset.x = int(pixelDimensions.x / 2);
                offset.y = int(pixelDimensions.y / 2);
                break;

            case MIDDLE_RIGHT:
                offset.x = pixelDimensions.x;
                offset.y = int(pixelDimensions.y / 2);
                break;

            case BOTTOM_LEFT:
                offset.y = pixelDimensions.y;
                break;

            case BOTTOM_MIDDLE:
                offset.x = int(pixelDimensions.x / 2);
                offset.y = pixelDimensions.y;
                break;

            case BOTTOM_RIGHT:
                offset.x = pixelDimensions.x;
                offset.y = pixelDimensions.y;
                break;

            default:
                break;
            }

            for (int y = 0; y < m_grid.size(); y++)
            {
                for (int x = 0; x < m_grid[y].size(); x++)
                {
                    T_Entity entity = m_grid[y][x];
                    int xPos = (x * (entity.GetMarginWidth() + entity.GetWidth())) + position.x - offset.x;
                    int yPos = (y * (entity.GetMarginHeight() + entity.GetHeight())) + position.y - offset.y;

                    m_grid[y][x].SetPositionOnScreen(xPos, yPos);
                }
            }
        }

        void DisplayGrid() const
        {

            for (int y = 0; y < m_grid.size(); y++)
            {
                for (int x = 0; x < m_grid[y].size(); x++)
                {
                    if (ShouldRenderEntity(IntVector2{ x, y }))
                    {
                        m_grid[y][x].Render();
                    }
                }
            }
        }

        virtual void ProcessMouseInput()
        {

        }

        std::vector<T_Entity> GetNeighbours(const T_Entity& tile) const
        {
            std::vector<T_Entity> neighbours;
            int x = tile.GetGridCoords().x;
            int y = tile.GetGridCoords().y;

            const int dx[] = { -1, -1, -1,  0, 0,  1, 1, 1 };
            const int dy[] = { -1,  0,  1, -1, 1, -1, 0, 1 };

            for (int i = 0; i < 8; ++i)
            {
                int newX = x + dx[i];
                int newY = y + dy[i];
                if (newX >= 0 && newX < m_grid[0].size() &&
                    newY >= 0 && newY < m_grid.size())
                {
                    neighbours.push_back(m_grid[newY][newX]);
                }
            }

            return neighbours;
        }
    };
};

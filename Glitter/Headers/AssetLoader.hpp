#pragma once

#include "Shader.hpp"
#include "Mesh.hpp"

/// <summary>
/// Asset representation with name and mesh data
/// </summary>
struct Asset
{
    std::string m_name;
    std::unique_ptr<Mesh> m_mesh;
};

class AssetLoader
{
public:
    /// <summary>
    /// Asset Loader that contains a list of found assets (using the Load function to add assets by pattern)
    /// </summary>
    AssetLoader();

    /// <summary>
    /// Load an asset with its shader using an expression.
    /// </summary>
    /// <param name="expr">Expr to search for, can contain wildcard characters</param>
    /// <param name="shader">Shader to use with Asset</param>
    void Load(std::string const& expr, Shader& shader);

    /// <summary>
    /// Get the list of loaded assets
    /// </summary>
    /// <returns>A vector containing all loaded assets</returns>
    inline std::vector<std::unique_ptr<Asset>> const& Get()
    {
        return m_assets;
    }

private:
    std::vector<std::unique_ptr<Asset>> m_assets;
};


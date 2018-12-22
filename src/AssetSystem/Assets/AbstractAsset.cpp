// C++ STL
#include <algorithm>

// Editor
#include <AssetSystem/Assets/AbstractAsset.hpp>

// ImGui
#include <imgui.h>

HG::Editor::AssetSystem::Assets::AbstractAsset::AbstractAsset(std::filesystem::path path, std::size_t type) :
    m_assetManager(nullptr),
    m_parent(nullptr),
    m_children(),
    m_state(State::NotLoaded),
    m_stateMutex(),
    m_path(std::move(path)),
    m_type(type)
{

}

HG::Editor::AssetSystem::Assets::AbstractAsset::~AbstractAsset()
{
    if (m_parent)
    {
        m_parent->m_children.erase(
            std::find_if(
                m_parent->m_children.begin(),
                m_parent->m_children.end(),
                [this](const AssetPtr& asset)
                {
                    return asset.get() == this;
                }
            )
        );
    }

    m_children.clear();
}

void HG::Editor::AssetSystem::Assets::AbstractAsset::setAssetsManager(HG::Editor::AssetSystem::AssetsManager *manager)
{
    m_assetManager = manager;
}

HG::Editor::AssetSystem::AssetsManager *HG::Editor::AssetSystem::Assets::AbstractAsset::assetsManager() const
{
    return m_assetManager;
}

std::string HG::Editor::AssetSystem::Assets::AbstractAsset::name() const
{
    return m_path.filename();
}

std::filesystem::path HG::Editor::AssetSystem::Assets::AbstractAsset::path() const
{
    return m_path;
}

std::size_t HG::Editor::AssetSystem::Assets::AbstractAsset::type() const
{
    return m_type;
}

HG::Editor::AssetSystem::Assets::AbstractAsset::State HG::Editor::AssetSystem::Assets::AbstractAsset::state() const
{
    std::shared_lock<std::shared_mutex> lock(m_stateMutex);

    return m_state;
}

HG::Editor::AssetSystem::Assets::AbstractAsset *HG::Editor::AssetSystem::Assets::AbstractAsset::parent() const
{
    return m_parent;
}

std::vector<HG::Editor::AssetSystem::Assets::AssetPtr> &HG::Editor::AssetSystem::Assets::AbstractAsset::children()
{
    return m_children;
}

void HG::Editor::AssetSystem::Assets::AbstractAsset::addChild(HG::Editor::AssetSystem::Assets::AssetPtr child)
{
    if (child->m_parent)
    {
        child->m_parent->m_children.erase(
            std::find_if(
                child->m_parent->m_children.begin(),
                child->m_parent->m_children.end(),
                [this](const AssetPtr& asset)
                {
                    return asset.get() == this;
                }
            )
        );
    }

    child->m_parent = this;

    m_children.emplace_back(std::move(child));
}

bool HG::Editor::AssetSystem::Assets::AbstractAsset::load()
{
    setState(State::Loading);

    auto result = onLoad();

    setState(result ? State::Loaded : State::Failed);

    return result;
}

void HG::Editor::AssetSystem::Assets::AbstractAsset::postLoad()
{
    onPostLoad();
}

void HG::Editor::AssetSystem::Assets::AbstractAsset::setState(HG::Editor::AssetSystem::Assets::AbstractAsset::State state)
{
    std::unique_lock<std::shared_mutex> lock(m_stateMutex);
    m_state = state;
}

HG::Editor::ThumbnailsCache::Handle HG::Editor::AssetSystem::Assets::AbstractAsset::icon() const
{
    return HG::Editor::ThumbnailsCache::InvalidHandle;
}

bool HG::Editor::AssetSystem::Assets::AbstractAsset::onLoad()
{
    return false;
}

void HG::Editor::AssetSystem::Assets::AbstractAsset::onPostLoad()
{

}

void HG::Editor::AssetSystem::Assets::AbstractAsset::onInspector()
{

}

bool HG::Editor::AssetSystem::Assets::AbstractAsset::onTreeItemDraw(int flags)
{
    return ImGui::TreeNodeEx(this, flags, "%s", name().c_str());
}

// Editor
#include <Widgets/Inspector.hpp>
#include <Widgets/CommonSettings.hpp>
#include <Editor/Application.hpp>
#include <Fabrics/PropertyEditorsFabric.hpp>
#include <AbstractPropertyProcessor.hpp>
#include <Tools/ImGuiWidgets.hpp>

// HG::Core
#include <HG/Core/GameObject.hpp>
#include <HG/Core/Transform.hpp>

// ImGui
#include <imgui.h>

// ALogger
#include <SystemTools.h>
#include <Tools/ImGuiIdentificators.hpp>

HG::Editor::Widgets::Inspector::Inspector(HG::Editor::Widgets::Settings::Common *settings) :
    m_commonSettings(settings)
{

}

void HG::Editor::Widgets::Inspector::onDraw()
{
    ImGui::IDGuard idGuard(HG::ID::Inspector::Window);

    if (ImGui::Begin(HG::Names::Inspector::Window, &m_opened))
    {
        switch (m_commonSettings->lastSelectedType)
        {
        case HG::Editor::Widgets::Settings::Common::LastSelectedType::GameObject:
            drawGameObjectBody();
            break;
        case HG::Editor::Widgets::Settings::Common::LastSelectedType::Asset:
            drawAssetBody();
            break;
        case HG::Editor::Widgets::Settings::Common::LastSelectedType::None:
            break;
        }
    }

    ImGui::End();
}

void HG::Editor::Widgets::Inspector::drawGameObjectBody()
{
    auto gameObject = m_commonSettings->selectedGameObject;

    if (m_commonSettings->selectedGameObject == nullptr)
    {
        return;
    }

    // Displaying gameobject header
    bool enabledCache = gameObject->isEnabled();
    if (ImGui::IDGuard(HG::ID::Inspector::EnabledCheckbox),
        ImGui::Checkbox(HG::Names::Inspector::EnabledCheckbox, &enabledCache))
    {
        gameObject->setEnabled(enabledCache);
    }

    ImGui::SameLine();

    ImGui::Text("%s", gameObject->name().c_str());

    // todo: Don't invalidate cache if current gameobject was not changed
    //       or if behaviours amount was not changed.
    m_commonSettings->behavioursCache.clear();

    // Getting and iterating all behaviours
    gameObject->getBehaviours(m_commonSettings->behavioursCache);
    gameObject->getRenderingBehaviours(m_commonSettings->behavioursCache);

    // Getting Editor::Application
    auto editorApplication = application();

    if (editorApplication == nullptr)
    {
        ImGui::Text("%s", "This application has wrong type.");
        return;
    }

    drawTransformEdit(gameObject->transform());

    for (const auto& behaviour : m_commonSettings->behavioursCache)
    {
        if (ImGui::IDGuard(HG::ID::GameObject::Header),
            ImGui::CollapsingHeader(SystemTools::getTypeName(*behaviour).c_str()))
        {
            m_commonSettings->propertiesCache.clear();

            // Getting and iterating all properties
            behaviour->getProperties(m_commonSettings->propertiesCache);

            for (const auto& property : m_commonSettings->propertiesCache)
            {
                // Displaying title
                auto processor = editorApplication
                    ->propertyEditorsFabric()
                    ->create(property.typeInfo().hash_code());

                if (processor == nullptr)
                {
                    ImGui::Text("No processor for \"%s\" type (%s)", property.type().c_str(), property.name().c_str());
                    continue;
                }

                ImGui::IDGuard idGuard((void*) &property);

                processor->perform(property.name(), property);
            }
        }
    }
}

void HG::Editor::Widgets::Inspector::drawTransformEdit(HG::Core::Transform* transform)
{
    // Position
    int id = 0;

    auto tempVec3 = transform->globalPosition();
    if (ImGui::IDGuard(++id),
        ImGui::DragFloat3("Position", tempVec3.data.data, 0.05))
    {
        transform->setGlobalPosition(tempVec3);
    }

    tempVec3 = glm::eulerAngles(transform->localRotation());
    if (ImGui::IDGuard(++id),
        ImGui::DragFloat3("Rotation", tempVec3.data.data, 0.05))
    {
        transform->setGlobalRotation(glm::quat(tempVec3));
    }

    tempVec3 = transform->localScale();
    if (ImGui::IDGuard(++id),
        ImGui::DragFloat3("Scale", tempVec3.data.data, 0.05))
    {
        transform->setLocalScale(tempVec3);
    }
}

void HG::Editor::Widgets::Inspector::drawAssetBody()
{

}

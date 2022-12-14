/*
 * All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
 * its licensors.
 *
 * For complete copyright and license terms please see the LICENSE at the root of this
 * distribution (the "License"). All use of this software is governed by the License,
 * or, if provided, by the license below or the license accompanying this file. Do not
 * remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 */

#include "WhiteBox_precompiled.h"

#include "Asset/WhiteBoxMeshAsset.h"
#include "Asset/WhiteBoxMeshAssetHandler.h"
#include "Rendering/Legacy/WhiteBoxLegacyRenderMesh.h"
#include "Rendering/Legacy/WhiteBoxLegacyMaterials.h"
#include "WhiteBoxSystemComponent.h"
#include "WhiteBoxToolApiReflection.h"

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace WhiteBox
{
    struct Impl
    {
        //! Look-up table for White Box materials.
        WhiteBoxLegacyMaterials m_whiteBoxLegacyMaterials;
    };

    void WhiteBoxSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<WhiteBoxSystemComponent, AZ::Component>()->Version(0);

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<WhiteBoxSystemComponent>(
                      "WhiteBox", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System", 0xc94d118b))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }
#ifdef WHITE_BOX_EDITOR
        WhiteBox::Reflect(context);
#endif
    }

    WhiteBoxSystemComponent::WhiteBoxSystemComponent() = default;
    WhiteBoxSystemComponent::~WhiteBoxSystemComponent() = default;

    void WhiteBoxSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("WhiteBoxService", 0x2f2f42b8));
    }

    void WhiteBoxSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("WhiteBoxService", 0x2f2f42b8));
    }

    void WhiteBoxSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& /*required*/) {}

    void WhiteBoxSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& /*dependent*/) {}

    template<typename AssetHandlerT, typename AssetT>
    void RegisterAsset(AZStd::vector<AZStd::unique_ptr<AZ::Data::AssetHandler>>& assetHandlers)
    {
        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequests::EnableCatalogForAsset, AZ::AzTypeInfo<AssetT>::Uuid());
        AZ::Data::AssetCatalogRequestBus::Broadcast(
            &AZ::Data::AssetCatalogRequests::AddExtension, AssetHandlerT::AssetFileExtension);

        assetHandlers.emplace_back(AZStd::make_unique<AssetHandlerT>());
    }

    void WhiteBoxSystemComponent::Activate()
    {
        WhiteBoxRequestBus::Handler::BusConnect();
#ifdef WHITE_BOX_EDITOR
        RegisterAsset<Pipeline::WhiteBoxMeshAssetHandler, Pipeline::WhiteBoxMeshAsset>(m_assetHandlers);
#endif
        m_impl = AZStd::make_unique<Impl>();
        m_impl->m_whiteBoxLegacyMaterials.Connect();
    }

    void WhiteBoxSystemComponent::Deactivate()
    {
        m_impl->m_whiteBoxLegacyMaterials.Disconnect();
        m_impl.reset();

        WhiteBoxRequestBus::Handler::BusDisconnect();
        m_assetHandlers.clear();
    }

    AZStd::unique_ptr<RenderMeshInterface> WhiteBoxSystemComponent::CreateRenderMeshInterface()
    {
        // #if LEGACY
        return AZStd::make_unique<LegacyRenderMesh>();
        // #else if ATOM
        // return AZStd::make_unique<AtomRenderMesh>();
        // #endif
    }
} // namespace WhiteBox

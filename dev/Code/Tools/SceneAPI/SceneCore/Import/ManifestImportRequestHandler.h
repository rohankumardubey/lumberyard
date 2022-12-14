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

#pragma once

#include <SceneAPI/SceneCore/Components/BehaviorComponent.h>
#include <SceneAPI/SceneCore/Events/AssetImportRequest.h>

namespace AZ
{
    namespace SceneAPI
    {
        namespace Import
        {
            class ManifestImportRequestHandler
                : public SceneCore::BehaviorComponent
                , public Events::AssetImportRequestBus::Handler
            {
            public:
                AZ_COMPONENT(ManifestImportRequestHandler, "{6CF0520E-D5A9-4003-81A5-F20D62010E6F}", SceneCore::BehaviorComponent);

                ~ManifestImportRequestHandler() override = default;

                void Activate() override;
                void Deactivate() override;
                static void Reflect(ReflectContext* context);

                void GetManifestExtension(AZStd::string& result) override;
                Events::LoadingResult LoadAsset(Containers::Scene& scene, const AZStd::string& path, const Uuid& guid,
                    RequestingApplication requester) override;
                
            private:
                static const char* s_extension;
                static const char* s_generated;
            };
        } // namespace Import
    } // namespace SceneAPI
} // namespace AZ

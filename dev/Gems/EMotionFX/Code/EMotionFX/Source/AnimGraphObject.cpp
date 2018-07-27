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

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <MCore/Source/IDGenerator.h>
#include <MCore/Source/AttributeFactory.h>
#include <MCore/Source/AttributePool.h>
#include <MCore/Source/MCoreSystem.h>
#include <MCore/Source/ReflectionSerializer.h>
#include <EMotionFX/Source/AnimGraphBus.h>
#include "EMotionFXConfig.h"
#include "AnimGraphObject.h"
#include "AnimGraphAttributeTypes.h"
#include "AnimGraphInstance.h"
#include "AnimGraphManager.h"
#include "EMotionFXManager.h"
#include "AnimGraph.h"


namespace EMotionFX
{
    AZ_CLASS_ALLOCATOR_IMPL(AnimGraphObject, AnimGraphAllocator, 0)

    AnimGraphObject::AnimGraphObject()
        : mAnimGraph(nullptr)
        , mObjectIndex(MCORE_INVALIDINDEX32)
    {
    }


    AnimGraphObject::AnimGraphObject(AnimGraph* animGraph)
        : AnimGraphObject()
    {
        mAnimGraph = animGraph;
    }


    AnimGraphObject::~AnimGraphObject()
    {
    }


    // unregister itself
    void AnimGraphObject::Unregister()
    {
    }


    // construct and output the information summary string for this object
    void AnimGraphObject::GetSummary(AZStd::string* outResult) const
    {
        *outResult = AZStd::string::format("%s: %s", RTTI_GetTypeName(), MCore::ReflectionSerializer::Serialize(this).GetValue().c_str());
    }


    // construct and output the tooltip for this object
    void AnimGraphObject::GetTooltip(AZStd::string* outResult) const
    {
        *outResult = AZStd::string::format("%s: %s", RTTI_GetTypeName(), MCore::ReflectionSerializer::Serialize(this).GetValue().c_str());
    }


    const char* AnimGraphObject::GetHelpUrl() const
    {
        return "";
    }


    // save and return number of bytes written, when outputBuffer is nullptr only return num bytes it would write
    uint32 AnimGraphObject::SaveUniqueData(const AnimGraphInstance* animGraphInstance, uint8* outputBuffer) const
    {
        AnimGraphObjectData* data = animGraphInstance->FindUniqueObjectData(this);
        if (data)
        {
            return data->Save(outputBuffer);
        }

        return 0;
    }



    // load and return number of bytes read, when dataBuffer is nullptr, 0 should be returned
    uint32 AnimGraphObject::LoadUniqueData(const AnimGraphInstance* animGraphInstance, const uint8* dataBuffer)
    {
        AnimGraphObjectData* data = animGraphInstance->FindUniqueObjectData(this);
        if (data)
        {
            return data->Load(dataBuffer);
        }

        return 0;
    }


    // collect internal objects
    void AnimGraphObject::RecursiveCollectObjects(MCore::Array<AnimGraphObject*>& outObjects) const
    {
        outObjects.Add(const_cast<AnimGraphObject*>(this));
    }


    // on default create a base class object
    void AnimGraphObject::OnUpdateUniqueData(AnimGraphInstance* animGraphInstance)
    {
        // try to find existing data
        AnimGraphObjectData* data = animGraphInstance->FindUniqueObjectData(this);
        if (data == nullptr) // doesn't exist
        {
            AnimGraphObjectData* newData = aznew AnimGraphObjectData(this, animGraphInstance);
            animGraphInstance->RegisterUniqueObjectData(newData);
        }
    }


    void AnimGraphObject::UpdateUniqueDatas()
    {
        if (mAnimGraph)
        {
            mAnimGraph->UpdateUniqueData();
        }
    }


    // reset the unique data
    void AnimGraphObject::ResetUniqueData(AnimGraphInstance* animGraphInstance)
    {
        AnimGraphObjectData* uniqueData = animGraphInstance->FindUniqueObjectData(this);
        if (uniqueData)
        {
            uniqueData->Reset();
        }
    }


    // default update implementation
    void AnimGraphObject::Update(AnimGraphInstance* animGraphInstance, float timePassedInSeconds)
    {
        MCORE_UNUSED(timePassedInSeconds);
        animGraphInstance->EnableObjectFlags(mObjectIndex, AnimGraphInstance::OBJECTFLAGS_UPDATE_READY);
    }


    // check for an error
    bool AnimGraphObject::GetHasErrorFlag(AnimGraphInstance* animGraphInstance) const
    {
        AnimGraphObjectData* uniqueData = animGraphInstance->FindUniqueObjectData(this);
        if (uniqueData)
        {
            return uniqueData->GetHasError();
        }
        else
        {
            return false;
        }
    }


    // set the error flag
    void AnimGraphObject::SetHasErrorFlag(AnimGraphInstance* animGraphInstance, bool hasError)
    {
        AnimGraphObjectData* uniqueData = animGraphInstance->FindUniqueObjectData(this);
        if (uniqueData == nullptr)
        {
            return;
        }

        uniqueData->SetHasError(hasError);
    }

    // init the internal attributes
    void AnimGraphObject::InitInternalAttributes(AnimGraphInstance* animGraphInstance)
    {
        MCORE_UNUSED(animGraphInstance);
        // currently base objects do not do this, but will come later
    }


    // remove the internal attributes
    void AnimGraphObject::RemoveInternalAttributesForAllInstances()
    {
        // currently base objects themselves do not have internal attributes yet, but this will come at some stage
    }


    // decrease internal attribute indices for index values higher than the specified parameter
    void AnimGraphObject::DecreaseInternalAttributeIndices(uint32 decreaseEverythingHigherThan)
    {
        MCORE_UNUSED(decreaseEverythingHigherThan);
        // currently no implementation for the base object type, but this will come later
    }


    // does the init for all anim graph instances in the parent animgraph
    void AnimGraphObject::InitInternalAttributesForAllInstances()
    {
        if (mAnimGraph == nullptr)
        {
            return;
        }

        const size_t numInstances = mAnimGraph->GetNumAnimGraphInstances();
        for (size_t i = 0; i < numInstances; ++i)
        {
            InitInternalAttributes(mAnimGraph->GetAnimGraphInstance(i));
        }
    }


    void AnimGraphObject::SyncVisualObject()
    {
        AnimGraphNotificationBus::Broadcast(&AnimGraphNotificationBus::Events::OnSyncVisualObject, this);
    }


    void AnimGraphObject::CalculateMotionExtractionDelta(EExtractionMode extractionMode, AnimGraphRefCountedData* sourceRefData, AnimGraphRefCountedData* targetRefData, float weight, bool hasMotionExtractionNodeInMask, Transform& outTransform, Transform& outTransformMirrored)
    {
        // Calculate the motion extraction output based on the motion extraction mode.
        switch (extractionMode)
        {
            // Blend between the source and target.
            case EXTRACTIONMODE_BLEND:
            {
                if (hasMotionExtractionNodeInMask)
                {
                    if (weight < MCore::Math::epsilon || !targetRefData)     // Weight is 0.
                    {
                        outTransform = sourceRefData->GetTrajectoryDelta();
                        outTransformMirrored = sourceRefData->GetTrajectoryDeltaMirrored();
                    }
                    else if (weight < 1.0f - MCore::Math::epsilon)     // Weight between 0 and 1.
                    {
                        outTransform = sourceRefData->GetTrajectoryDelta();
                        outTransform.Blend(targetRefData->GetTrajectoryDelta(), weight);
                        outTransformMirrored = sourceRefData->GetTrajectoryDeltaMirrored();
                        outTransformMirrored.Blend(targetRefData->GetTrajectoryDeltaMirrored(), weight);
                    }
                    else     // Weight is 1.
                    {
                        outTransform = targetRefData->GetTrajectoryDelta();
                        outTransformMirrored = targetRefData->GetTrajectoryDeltaMirrored();
                    }
                }
                else
                {
                    outTransform = sourceRefData->GetTrajectoryDelta();
                    outTransformMirrored = sourceRefData->GetTrajectoryDeltaMirrored();
                }
            }
            break;

            // Output only the target state's delta.
            case EXTRACTIONMODE_TARGETONLY:
            {
                if (hasMotionExtractionNodeInMask)
                {
                    if (targetRefData)
                    {
                        outTransform = targetRefData->GetTrajectoryDelta();
                        outTransformMirrored = targetRefData->GetTrajectoryDeltaMirrored();
                    }
                    else
                    {
                        outTransform = sourceRefData->GetTrajectoryDelta();
                        outTransformMirrored = sourceRefData->GetTrajectoryDeltaMirrored();
                    }
                }
                else
                {
                    outTransform.ZeroWithIdentityQuaternion();
                    outTransformMirrored.ZeroWithIdentityQuaternion();
                }
            }
            break;

            // Output only the source state's delta.
            case EXTRACTIONMODE_SOURCEONLY:
            {
                outTransform = sourceRefData->GetTrajectoryDelta();
                outTransformMirrored = sourceRefData->GetTrajectoryDeltaMirrored();
            }
            break;

            default:
                AZ_Assert(false, "Unknown motion extraction mode used.");
        }
    }


    void AnimGraphObject::CalculateMotionExtractionDeltaAdditive(EExtractionMode extractionMode, AnimGraphRefCountedData* sourceRefData, AnimGraphRefCountedData* targetRefData, Transform basePoseTransform, float weight, bool hasMotionExtractionNodeInMask, Transform& outTransform, Transform& outTransformMirrored)
    {
        if (!hasMotionExtractionNodeInMask)
        {
            outTransform = sourceRefData->GetTrajectoryDelta();
            outTransformMirrored = sourceRefData->GetTrajectoryDeltaMirrored();
            return;
        }

        // Calculate the motion extraction output based on the motion extraction mode.
        switch (extractionMode)
        {
            // Blend between the source and target.
            case EXTRACTIONMODE_BLEND:
            {
                if (weight < MCore::Math::epsilon || !targetRefData)     // Weight is 0 or there is no target ref data.
                {
                    outTransform = sourceRefData->GetTrajectoryDelta();
                    outTransformMirrored = sourceRefData->GetTrajectoryDeltaMirrored();
                }
                else     // Weight between 0 and 1.
                {
                    outTransform = sourceRefData->GetTrajectoryDelta();
                    outTransform.BlendAdditive(targetRefData->GetTrajectoryDelta(), basePoseTransform, weight);
                    outTransformMirrored = sourceRefData->GetTrajectoryDeltaMirrored();
                    outTransformMirrored.BlendAdditive(targetRefData->GetTrajectoryDeltaMirrored(), basePoseTransform, weight);
                }
            }
            break;

            // Output only the target state's delta if it is available.
            case EXTRACTIONMODE_TARGETONLY:
            {
                if (targetRefData)
                {
                    outTransform = sourceRefData->GetTrajectoryDelta();
                    outTransform.BlendAdditive(targetRefData->GetTrajectoryDelta(), basePoseTransform, 1.0f);       // TODO: could eliminate the lerp internally
                    outTransformMirrored = sourceRefData->GetTrajectoryDeltaMirrored();
                    outTransformMirrored.BlendAdditive(targetRefData->GetTrajectoryDeltaMirrored(), basePoseTransform, 1.0f);       // TODO: could eliminate the lerp internally
                }
                else
                {
                    outTransform = sourceRefData->GetTrajectoryDelta();
                    outTransformMirrored = sourceRefData->GetTrajectoryDeltaMirrored();
                }
            }
            break;

            // Output only the source state's delta.
            case EXTRACTIONMODE_SOURCEONLY:
            {
                outTransform = sourceRefData->GetTrajectoryDelta();
                outTransformMirrored = sourceRefData->GetTrajectoryDeltaMirrored();
            }
            break;

            default:
                AZ_Assert(false, "Unknown motion extraction mode used.");
        }
    }


    void AnimGraphObject::Reflect(AZ::ReflectContext* context)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }

        serializeContext->Class<AnimGraphObject>()
            ->Version(1);


        AZ::EditContext* editContext = serializeContext->GetEditContext();
        if (!editContext)
        {
            return;
        }

        editContext->Enum<ESyncMode>("Sync Mode", "The synchronization method to use. Event track based will use event tracks, full clip based will ignore the events and sync as a full clip. If set to Event Track Based while no sync events exist inside the track a full clip based sync will be performed instead.")
            ->Value("Disabled", SYNCMODE_DISABLED)
            ->Value("Event Track Based", SYNCMODE_TRACKBASED)
            ->Value("Full Clip Based", SYNCMODE_CLIPBASED);

        editContext->Enum<EEventMode>("Event Filter Mode", "The event filter mode, which controls which events are passed further up the hierarchy.")
            ->Value("Master Node Only", EVENTMODE_MASTERONLY)
            ->Value("Servant Node Only", EVENTMODE_SLAVEONLY)
            ->Value("Both Nodes", EVENTMODE_BOTHNODES)
            ->Value("Most Active", EVENTMODE_MOSTACTIVE);

        editContext->Enum<EExtractionMode>("Extraction Mode", "The motion extraction blend mode to use.")
            ->Value("Blend", EXTRACTIONMODE_BLEND)
            ->Value("Target Only", EXTRACTIONMODE_TARGETONLY)
            ->Value("Source Only", EXTRACTIONMODE_SOURCEONLY);
    }
} // namespace EMotionFX
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

// include required headers
#include "AttachmentsHierarchyWindow.h"
#include <EMotionFX/CommandSystem/Source/CommandManager.h>
#include <EMotionFX/Source/Attachment.h>
#include <EMotionFX/Source/ActorManager.h>
#include "../../../../EMStudioSDK/Source/EMStudioManager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>


namespace EMStudio
{
    // constructor
    AttachmentsHierarchyWindow::AttachmentsHierarchyWindow(QWidget* parent)
        : QWidget(parent)
    {
        mHierarchy = nullptr;
    }


    // destructor
    AttachmentsHierarchyWindow::~AttachmentsHierarchyWindow()
    {
    }


    void AttachmentsHierarchyWindow::Init()
    {
        QVBoxLayout* verticalLayout = new QVBoxLayout();
        verticalLayout->setMargin(0);
        setLayout(verticalLayout);

        mHierarchy = new QTreeWidget();
        verticalLayout->addWidget(mHierarchy);
        mHierarchy->setColumnCount(1);
        mHierarchy->setHeaderHidden(true);

        // set optical stuff for the tree
        mHierarchy->setColumnWidth(0, 200);
        mHierarchy->setColumnWidth(1, 20);
        mHierarchy->setColumnWidth(1, 100);
        mHierarchy->setSortingEnabled(false);
        mHierarchy->setSelectionMode(QAbstractItemView::NoSelection);
        mHierarchy->setMinimumWidth(150);
        mHierarchy->setMinimumHeight(125);
        mHierarchy->setAlternatingRowColors(true);
        mHierarchy->setExpandsOnDoubleClick(true);
        mHierarchy->setAnimated(true);

        // disable the move of section to have column order fixed
        mHierarchy->header()->setSectionsMovable(false);

        verticalLayout->addWidget(mHierarchy);

        ReInit();
    }


    void AttachmentsHierarchyWindow::ReInit()
    {
        // clear the tree
        mHierarchy->clear();

        // get the number of actor instances and iterate through them
        const uint32 numActorInstances = EMotionFX::GetActorManager().GetNumActorInstances();
        for (uint32 i = 0; i < numActorInstances; ++i)
        {
            EMotionFX::ActorInstance*   actorInstance = EMotionFX::GetActorManager().GetActorInstance(i);

            if (actorInstance->GetIsOwnedByRuntime())
            {
                continue;
            }

            EMotionFX::ActorInstance*   attachedTo      = actorInstance->GetAttachedTo();
            EMotionFX::Actor*           actor           = actorInstance->GetActor();

            // check if we are dealing with a root actor instance
            if (attachedTo == nullptr)
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(mHierarchy);
                AZStd::string actorFilename;
                AzFramework::StringFunc::Path::GetFileName(actor->GetFileNameString().c_str(), actorFilename);
                item->setText(0, QString("%1 (ID:%2)").arg(actorFilename.c_str()).arg(actorInstance->GetID()));
                item->setExpanded(true);
                mHierarchy->addTopLevelItem(item);

                // get the number of attachments and iterate through them
                const uint32 numAttachments = actorInstance->GetNumAttachments();
                for (uint32 j = 0; j < numAttachments; ++j)
                {
                    EMotionFX::Attachment* attachment = actorInstance->GetAttachment(j);
                    MCORE_ASSERT(actorInstance == attachment->GetAttachToActorInstance());

                    // recursively go through all attachments
                    RecursivelyAddAttachments(item, attachment->GetAttachmentActorInstance());
                }
            }
        }
    }


    void AttachmentsHierarchyWindow::RecursivelyAddAttachments(QTreeWidgetItem* parent, EMotionFX::ActorInstance* actorInstance)
    {
        // get the actor from the actor instance
        EMotionFX::Actor* actor = actorInstance->GetActor();

        // add the current actor instance to the hierarchy
        QTreeWidgetItem* item = new QTreeWidgetItem(parent);
        AZStd::string actorFilename;
        AzFramework::StringFunc::Path::GetFileName(actor->GetFileNameString().c_str(), actorFilename);
        item->setText(0, QString("%1 (ID:%2)").arg(actorFilename.c_str()).arg(actorInstance->GetID()));
        item->setExpanded(true);
        parent->addChild(item);

        // get the number of attachments and iterate through them
        const uint32 numAttachments = actorInstance->GetNumAttachments();
        for (uint32 i = 0; i < numAttachments; ++i)
        {
            EMotionFX::Attachment* attachment = actorInstance->GetAttachment(i);
            MCORE_ASSERT(actorInstance == attachment->GetAttachToActorInstance());

            // recursively go through all attachments
            RecursivelyAddAttachments(item, attachment->GetAttachmentActorInstance());
        }
    }
} // namespace EMStudio

#include <EMotionFX/Tools/EMotionStudio/Plugins/StandardPlugins/Source/Attachments/AttachmentsHierarchyWindow.moc>
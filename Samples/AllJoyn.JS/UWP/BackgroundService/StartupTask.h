#pragma once

#include <ppltasks.h>
#include <agile.h>

namespace AllJoynJSBackgroundService
{
	[Windows::Foundation::Metadata::WebHostHidden]
    public ref class StartupTask sealed : public Windows::ApplicationModel::Background::IBackgroundTask
    {
    public:
        virtual void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);
    };
}

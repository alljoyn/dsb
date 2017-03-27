//
// 
// IN Copyright (c) %s, Microsoft Corporation
// IN 
// IN SPDX-License-Identifier: Apache-2.0 *
// IN 
// IN All rights reserved. This program and the accompanying materials are
// IN made available under the terms of the Apache License, Version 2.0
// IN which accompanies this distribution, and is available at
// IN http://www.apache.org/licenses/LICENSE-2.0
// IN 
// IN Permission to use, copy, modify, and/or distribute this software for
// IN any purpose with or without fee is hereby granted, provided that the
// IN above copyright notice and this permission notice appear in all
// IN copies.
// IN 
// IN THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// IN WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// IN WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// IN AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// IN DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// IN PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// IN TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// IN PERFORMANCE OF THIS SOFTWARE.
//

#pragma once

#include "pch.h"
#include "Models\BackgroundServiceOperationModel.h"
#include "Models\IServiceOperationModel.h"
#include "ViewModels\ServiceOperationViewModel.h"
#include "Views\ServiceOperationView.xaml.h"

namespace BackgroundHost { namespace Headed
{
    using namespace ::BackgroundHost::Headed::Models;
    using namespace ::BackgroundHost::Headed::ViewModels;
    using namespace ::BackgroundHost::Headed::Views;
    using namespace ::BackgroundHost;
    using namespace ::Windows::ApplicationModel::Activation;
    using namespace ::Windows::ApplicationModel::Resources;
    using namespace ::Windows::UI::Xaml;
    using namespace ::Windows::System::Profile;
    namespace wfm = ::Windows::Foundation::Metadata;

    [wfm::WebHostHidden]
    public ref class ApplicationImplementation sealed
    {
    public:
        ApplicationImplementation(String^ backgroundTaskEntryPointName)
            : m_backgroundTaskEntryPointName(backgroundTaskEntryPointName)
        {
            if (m_backgroundTaskEntryPointName->IsEmpty())
                throw ref new InvalidArgumentException(L"backgroundTaskType");
        }

    public:
        void OnLaunched(LaunchActivatedEventArgs^ args)
        {
            // Create the main view to be hosted in the window
            if (!Window::Current->Content)
            {
                // Create the Model
                auto serviceOperationModel = ref new BackgroundServiceOperationModel(m_backgroundTaskEntryPointName);

                // Create the ViewModel
                auto contentViewModel = ref new ServiceOperationViewModel(serviceOperationModel);

                // Create the View
                auto contentView = ref new ServiceOperationView();
                contentView->DataContext = contentViewModel;
                Window::Current->Content = contentView;

                // Set the View's application display title if needed
                if (contentView->ApplicationDisplayTitle->IsEmpty())
                {
                    auto resourceLoader = ResourceLoader::GetForCurrentView();
                    contentView->ApplicationDisplayTitle = resourceLoader->GetString("ApplicationDisplayName");
                }
            }

            // Ensure the current window is active
            Window::Current->Activate();
        }

    private:
        String^ m_backgroundTaskEntryPointName;
    };
}}
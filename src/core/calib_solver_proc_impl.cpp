// eKalibr, Copyright 2024, the School of Geodesy and Geomatics (SGG), Wuhan University, China
// https://github.com/Unsigned-Long/eKalibr.git
// Author: Shuolong Chen (shlchen@whu.edu.cn)
// GitHub: https://github.com/Unsigned-Long
//  ORCID: 0000-0002-5283-9057
// Purpose: See .h/.hpp file.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * The names of its contributors can not be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "core/calib_solver.h"
#include "core/sae.h"
#include "spdlog/spdlog.h"
#include "util/tqdm.h"
#include "config/configor.h"
#include "sensor/event.h"
#include "viewer/viewer.h"
#include "opencv4/opencv2/highgui.hpp"

namespace ns_ekalibr {

void CalibSolver::Process() {
    /**
     * load event data from the rosbag and align timestamps (temporal normalization)
     */
    spdlog::info("load event data from the rosbag and align timestamps...");
    this->LoadEventData();

    const double decay = Configor::Prior::DecayTimeOfActiveEvents;

    for (const auto &[topic, eventMes] : _evMes) {
        spdlog::info("perform norm-flow-based circle grid identification for camera '{}'", topic);

        const auto &config = Configor::DataStream::EventTopics.at(topic);
        auto sae = ActiveEventSurface::Create(config.Width, config.Height, 0.01);

        double lastUpdateTime = eventMes.front()->GetTimestamp();
        auto bar = std::make_shared<tqdm>();

        for (int i = 0; i < static_cast<int>(eventMes.size()); i++) {
            bar->progress(i, eventMes.size());

            for (const auto &event : eventMes.at(i)->GetEvents()) {
                sae->GrabEvent(event);
                const auto timeLatest = sae->GetTimeLatest();

                if (timeLatest - eventMes.front()->GetTimestamp() < 0.05 ||
                    timeLatest - lastUpdateTime < decay) {
                    continue;
                } else {
                    lastUpdateTime = timeLatest;
                }

                auto dts = sae->DecayTimeSurface(true, 0, decay);
                cv::imshow("Decay Surface Of Active Events", dts);

                cv::waitKey(0);
                _viewer->ClearViewer();
            }
        }
        bar->finish();
    }

    _solveFinished = true;
}
}  // namespace ns_ekalibr
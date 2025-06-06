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

#ifndef VIEWER_H
#define VIEWER_H

#include "tiny-viewer/core/viewer.h"
#include "tiny-viewer/entity/entity.h"
#include <opencv2/core/types.hpp>
#include "ctraj/core/spline_bundle.h"
#include "config/configor.h"

namespace ns_ekalibr {
struct EventArray;
using EventArrayPtr = std::shared_ptr<EventArray>;
struct Event;
using EventPtr = std::shared_ptr<Event>;
struct CalibParamManager;
using CalibParamManagerPtr = std::shared_ptr<CalibParamManager>;
struct CircleGrid3D;
using CircleGrid3DPtr = std::shared_ptr<CircleGrid3D>;

class Viewer : public ns_viewer::Viewer {
public:
    using Ptr = std::shared_ptr<Viewer>;
    using Parent = ns_viewer::Viewer;
    using So3SplineType = ns_ctraj::So3Spline<Configor::Prior::SplineOrder>;
    using PosSplineType = ns_ctraj::RdSpline<3, Configor::Prior::SplineOrder>;

private:
    std::list<std::size_t> _entities;
    int keptEntityCount;

    const std::vector<std::pair<So3SplineType, PosSplineType>> *_splineSegments;
    CalibParamManagerPtr _parMagr;
    CircleGrid3DPtr _grid3d;

public:
    explicit Viewer(int keptEntityCount = -1);

    static Ptr Create(int keptEntityCount = -1);

    Viewer &ClearViewer();

    Viewer &ResetViewerCamera();

    Viewer &PopBackEntity();

    Viewer &AddEntityLocal(const std::vector<ns_viewer::Entity::Ptr> &entities);

    // add discrete pos
    Viewer &AddSpatioTemporalTrace(const std::vector<Eigen::Vector3d> &trace,
                                   float size = 0.5f,
                                   const ns_viewer::Colour &color = ns_viewer::Colour::Blue(),
                                   const std::pair<float, float> &ptScales = {0.01f, 2.0f});

    Viewer &AddEventData(const std::vector<EventArrayPtr>::const_iterator &sIter,
                         const std::vector<EventArrayPtr>::const_iterator &eIter,
                         const std::pair<float, float> &ptScales = {0.01f, 2.0f},
                         float ptSize = 1.0f);

    Viewer &AddEventData(const EventArrayPtr &ary,
                         const std::pair<float, float> &ptScales = {0.01f, 2.0f},
                         const std::optional<ns_viewer::Colour> &color = {},
                         float ptSize = 1.0f);

    Viewer &AddEventData(const std::list<EventPtr> &ary,
                         const std::pair<float, float> &ptScales = {0.01f, 2.0f},
                         const std::optional<ns_viewer::Colour> &color = {},
                         float ptSize = 1.0f);

    Viewer &AddGridPattern(
        const std::vector<cv::Point2f> &centers,
        double timestamp,
        const std::pair<float, float> &ptScales = {0.01f, 2.0f},
        const ns_viewer::Colour &color = ns_viewer::Colour(1.0f, 1.0f, 0.0f, 1.0f),
        float ptSize = 0.05f);

    Viewer &AddGridPattern(
        const std::vector<cv::Point3f> &centers,
        float radius,
        const float &pScale = 0.01f,
        const ns_viewer::Colour &color = ns_viewer::Colour(1.0f, 1.0f, 0.0f, 1.0f),
        float ptSize = 0.05f);

    Viewer &UpdateViewer(const Sophus::SE3f &SE3_RefToWorld = {},
                         const float &pScale = Configor::Preference::SplineViewerSpatialScale,
                         double dt = 0.005);

    Viewer &AddSplineSegment(const std::pair<So3SplineType, PosSplineType> &spline,
                             const float &pScale = Configor::Preference::SplineViewerSpatialScale,
                             double dt = 0.005);

    void SetSpline(const std::vector<std::pair<So3SplineType, PosSplineType>> *splines);

    void SetParMgr(const CalibParamManagerPtr &parMgr);

    void SetGrid3D(const CircleGrid3DPtr &grid3d);

    void SetStates(const std::vector<std::pair<So3SplineType, PosSplineType>> *splines,
                   const CalibParamManagerPtr &parMgr,
                   const CircleGrid3DPtr &grid3d);

    void SetKeptEntityCount(int val = -1);

    ns_viewer::Entity::Ptr Gravity() const;

protected:
    ns_viewer::ViewerConfigor GenViewerConfigor();

    pangolin::OpenGlRenderState GetInitRenderState() const;

    void ZoomInSpatialScaleCallBack();

    void ZoomOutSpatialScaleCallBack();

    static void ZoomInTemporalScaleCallBack();

    static void ZoomOutTemporalScaleCallBack();
};
}  // namespace ns_ekalibr

#endif  // VIEWER_H

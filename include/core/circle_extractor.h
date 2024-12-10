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

#ifndef CIRCLE_EXTRACTOR_H
#define CIRCLE_EXTRACTOR_H

#include "Eigen/Dense"
#include "core/norm_flow.h"
#include "set"
#include "tiny-viewer/entity/utils.h"

namespace ns_ekalibr {

struct NormFlow;
using NormFlowPtr = std::shared_ptr<NormFlow>;
struct Viewer;
using ViewerPtr = std::shared_ptr<Viewer>;
struct Event;
using EventPtr = std::shared_ptr<Event>;
struct EventArray;
using EventArrayPtr = std::shared_ptr<EventArray>;

class EventCircleExtractor {
public:
    using Ptr = std::shared_ptr<EventCircleExtractor>;

    constexpr static double DEG2RAD = M_PI / 180.0;

    enum class CircleClusterType : int { CHASE = 0, RUN = 1, OTHER = 2 };

    struct CircleClusterInfo {
        using Ptr = std::shared_ptr<CircleClusterInfo>;

        std::list<NormFlowPtr> nfCluster;
        bool polarity;
        Eigen::Vector2d center;
        Eigen::Vector2d dir;

        CircleClusterInfo(const std::list<NormFlowPtr>& nf_cluster,
                          bool polarity,
                          const Eigen::Vector2d& center,
                          const Eigen::Vector2d& dir);

        static Ptr Create(const std::list<NormFlowPtr>& nf_cluster,
                          bool polarity,
                          const Eigen::Vector2d& center,
                          const Eigen::Vector2d& dir);
    };

    struct TimeVaryingCircle {
        using Ptr = std::shared_ptr<TimeVaryingCircle>;
        // center, radius
        using Circle = std::pair<Eigen::Vector2d, double>;

        double st, et;
        Eigen::Vector2d cx;
        Eigen::Vector2d cy;
        Eigen::Vector3d r2;

        TimeVaryingCircle(double st,
                          double et,
                          const Eigen::Vector2d& cx,
                          const Eigen::Vector2d& cy,
                          const Eigen::Vector3d& r2);

        static Ptr Create(double st,
                          double et,
                          const Eigen::Vector2d& cx,
                          const Eigen::Vector2d& cy,
                          const Eigen::Vector3d& r2);

        Eigen::Vector2d PosAt(double t) const;

        std::vector<Eigen::Vector3d> PosVecAt(double dt) const;

        double RadiusAt(double t) const;

        Circle CircleAt(double t) const { return {PosAt(t), RadiusAt(t)}; }

        double PointToCircleDistance(const EventPtr& event) const;
    };

protected:
    const double CLUSTER_AREA_THD;
    const double DIR_DIFF_DEG_THD;
    const double POINT_TO_CIRCLE_AVG_THD;

    // for visualization
    bool visualization;
    cv::Mat imgClusterNormFlowEvents;
    cv::Mat imgIdentifyCategory;
    cv::Mat imgSearchMatches;
    cv::Mat imgExtractCircles;
    cv::Mat imgExtractCirclesGrid;

public:
    EventCircleExtractor(bool visualization,
                         double CLUSTER_AREA_THD,
                         double DIR_DIFF_DEG_THD,
                         double POINT_TO_CIRCLE_AVG_THD)
        : CLUSTER_AREA_THD(CLUSTER_AREA_THD),
          DIR_DIFF_DEG_THD(DIR_DIFF_DEG_THD),
          POINT_TO_CIRCLE_AVG_THD(POINT_TO_CIRCLE_AVG_THD),
          visualization(visualization) {}

    static Ptr Create(bool visualization = false,
                      double CLUSTER_AREA_THD = 10.0,
                      double DIR_DIFF_DEG_THD = 30.0,
                      double POINT_TO_CIRCLE_AVG_THD = 1.0) {
        return std::make_shared<EventCircleExtractor>(visualization, CLUSTER_AREA_THD,
                                                      DIR_DIFF_DEG_THD, POINT_TO_CIRCLE_AVG_THD);
    }

    std::pair<double, std::vector<TimeVaryingCircle::Circle>> ExtractCircles(
        const EventNormFlow::NormFlowPack::Ptr& nfPack, const ViewerPtr& viewer = nullptr);

    std::optional<std::vector<cv::Point2f>> ExtractCirclesGrid(
        const EventNormFlow::NormFlowPack::Ptr& nfPack,
        const cv::Size& gridSize,
        const ViewerPtr& viewer = nullptr);

    void Visualization() const;

protected:
    std::vector<std::pair<EventArrayPtr, EventArrayPtr>> ExtractPotentialCircleClusters(
        const EventNormFlow::NormFlowPack::Ptr& nfPack,
        double CLUSTER_AREA_THD,
        double DIR_DIFF_DEG_THD);

    static TimeVaryingCircle::Ptr FitTimeVaryingCircle(const EventArrayPtr& ary1,
                                                       const EventArrayPtr& ary2,
                                                       double avgDistThd);

protected:
    static std::vector<std::pair<EventArrayPtr, EventArrayPtr>> RawEventsOfCircleClusterPairs(
        const std::map<CircleClusterInfo::Ptr, CircleClusterInfo::Ptr>& pairs,
        const EventNormFlow::NormFlowPack::Ptr& nfPack);

    /**
     * The functions 'SearchMatchesInRunChasePair', 'ReSearchMatchesCirclesOtherPair', and
     * 'ReSearchMatchesOtherOtherPair' are used to search for potential circular matching clusters.
     */
    static std::map<CircleClusterInfo::Ptr, CircleClusterInfo::Ptr> SearchMatchesInRunChasePair(
        const std::map<CircleClusterType, std::vector<CircleClusterInfo::Ptr>>& clusters,
        double DIR_DIFF_COS_THD);

    static std::map<CircleClusterInfo::Ptr, CircleClusterInfo::Ptr> ReSearchMatchesCirclesOtherPair(
        const std::map<CircleClusterType, std::vector<CircleClusterInfo::Ptr>>& clusters,
        const std::set<CircleClusterInfo::Ptr>& alreadyMatched,
        double DIR_DIFF_COS_THD);

    static std::map<CircleClusterInfo::Ptr, CircleClusterInfo::Ptr> ReSearchMatchesOtherOtherPair(
        const std::map<CircleClusterType, std::vector<CircleClusterInfo::Ptr>>& clusters,
        const std::set<CircleClusterInfo::Ptr>& alreadyMatched,
        double DIR_DIFF_COS_THD);

    static double TryMatchRunChaseCircleClusterPair(const CircleClusterInfo::Ptr& rCluster,
                                                    const CircleClusterInfo::Ptr& cCluster,
                                                    double DIR_DIFF_COS_THD);

    static bool ClusterExistsInCurCircle(
        const std::map<CircleClusterType, std::vector<CircleClusterInfo::Ptr>>& clusters,
        const CircleClusterInfo::Ptr& p1,
        const CircleClusterInfo::Ptr& p2);

    static void RemovingAmbiguousMatches(
        std::map<CircleClusterInfo::Ptr, CircleClusterInfo::Ptr>& pairs);

    /**
     * The classification of the clusters is determined as either the 'chase' category, the 'run'
     * category, or the 'other' category.
     */
    static std::vector<CircleClusterType> IdentifyCategory(
        const std::vector<std::list<NormFlowPtr>>& clusters,
        const std::vector<std::pair<Eigen::Vector2d, Eigen::Vector2d>>& cenDirs,
        const EventNormFlow::NormFlowPack::Ptr& nfPack);

    static CircleClusterType IdentifyCategory(
        const std::list<NormFlowPtr>& clusters,
        const std::pair<Eigen::Vector2d, Eigen::Vector2d>& cenDir,
        const EventNormFlow::NormFlowPack::Ptr& nfPack);

    static std::vector<std::pair<Eigen::Vector2d, Eigen::Vector2d>> ComputeCenterDir(
        const std::vector<std::list<NormFlowPtr>>& clusters,
        const EventNormFlow::NormFlowPack::Ptr& nfPack);

    static std::pair<Eigen::Vector2d, Eigen::Vector2d> ComputeCenterDir(
        const std::list<NormFlowPtr>& cluster, const EventNormFlow::NormFlowPack::Ptr& nfPack);

    /**
     * The following functions serve the purpose of 'ClusterNormFlowEvents'.
     */
    static std::pair<std::vector<std::list<NormFlowPtr>>, std::vector<std::list<NormFlowPtr>>>
    ClusterNormFlowEvents(const EventNormFlow::NormFlowPack::Ptr& nfPack, double clusterAreaThd);

    static void InterruptionInTimeDomain(cv::Mat& pMat, const cv::Mat& tMat, double thd);

    static void FilterContoursUsingArea(std::vector<std::vector<cv::Point>>& contours,
                                        double areaThd);

    static void FindContours(const cv::Mat& binaryImg,
                             std::vector<std::vector<cv::Point>>& contours);

protected:
    /**
     * The following functions are used for visualization.
     */
    static void DrawCircleCluster(
        cv::Mat& mat,
        const std::map<CircleClusterType, std::vector<CircleClusterInfo::Ptr>>& clusters,
        const EventNormFlow::NormFlowPack::Ptr& nfPack,
        double scale);

    static void DrawCircleClusterPair(
        cv::Mat& mat,
        const std::map<CircleClusterInfo::Ptr, CircleClusterInfo::Ptr>& pair,
        const EventNormFlow::NormFlowPack::Ptr& nfPack);

    static void DrawCircleCluster(cv::Mat& mat,
                                  const std::vector<CircleClusterInfo::Ptr>& clusters,
                                  CircleClusterType type,
                                  const EventNormFlow::NormFlowPack::Ptr& nfPack,
                                  double scale);

    static void DrawCluster(cv::Mat& mat,
                            const std::vector<std::list<NormFlowPtr>>& clusters,
                            const EventNormFlow::NormFlowPack::Ptr& nfPack);

    static void DrawCluster(cv::Mat& mat,
                            const std::list<NormFlowPtr>& clusters,
                            const EventNormFlow::NormFlowPack::Ptr& nfPack,
                            std::optional<ns_viewer::Colour> color = std::nullopt);

    static void DrawContours(cv::Mat& mat,
                             const std::vector<std::vector<cv::Point>>& contours,
                             const cv::Vec3b& color = {255, 255, 255});
};
}  // namespace ns_ekalibr

#endif  // CIRCLE_EXTRACTOR_H

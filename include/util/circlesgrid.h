/*M///////////////////////////////////////////////////////////////////////////////////////
 //
 //  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 //
 //  By downloading, copying, installing or using the software you agree to this license.
 //  If you do not agree to this license, do not download, install,
 //  copy or use the software.
 //
 //
 //                           License Agreement
 //                For Open Source Computer Vision Library
 //
 // Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
 // Copyright (C) 2009, Willow Garage Inc., all rights reserved.
 // Third party copyrights are property of their respective owners.
 //
 // Redistribution and use in source and binary forms, with or without modification,
 // are permitted provided that the following conditions are met:
 //
 //   * Redistribution's of source code must retain the above copyright notice,
 //     this list of conditions and the following disclaimer.
 //
 //   * Redistribution's in binary form must reproduce the above copyright notice,
 //     this list of conditions and the following disclaimer in the documentation
 //     and/or other materials provided with the distribution.
 //
 //   * The name of the copyright holders may not be used to endorse or promote products
 //     derived from this software without specific prior written permission.
 //
 // This software is provided by the copyright holders and contributors "as is" and
 // any express or implied warranties, including, but not limited to, the implied
 // warranties of merchantability and fitness for a particular purpose are disclaimed.
 // In no event shall the Intel Corporation or contributors be liable for any direct,
 // indirect, incidental, special, exemplary, or consequential damages
 // (including, but not limited to, procurement of substitute goods or services;
 // loss of use, data, or profits; or business interruption) however caused
 // and on any theory of liability, whether in contract, strict liability,
 // or tort (including negligence or otherwise) arising in any way out of
 // the use of this software, even if advised of the possibility of such damage.
 //
 //M*/

#ifndef EKALIBR_CIRCLESGRID_HPP
#define EKALIBR_CIRCLESGRID_HPP

#include <fstream>
#include <set>
#include <list>
#include <numeric>
#include <map>
#include <opencv2/core.hpp>
#include "opencv2/features2d.hpp"

namespace ns_cv_helper {

struct CV_EXPORTS_W_SIMPLE CirclesGridFinderParameters {
    CV_WRAP CirclesGridFinderParameters();
    CV_PROP_RW cv::Size2f densityNeighborhoodSize;
    CV_PROP_RW float minDensity;
    CV_PROP_RW int kmeansAttempts;
    CV_PROP_RW int minDistanceToAddKeypoint;
    CV_PROP_RW int keypointScale;
    CV_PROP_RW float minGraphConfidence;
    CV_PROP_RW float vertexGain;
    CV_PROP_RW float vertexPenalty;
    CV_PROP_RW float existingVertexGain;
    CV_PROP_RW float edgeGain;
    CV_PROP_RW float edgePenalty;
    CV_PROP_RW float convexHullFactor;
    CV_PROP_RW float minRNGEdgeSwitchDist;

    enum GridType { SYMMETRIC_GRID, ASYMMETRIC_GRID };
    GridType gridType;

    CV_PROP_RW float
        squareSize;  //!< Distance between two adjacent points. Used by CALIB_CB_CLUSTERING.
    CV_PROP_RW float
        maxRectifiedDistance;  //!< Max deviation from prediction. Used by CALIB_CB_CLUSTERING.
};

class Graph {
public:
    typedef std::set<size_t> Neighbors;
    struct Vertex {
        Neighbors neighbors;
    };
    typedef std::map<size_t, Vertex> Vertices;

    Graph(size_t n);
    void addVertex(size_t id);
    void addEdge(size_t id1, size_t id2);
    void removeEdge(size_t id1, size_t id2);
    bool doesVertexExist(size_t id) const;
    bool areVerticesAdjacent(size_t id1, size_t id2) const;
    size_t getVerticesCount() const;
    size_t getDegree(size_t id) const;
    const Neighbors &getNeighbors(size_t id) const;
    void floydWarshall(cv::Mat &distanceMatrix, int infinity = -1) const;

private:
    Vertices vertices;
};

struct Path {
    int firstVertex;
    int lastVertex;
    int length;

    std::vector<size_t> vertices;

    Path(int first = -1, int last = -1, int len = -1) {
        firstVertex = first;
        lastVertex = last;
        length = len;
    }
};

class CirclesGridFinder {
public:
    CirclesGridFinder(
        cv::Size patternSize,
        const std::vector<cv::Point2f> &testKeypoints,
        const CirclesGridFinderParameters &parameters = CirclesGridFinderParameters());
    bool findHoles();
    static cv::Mat rectifyGrid(cv::Size detectedGridSize,
                               const std::vector<cv::Point2f> &centers,
                               const std::vector<cv::Point2f> &keypoint,
                               std::vector<cv::Point2f> &warpedKeypoints);

    void getHoles(std::vector<cv::Point2f> &holes) const;
    void getAsymmetricHoles(std::vector<cv::Point2f> &holes) const;
    cv::Size getDetectedGridSize() const;

    void drawBasis(const std::vector<cv::Point2f> &basis,
                   cv::Point2f origin,
                   cv::Mat &drawImg) const;
    void drawBasisGraphs(const std::vector<Graph> &basisGraphs,
                         cv::Mat &drawImg,
                         bool drawEdges = true,
                         bool drawVertices = true) const;
    void drawHoles(const cv::Mat &srcImage, cv::Mat &drawImage) const;

private:
    void computeRNG(Graph &rng, std::vector<cv::Point2f> &vectors, cv::Mat *drawImage = 0) const;
    void rng2gridGraph(Graph &rng, std::vector<cv::Point2f> &vectors) const;
    void eraseUsedGraph(std::vector<Graph> &basisGraphs) const;
    void filterOutliersByDensity(const std::vector<cv::Point2f> &samples,
                                 std::vector<cv::Point2f> &filteredSamples);
    void findBasis(const std::vector<cv::Point2f> &samples,
                   std::vector<cv::Point2f> &basis,
                   std::vector<Graph> &basisGraphs);
    void findMCS(const std::vector<cv::Point2f> &basis, std::vector<Graph> &basisGraphs);
    size_t findLongestPath(std::vector<Graph> &basisGraphs, Path &bestPath);
    float computeGraphConfidence(const std::vector<Graph> &basisGraphs,
                                 bool addRow,
                                 const std::vector<size_t> &points,
                                 const std::vector<size_t> &seeds);
    void addHolesByGraph(const std::vector<Graph> &basisGraphs, bool addRow, cv::Point2f basisVec);

    size_t findNearestKeypoint(cv::Point2f pt) const;
    void addPoint(cv::Point2f pt, std::vector<size_t> &points);
    void findCandidateLine(std::vector<size_t> &line,
                           size_t seedLineIdx,
                           bool addRow,
                           cv::Point2f basisVec,
                           std::vector<size_t> &seeds);
    void findCandidateHoles(std::vector<size_t> &above,
                            std::vector<size_t> &below,
                            bool addRow,
                            cv::Point2f basisVec,
                            std::vector<size_t> &aboveSeeds,
                            std::vector<size_t> &belowSeeds);
    static bool areCentersNew(const std::vector<size_t> &newCenters,
                              const std::vector<std::vector<size_t>> &holes);
    bool isDetectionCorrect();

    static void insertWinner(float aboveConfidence,
                             float belowConfidence,
                             float minConfidence,
                             bool addRow,
                             const std::vector<size_t> &above,
                             const std::vector<size_t> &below,
                             std::vector<std::vector<size_t>> &holes);

    struct Segment {
        cv::Point2f s;
        cv::Point2f e;
        Segment(cv::Point2f _s, cv::Point2f _e);
    };

    // if endpoint is on a segment then function return false
    static bool areSegmentsIntersecting(Segment seg1, Segment seg2);
    static bool doesIntersectionExist(const std::vector<Segment> &corner,
                                      const std::vector<std::vector<Segment>> &segments);
    void getCornerSegments(const std::vector<std::vector<size_t>> &points,
                           std::vector<std::vector<Segment>> &segments,
                           std::vector<cv::Point> &cornerIndices,
                           std::vector<cv::Point> &firstSteps,
                           std::vector<cv::Point> &secondSteps) const;
    size_t getFirstCorner(std::vector<cv::Point> &largeCornerIndices,
                          std::vector<cv::Point> &smallCornerIndices,
                          std::vector<cv::Point> &firstSteps,
                          std::vector<cv::Point> &secondSteps) const;
    static double getDirection(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3);

    std::vector<cv::Point2f> keypoints;

    std::vector<std::vector<size_t>> holes;
    std::vector<std::vector<size_t>> holes2;
    std::vector<std::vector<size_t>> *largeHoles;
    std::vector<std::vector<size_t>> *smallHoles;

    const cv::Size_<size_t> patternSize;
    CirclesGridFinderParameters parameters;

    CirclesGridFinder &operator=(const CirclesGridFinder &);
    CirclesGridFinder(const CirclesGridFinder &);
};

/** @brief Finds centers in the grid of circles.

    @param image grid view of input circles; it must be an 8-bit grayscale or color image.
    @param patternSize number of circles per row and column
    ( patternSize = Size(points_per_row, points_per_colum) ).
    @param centers output array of detected centers.
    @param flags various operation flags that can be one of the following values:
    -   @ref CALIB_CB_SYMMETRIC_GRID uses symmetric pattern of circles.
    -   @ref CALIB_CB_ASYMMETRIC_GRID uses asymmetric pattern of circles.
    @param blobDetector feature detector that finds blobs like dark circles on light background.
                        If `blobDetector` is NULL then `image` represents Point2f array of
    candidates.
    @param parameters struct for finding circles in a grid pattern.

    The function attempts to determine whether the input image contains a grid of circles. If it is,
    the function locates centers of the circles. The function returns a non-zero value if all of the
    centers have been found and they have been placed in a certain order (row by row, left to right
    in every row). Otherwise, if the function fails to find all the corners or reorder them, it
    returns 0.

    Sample usage of detecting and drawing the centers of circles: :
    @code
        Size patternsize(7,7); //number of centers
        Mat gray = ...; //source image
        vector<Point2f> centers; //this will be filled by the detected centers

        bool patternfound = findCirclesGrid(gray, patternsize, centers);

        drawChessboardCorners(img, patternsize, Mat(centers), patternfound);
    @endcode
    @note The function requires white space (like a square-thick border, the wider the better)
    around the board to make the detection more robust in various environments.
     */
bool FindCirclesGrid(cv::InputArray image,
                     cv::Size patternSize,
                     cv::OutputArray centers,
                     int flags,
                     const cv::Ptr<cv::FeatureDetector> &blobDetector,
                     const ns_cv_helper::CirclesGridFinderParameters &parameters);
}  // namespace ns_cv_helper

#endif /* EKALIBR_CIRCLESGRID_HPP */

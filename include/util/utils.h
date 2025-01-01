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

#ifndef UTILS_H
#define UTILS_H

#include "veta/type_def.hpp"
#include "array"
#include "opencv4/opencv2/core.hpp"
#include "Eigen/Dense"
#include "magic_enum.hpp"
#include "random"
#include "map"
#include "set"

namespace ns_ekalibr {

// config the 'spdlog' log pattern
void ConfigSpdlog();

void PrintIKalibrLibInfo();

bool TryCreatePath(const std::string &path);

// given n points and a x value, compute the y value using lagrange polynomial
template <class Type, int N>
double LagrangePolynomial(Type xQuery,
                          const std::array<Type, N> &xData,
                          const std::array<Type, N> &yData);

// given three points, compute the first order of the middle point using lagrange polynomial
template <class Type>
Type LagrangePolynomialTripleMidFOD(const std::array<Type, 3> &xData,
                                    const std::array<Type, 3> &yData);

// given three points, compute the first order of the middle point using lagrange polynomial
template <class Type>
Type LagrangePolynomialTripleMidFOD(const std::array<std::pair<Type, Type>, 3> &data);

template <class Type>
Type GetParamFromROS(const std::string &param);

std::string UpperString(std::string s);

std::string LowerString(std::string s);

void DrawKeypointOnCVMat(cv::Mat &img,
                         const Eigen::Vector2d &feat,
                         bool withBox = true,
                         const cv::Scalar &color = cv::Scalar(0, 255, 0));

void DrawKeypointOnCVMat(cv::Mat &img,
                         const cv::Point2d &feat,
                         bool withBox = true,
                         const cv::Scalar &color = cv::Scalar(0, 255, 0));

void DrawLineOnCVMat(cv::Mat &img,
                     const Eigen::Vector2d &p1,
                     const Eigen::Vector2d &p2,
                     const cv::Scalar &color = cv::Scalar(0, 255, 0));

void DrawLineOnCVMat(cv::Mat &img,
                     const cv::Point2d &p1,
                     const cv::Point2d &p2,
                     const cv::Scalar &color = cv::Scalar(0, 255, 0));

void PutTextOnCVMat(cv::Mat &img,
                    const std::string &str,
                    const cv::Point2d &pt,
                    double xBias = 10.0,
                    double yBias = 0.0,
                    const cv::Scalar &color = cv::Scalar(255, 0, 0));

void PutTextOnCVMat(cv::Mat &img,
                    const std::string &str,
                    const Eigen::Vector2d &pt,
                    double xBias = 10.0,
                    double yBias = 0.0,
                    const cv::Scalar &color = cv::Scalar(255, 0, 0));

using namespace magic_enum::bitwise_operators;

template <class EnumType>
bool IsOptionWith(EnumType desired, EnumType current);

bool IsNotWhiteSpace(int character);

void StringLeftTrim(std::string *str);

void StringRightTrim(std::string *str);

void StringTrim(std::string *str);

std::string GetIndexedFilename(int idx, int num);

Eigen::MatrixXd TangentBasis(const Eigen::Vector3d &g0);

/**
 * @brief sampling the samples without replacement
 *
 * @param num the num of the samples to sampling
 * @param engine the random engine
 * @param start the start index
 * @param end the end index
 * @param step the step
 * @attention range: [start, end](step) i.e. for [1, 5](2) -> pool: {1, 3, 5}
 * @return std::vector<std::size_t>
 */
std::vector<std::size_t> SamplingWoutReplace(std::default_random_engine &engine,
                                             std::size_t num,
                                             std::size_t start,
                                             std::size_t end,
                                             std::size_t step = 1);

/**
 * @brief sampling the samples without replacement
 *
 * @tparam ElemType the element type
 * @param engine the random engine
 * @param dataVec the data vector
 * @param num the num of the samples to sampling
 * @return std::vector<std::size_t>
 */
template <typename ElemType>
std::vector<std::size_t> SamplingWoutReplace(std::default_random_engine &engine,
                                             const std::vector<ElemType> &dataVec,
                                             std::size_t num);

/**
 * @brief sampling the samples without replacement
 *
 * @tparam ElemType the element type
 * @param engine the random engine
 * @param dataVec the data vector
 * @param num the num of the samples to sampling
 * @return std::vector<ElemType>
 */
template <typename ElemType>
std::vector<ElemType> SamplingWoutReplace2(std::default_random_engine &engine,
                                           const std::vector<ElemType> &dataVec,
                                           std::size_t num);

/**
 * @brief sampling the samples with replacement
 *
 * @param engine the random engine
 * @param num the num of the samples to sampling
 * @param start the start index
 * @param end the end index
 * @param step the step
 * @attention range: [start, end](step) i.e. for [1, 5](2) -> pool: {1, 3, 5}
 * @return std::vector<std::size_t>
 */
std::vector<std::size_t> SamplingWithReplace(std::default_random_engine &engine,
                                             std::size_t num,
                                             std::size_t start,
                                             std::size_t end,
                                             std::size_t step = 1);

/**
 * @brief sampling the samples with replacement
 *
 * @tparam ElemType the element type
 * @param engine the random engine
 * @param dataVec the data vector
 * @param num the num of the samples to sampling
 * @return std::vector<std::size_t>
 */
template <typename ElemType>
std::vector<std::size_t> SamplingWithReplace(std::default_random_engine &engine,
                                             const std::vector<ElemType> &dataVec,
                                             std::size_t num);

/**
 * @brief sampling the samples with replacement
 *
 * @tparam ElemType the element type
 * @param engine the random engine
 * @param dataVec the data vector
 * @param num the num of the samples to sampling
 * @return std::vector<ElemType>
 */
template <typename ElemType>
std::vector<ElemType> SamplingWithReplace2(std::default_random_engine &engine,
                                           const std::vector<ElemType> &dataVec,
                                           std::size_t num);

Eigen::Vector3d RotMatToYPR(const Eigen::Matrix3d &R);

double NormalizeAngle(double ang_degree);

template <typename Derived>
Eigen::Matrix<typename Derived::Scalar, 3, 3> SkewSymmetric(const Eigen::MatrixBase<Derived> &v3d);

template <typename Derived>
Eigen::Matrix<typename Derived::Scalar, 4, 4> LeftQuatMatrix(
    const Eigen::QuaternionBase<Derived> &q);

template <typename Derived>
Eigen::Matrix<typename Derived::Scalar, 4, 4> RightQuatMatrix(
    const Eigen::QuaternionBase<Derived> &p);

struct RetrieveKey {
    template <typename T>
    typename T::first_type operator()(T keyValuePair) const {
        return keyValuePair.first;
    }
};

struct RetrieveVal {
    template <typename T>
    typename T::first_type operator()(T keyValuePair) const {
        return keyValuePair.second;
    }
};

template <typename KeyType, typename ValueType>
std::vector<KeyType> ExtractKeysAsVec(const std::map<KeyType, ValueType> &inputMap);

template <typename KeyType, typename ValueType>
std::set<KeyType> ExtractKeysAsSet(const std::map<KeyType, ValueType> &inputMap);

template <typename KeyType, typename ValueType>
std::vector<ValueType> ExtractValsAsVec(const std::map<KeyType, ValueType> &inputMap);

template <typename KeyType, typename ValueType>
std::set<ValueType> ExtractValsAsSet(const std::map<KeyType, ValueType> &inputMap);

template <typename ScaleType>
std::string FormatValueVector(const std::vector<const char *> &descVec,
                              const std::vector<ScaleType> &valVec,
                              const char *scaleFormatStr = "{:+011.6f}");

/**
 * @brief a function to get all the filenames in the directory
 * @param directory the directory
 * @return the filenames in the directory
 */
std::vector<std::string> FilesInDir(const std::string &directory, bool sort = false);

std::vector<std::string> FilesInDirRecursive(const std::string &directory, bool sort = false);

/**
 * @brief a function to split a string to some string elements according the splitor
 * @param str the string to be split
 * @param splitor the splitor char
 * @param ignoreEmpty whether ignoring the empty string element or not
 * @return the split string vector
 */
std::vector<std::string> SplitString(const std::string &str, char splitor, bool ignoreEmpty = true);

template <typename Scale, int Rows, int Cols>
Eigen::Matrix<Scale, Rows, Cols> TrapIntegrationOnce(
    const std::vector<std::pair<Scale, Eigen::Matrix<Scale, Rows, Cols>>> &data);

template <typename Scale, int Rows, int Cols>
Eigen::Matrix<Scale, Rows, Cols> TrapIntegrationTwice(
    const std::vector<std::pair<Scale, Eigen::Matrix<Scale, Rows, Cols>>> &data);

template <typename EigenVectorType>
auto EigenVecXToVector(const EigenVectorType &eigenVec);

template <class Scale, int Rows, int Cols>
Eigen::Matrix<Scale, Rows, Cols> ComputeMatVecMean(
    const std::vector<Eigen::Matrix<Scale, Rows, Cols>> &vec);

template <class Type>
Type ComputeNumericalMean(const std::vector<Type> &vec);

template <class KeyType, class ValueType>
std::vector<ValueType> ValueVecFromMap(const std::map<KeyType, ValueType> &m);

}  // namespace ns_ekalibr

#endif  // UTILS_H
